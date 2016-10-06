#include <iostream>
#include <math.h>
#include <list>
#include <map>
#include <set>
#include "TablePattern.h"
#include <queue>
#include <algorithm>

//CKEntry keeps the correspondecen between a table column and a KB type


CKEntry::CKEntry(string col,URI type,double score) {
	this->colName = col;
	this->type = type;
	this->score = score;
}	
string CKEntry::getColName() const{
	return this-> colName;
}

URI CKEntry::getType() const{
	return this-> type;
}

double CKEntry::getScore() const{
	return this->score;
}


PKEntry::PKEntry(string subCol,string objCol, URI relation,double score) {
	this->subColName = subCol;
	this->objColName = objCol;
	this->relation = relation;
	this->score = score;
}	
string PKEntry::getSubColName() const{
	return this->subColName;
}

string PKEntry::getObjColName() const{
	return this->objColName;
}

URI PKEntry::getRelation() const{
	return this-> relation;
}

double PKEntry::getScore() const{
	return this->score;
}

vector<TablePattern> TPGenerator::generatePatterns(const WebTable* WebTable, int num) const {
	return vector<TablePattern>();
}

queue<CKEntry> TPGenerator::getColTypes(string colHeader, vector<string> values){
	queue<CKEntry> typeQ;
	set<URI> types;

	//key: col value
	//value: a list of candidate type url for that value
	map<string, list<URI>>	typeMap;

	for (string value: values) {
		if(typeMap.find(value) != typeMap.end()) continue; 
		list<URI> cTypes = this->KBptr->listCandidateTypes(value);

		// cout << "Value: " << value << endl;

		// for (URI type:cTypes) cout << type << endl;
		if (cTypes.size() == 0 ) continue;

		typeMap[value] = cTypes;

		for(URI type:cTypes) types.insert(type);
	}
	
	for(URI type: types) {
		double score = 0.0;
		int kbTypeCount = this->KBptr->getNumberOfTypes();
		int typeInstanceCount = this->KBptr->countType(type);

		cout << "Type: " << type << " ";
		cout << "tiCOunt" << typeInstanceCount << endl;

		for(string value: values) {
			if(typeMap.find(value) == typeMap.end()) continue; 
			list<URI> ctypes = typeMap[value];
			if(find(ctypes.begin(),ctypes.end(),type) == ctypes.end()) continue;


			int ctypeCount = ctypes.size();
			score += (1.0 / log((double) typeInstanceCount)) * (log((double)kbTypeCount / (double) ctypeCount));
			cout << "S:" << score <<endl;

		}
		CKEntry ck(colHeader,type,score);
		typeQ.push(ck);
	}
	return typeQ;
}
queue<PKEntry> getPairRels(string col1, vector<string> values1, string col2, vector<string> values2){
}