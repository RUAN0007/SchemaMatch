#pragma once
#include "IKnowledgeBase.h"
#include "WebTable.h"
#include <queue>
#include <iostream>
using namespace std;

//TKEntry keeps the correspondecen between a table column and a KB type

class CKEntry {
private: 
	string colName;
	URI type;
	double score;

public:
	CKEntry(string col,URI type,double score);
	string getColName() const;

	URI getType() const;

	double getScore() const;

	friend inline std::ostream & operator<<(std::ostream & os, CKEntry const & v) {

		os <<  "ColID: ";
		os << v.colName << endl;
		os << "URI:"<< v.type << endl;
		os << "Score: " << v.score;
		return os;
	}

	friend inline bool operator< (const CKEntry& lhs, const CKEntry& rhs){ return lhs.score < rhs.score;};

	friend inline bool operator> (const CKEntry& lhs, const CKEntry& rhs){ return lhs.score > rhs.score;}
};

//PK Entry keeps a correspondence between a col pair with a KB relation

class PKEntry {
private: 
	string subColName;
	string objColName;
	URI relation;
	double score;

public:
	PKEntry(){};
	~PKEntry(){};
	PKEntry(string subCol,string objCol, URI relation,double score);
	string getSubColName() const;

	string getObjColName() const;

	URI getRelation() const;

	double getScore() const;

	friend inline std::ostream & operator<<(std::ostream & Str, PKEntry const & v) {
		Str << "SubColID: ";
			Str << v.subColName <<" ";
			Str <<  "ObjColID: ";
			Str << v.objColName <<endl;
			Str << "URI:"<< v.relation << endl;
			Str << "Score: " << v.score;
		return Str;
	}
	friend inline bool operator< (const PKEntry& lhs, const PKEntry& rhs){ return lhs.score < rhs.score;};

	friend inline bool operator> (const PKEntry& lhs, const PKEntry& rhs){ return lhs.score > rhs.score;}
};

class TablePattern {
public:
	inline vector<CKEntry> getCKEntries() const {
		return this->ckEntries;
	};
	inline vector<PKEntry> getPKEntries() const {
		return this->pkEntries;
	}
	inline void addCKEntry(CKEntry ckEntry){
		this->ckEntries.push_back(ckEntry);
	}
	inline void addPKEntry (PKEntry pkEntry){
		this->pkEntries.push_back(pkEntry);
	}
	inline void setScore(double score) {
		this->score = score;
	}
	inline double getScore() {
		return this->score;	
	}

	inline void setProbability(double probability) {
		this->probability = probability;
	}
	inline double getProbability() {
		return this->probability;
	}
	friend inline bool operator< (const TablePattern& lhs, const TablePattern& rhs){ return lhs.score < rhs.score;};

	friend inline bool operator> (const TablePattern& lhs, const TablePattern& rhs){ return lhs.score > rhs.score;};

	friend inline std::ostream & operator<<(std::ostream & os, TablePattern const & tp) {

		for (CKEntry ckEntry:tp.getCKEntries()) {
			os << ckEntry.getColName() << "[" << ckEntry.getScore() << "]->" <<ckEntry.getType() <<endl;
		}
		for (PKEntry pkEntry:tp.getPKEntries()) {
			os << "(" << pkEntry.getSubColName() <<"," << pkEntry.getObjColName() << ")";
			os << "[" << pkEntry.getScore() << "]->" <<pkEntry.getRelation() <<endl;
		}
		os << "Score: " << tp.score ;
		return os;
	}
private:
	
	vector<CKEntry> ckEntries;
	vector<PKEntry> pkEntries;
	double score = -1; //-1 implies the score has not been computed yet.
	double probability = 0.0; //-1 implies the probability has not been set
};

class TPGenerator {
private: 
	 IKB* KBptr;

public: 
	inline TPGenerator(IKB* kbPtr) {
		this->KBptr = kbPtr;
	}

	//generate the table patterns for the given web table using the knowledge base
	//num: the number of received table patterns with the highest point
	priority_queue<TablePattern> generatePatterns(const WebTable& webTable,int num) const;

	priority_queue<CKEntry> getColTypes(string colHeader, vector<string> values) const;
	priority_queue<PKEntry> getPairRels(string col1, vector<string> values1, string col2, vector<string> values2) const;
	void getCoherenceScore(const URI type, const URI rel, double* subScore, double* objScore) const;
	double computeScore(const TablePattern& tp) const;

	void genTPRecursively(
					  vector<priority_queue<CKEntry>>::iterator colCKIt,
					  vector<priority_queue<PKEntry>>::iterator pairPKIt,
					  vector<priority_queue<CKEntry>>::iterator colEndIt,
					  vector<priority_queue<PKEntry>>::iterator pairEndIt,
					  list<CKEntry> ckEntries,
					  list<PKEntry> pkEntries,
					  priority_queue<TablePattern>* tpQ) const;
};


int shareURICount(list<URI> l1, list<URI> l2);
