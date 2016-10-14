#include <iostream>
#include <math.h>
#include <list>
#include <map>
#include <set>
#include "TablePattern.h"
#include <queue>
#include <algorithm>
#include <assert.h>
#define DEBUG
#include <debug.h>
//CKEntry keeps the correspondence between a table column and a KB type

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

//Currently don't use num
priority_queue<TablePattern> TPGenerator::generatePatterns(const WebTable& wt,
		int num) const{
	vector<string> colHeaders = wt.getColHeaders();
	vector<priority_queue<CKEntry>> colCKEntries;
	vector<priority_queue<PKEntry>> pairPKEntries;

	for (string col1:colHeaders) {
		vector<string> values1 = wt.getColValues(col1);
		priority_queue<CKEntry> ckEntries = this->getColTypes(col1,values1);
		if(!ckEntries.empty())	colCKEntries.push_back(ckEntries);

		for(string col2:colHeaders) {
			if(col1.compare(col2)==0) continue;
			vector<string> values2 = wt.getColValues(col2);
			priority_queue<PKEntry> q1 = this->getPairRels(col1, values1, col2, values2);
			if(!q1.empty()) pairPKEntries.push_back(q1);

	    }


	}
//	DEBUG_STDOUT(colCKEntries.size());
//	DEBUG_STDOUT(pairPKEntries.size());
	vector<priority_queue<CKEntry>>::iterator colCKIt = colCKEntries.begin();
	vector<priority_queue<PKEntry>>::iterator pairPKIt = pairPKEntries.begin();

	vector<priority_queue<CKEntry>>::iterator colEndCKIt = colCKEntries.end();
	vector<priority_queue<PKEntry>>::iterator pairEndPKIt = pairPKEntries.end();

//	for(;pairPKIt != pairEndPKIt;pairPKIt++) {
//		cout << "hi " <<endl;
//	}
//	pairPKIt = pairPKEntries.begin();
	list<CKEntry> ckEntries;
	list<PKEntry> pkEntries;
	priority_queue<TablePattern> tpQ;

	this->genTPRecursively(colCKIt, pairPKIt, colEndCKIt, pairEndPKIt, ckEntries, pkEntries, &tpQ);
	return tpQ;
}

priority_queue<CKEntry> TPGenerator::getColTypes(string colHeader,
		vector<string> values) const{
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

		}
		if(score > 0.0){
			CKEntry ck(colHeader, type, score);
			typeQ.push(ck);
		}

	}
	return typeQ;
}
priority_queue<PKEntry> TPGenerator::getPairRels(string col1,
		vector<string> values1, string col2, vector<string> values2) const{
	assert(values1.size() == values2.size());
	priority_queue<PKEntry> relQ;

	set<URI> rels;
	map<string, list<URI>> relMap;

	int size = values1.size();
	for (int i = 0; i < size; i++) {
		string v1 = values1.at(i);
		string v2 = values2.at(i);

		string key = v1 + "_" + v2;
		if (relMap.find(key) != relMap.end())
			continue;

		list<URI> cRels = this->KBptr->listCandidateRelations(v1, v2);
		if (cRels.size() == 0)
			continue;

		relMap[key] = cRels;
		for (URI rel : cRels)
			rels.insert(rel);
	}

	int numRel = this->KBptr->getNumberOfRelations();
	//TODO Print
	//DEBUG_STDOUT(size);
	for (URI rel : rels) {
		double score = 0.0;
		int relationPairCount = this->KBptr->getSubjectEntites(rel).size();

		for (int i = 0; i < size; i++) {
			string v1 = values1.at(i);
			string v2 = values2.at(i);
			string key = v1 + "_" + v2;
			if (relMap.find(key) == relMap.end())
				continue;
			list<URI> cRels = relMap[key];
			int cRelsCount = cRels.size();
			if (cRelsCount == 0)
				continue;
			if (find(cRels.begin(), cRels.end(), rel) == cRels.end())
				continue;
			//DEBUG_STDOUT(numRel);
			score += (1.0 / log((double) relationPairCount))
					* (log(double(numRel) / (double) cRelsCount));

		} //End of for i
		if(score > 0.0){
			PKEntry pkEntry(col1, col2, rel, score);
			relQ.push(pkEntry);
		}
	}
	return relQ;
}

void TPGenerator::getCoherenceScore(const URI type, const URI rel,
		double* subScore, double* objScore) const{

	list<URI> entities = this->KBptr->getEntites(type);
	list<URI> subEntity = this->KBptr->getSubjectEntites(rel);
	list<URI> objEntity = this->KBptr->getObjectEntites(rel);

	int typeCount = this->KBptr->getNumberOfTypes();

	double tProb = (double) entities.size() / (double) typeCount;
	double subProb = (double) subEntity.size() / (double) typeCount;
	double objProb = (double) objEntity.size() / (double) typeCount;

	double tSubProb = (double) shareURICount(subEntity, entities)
			/ (double) typeCount;
	double tObjProb = (double) shareURICount(objEntity, entities)
			/ (double) typeCount;
//	DEBUG_STDOUT(tProb);
//	DEBUG_STDOUT(subProb);
//	DEBUG_STDOUT(objProb);
//
//	DEBUG_STDOUT(tSubProb);
//	DEBUG_STDOUT(tObjProbw);
	*subScore = tSubProb<1e-3?0:
			((log((tSubProb) / (tProb * subProb)) / (tSubProb * (-1))) + 1.0)
					/ 2.0;
	*objScore = tObjProb<1e-3?0:
			((log((tObjProb) / (tProb * objProb)) / (tObjProb * (-1))) + 1.0)
					/ 2.0;

}

double TPGenerator::computeScore(const TablePattern& tp) const{
	double score = 0.0;
	vector<CKEntry> ckEntries = tp.getCKEntries();
	vector<PKEntry> pkEntries = tp.getPKEntries();

	for(CKEntry ck: ckEntries) {
		score += ck.getScore();
	}
	for(PKEntry pk: pkEntries) {
		score += pk.getScore();
	}


	for(CKEntry ck: ckEntries) {
		string cid = ck.getColName();
		URI colType = ck.getType();

		for(PKEntry pk: pkEntries) {
			string subCid = pk.getSubColName();
			string objCid = pk.getObjColName();

			URI rel = pk.getRelation();

			//if cid is not equal to one of subCid and objCid, continue
			if(cid.compare(subCid) != 0 && cid.compare(objCid) != 0 ) continue;
			double subScore,objScore;
			this->getCoherenceScore(colType, rel, &subScore, &objScore);

			score += subScore +objScore;
		}//End for PKEntry
	}//End for for CKEntry
	return score;
}

void TPGenerator::genTPRecursively(
					  vector<priority_queue<CKEntry>>::iterator colCKIt,
					  vector<priority_queue<PKEntry>>::iterator pairPKIt,
					  vector<priority_queue<CKEntry>>::iterator colEndIt,
					  vector<priority_queue<PKEntry>>::iterator pairEndIt,
					  list<CKEntry> ckEntries,
					  list<PKEntry> pkEntries,
					  priority_queue<TablePattern>* tpQ) const{
//	DEBUG_STDOUT(ckEntries.size());
//	DEBUG_STDOUT(pkEntries.size());
//	if(colCKIt == colEndIt) cout << "End of Col" << endl;
//	if(pairPKIt == pairEndIt) cout << "End of Pair" << endl;
//	cout << "&&--------------"<< endl;
//	for(CKEntry cks: ckEntries) {
//		cout << cks.getColName() << endl;
//	}
//
//	for(PKEntry pks: pkEntries) {
//		cout << pks.getSubColName() << "," << pks.getObjColName() << endl;
//	}
//	cout << "----------------" <<endl;
	if(colCKIt != colEndIt) {
//		DEBUG_STDOUT("CK");
		priority_queue<CKEntry>  ckEntryQ = *colCKIt;
		while(!ckEntryQ.empty()) {
			CKEntry ckEntry = ckEntryQ.top();
			ckEntryQ.pop();
			ckEntries.push_back(ckEntry);
			vector<priority_queue<CKEntry>>::iterator newColCKIt = colCKIt + 1;
			this->genTPRecursively(newColCKIt, pairPKIt, colEndIt, pairEndIt, ckEntries, pkEntries, tpQ);
			ckEntries.pop_back();
		}
		return;
	}
	if(pairPKIt != pairEndIt){
//		DEBUG_STDOUT("PK");
		priority_queue<PKEntry>  pkEntryQ = *pairPKIt;
		while(!pkEntryQ.empty()) {
			PKEntry pkEntry = pkEntryQ.top();
			pkEntryQ.pop();
			pkEntries.push_back(pkEntry);
			vector<priority_queue<PKEntry>>::iterator newPairPKIt = pairPKIt + 1;
			this->genTPRecursively(colCKIt, newPairPKIt, colEndIt, pairEndIt, ckEntries, pkEntries, tpQ);
			pkEntries.pop_back();
		}
		return;
	}
		TablePattern tp;
		for (CKEntry ckEntry:ckEntries) {
			tp.addCKEntry(ckEntry);
//			if(ckEntry.getScore() < 1e-6) cout << "Error here" <<endl;
		}

		for(PKEntry pkEntry:pkEntries) {
			tp.addPKEntry(pkEntry);
		}
		double score = this->computeScore(tp);
//		cout << "Score: " << score << endl;
		tp.setScore(score);
		tpQ->push(tp);
//		cout <<"TPQ size: " << tpQ->size()<<endl;
}
//return the number of shared elements in two lists of URI

int shareURICount(list<URI> l1, list<URI> l2) {
	int r = 0;
	for (auto e : l1) {
		if (find(l2.begin(), l2.end(), e) != l2.end()) {
			r++;
		}
	}
	return r;
}


