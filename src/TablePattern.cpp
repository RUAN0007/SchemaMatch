#include <iostream>
#include <math.h>
#include <list>
#include <map>
#include <set>
#include "TablePattern.h"
#include <queue>
#include <algorithm>
#include <assert.h>

//CKEntry keeps the correspondence between a table column and a KB type
int shareURICount(list<URI> l1, list<URI> l2);

CKEntry::CKEntry(string col, URI type, double score) {
	this->colName = col;
	this->type = type;
	this->score = score;
}
string CKEntry::getColName() const {
	return this->colName;
}

URI CKEntry::getType() const {
	return this->type;
}

double CKEntry::getScore() const {
	return this->score;
}

PKEntry::PKEntry(string subCol, string objCol, URI relation, double score) {
	this->subColName = subCol;
	this->objColName = objCol;
	this->relation = relation;
	this->score = score;
}
string PKEntry::getSubColName() const {
	return this->subColName;
}

string PKEntry::getObjColName() const {
	return this->objColName;
}

URI PKEntry::getRelation() const {
	return this->relation;
}

double PKEntry::getScore() const {
	return this->score;
}

vector<TablePattern> TPGenerator::generatePatterns(const WebTable* WebTable,
		int num) const {
	return vector<TablePattern>();
}

priority_queue<CKEntry> TPGenerator::getColTypes(string colHeader,
		vector<string> values) {
	priority_queue<CKEntry> typeQ;
	set<URI> types;

	//key: col value
	//value: a list of candidate type url for that value
	map<string, list<URI>> typeMap;

	for (string value : values) {
		if (typeMap.find(value) != typeMap.end())
			continue;
		list<URI> cTypes = this->KBptr->listCandidateTypes(value);

		// cout << "Value: " << value << endl;

		// for (URI type:cTypes) cout << type << endl;
		if (cTypes.size() == 0)
			continue;

		typeMap[value] = cTypes;

		for (URI type : cTypes)
			types.insert(type);
	}

	for (URI type : types) {
		double score = 0.0;
		int kbTypeCount = this->KBptr->getNumberOfTypes();
		int typeInstanceCount = this->KBptr->countType(type);

		for (string value : values) {
			if (typeMap.find(value) == typeMap.end())
				continue;
			list<URI> ctypes = typeMap[value];
			if (find(ctypes.begin(), ctypes.end(), type) == ctypes.end())
				continue;

			int ctypeCount = ctypes.size();
			score += (1.0 / log((double) typeInstanceCount))
					* (log((double) kbTypeCount / (double) ctypeCount));
			cout << "S:" << score << endl;

		}
		CKEntry ck(colHeader, type, score);
		typeQ.push(ck);
	}
	return typeQ;
}
priority_queue<PKEntry> TPGenerator::getPairRels(string col1,
		vector<string> values1, string col2, vector<string> values2) {
	assert(values1.size() == values2.size());
	priority_queue<PKEntry> relQ;

	set<URI> rels;
	map<string, list<URI>> relMap;

	int size = values1.size();

	for (int i = 0; i < size; i++) {
		string v1 = values1.at(i);
		string v2 = values2.at(i);

		string key = v1 + "_" + v2;
		if (relMap.find(key) == relMap.end())
			continue;

		list<URI> cRels = this->KBptr->listCandidateRelations(v1, v2);
		if (cRels.size() == 0)
			continue;

		relMap[key] = cRels;
		for (URI rel : cRels)
			rels.insert(rel);
	}

	int numRel = this->KBptr->getNumberOfRelations();

	for (URI rel : rels) {
		double score = 0.0;
		int relationPairCount = 1; //TODO later query KB API

		for (int i = 0; i < size; i++) {
			string v1 = values1.at(1);
			string v2 = values2.at(2);
			string key = v1 + "_" + v2;
			if (relMap.find(key) == relMap.end())
				continue;
			list<URI> cRels = relMap[key];
			int cRelsCount = cRels.size();
			if (cRelsCount == 0)
				continue;
			if (find(cRels.begin(), cRels.end(), rel) == cRels.end())
				continue;

			score += (1.0 / log((double) relationPairCount))
					* (log(double(numRel) / (double) cRelsCount));

		} //End of for i

		PKEntry pkEntry(col1, col2, rel, score);
		relQ.push(pkEntry);
	}
	return relQ;
}

void TPGenerator::getCoherenceScore(const URI type,
									const URI rel,
									double* subScore,
									double* objScore) {

	list<URI> entities = this->KBptr->getEntites(type);
	list<URI> subEntity = this->KBptr->getSubjectEntites(rel);
	list<URI> objEntity = this->KBptr->getObjectEntites(rel);

	int typeCount = this->KBptr->getNumberOfTypes();

	double tProb = (double)entities.size() / (double)typeCount;
	double subProb = (double)subEntity.size() / (double)typeCount;
	double objProb = (double)objEntity.size() / (double)typeCount;

	double tSubProb = (double)shareURICount(subEntity,entities) / (double) typeCount;
	double tObjProb = (double)shareURICount(objEntity, entities) / (double) typeCount;

	*subScore = ((log((tSubProb)/(tProb * subProb))/(tSubProb * (-1))) + 1.0)/2.0;
	*objScore = ((log((tObjProb)/(tProb * objProb))/(tObjProb * (-1))) + 1.0)/2.0;

}

//return the number of shared elements in two lists of URI

int shareURICount(list<URI> l1, list<URI> l2) {
	int r = 0;
	for(auto e:l1) {
		if(find(l2.begin(),l2.end(),e) != l2.end()) {
			r++;
		}
	}
	return r;
}


