#include "IKnowledgeBase.h"
#include "WebTable.h"
#include <queue>
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
private:
	
	vector<CKEntry> ckEntries;
	vector<PKEntry> pkEntries;
	double score;
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
	vector<TablePattern> generatePatterns(const WebTable* webTable,int num) const;

	priority_queue<CKEntry> getColTypes(string colHeader, vector<string> values);
	priority_queue<PKEntry> getPairRels(string col1, vector<string> values1, string col2, vector<string> values2);
	void getCoherenceScore(const URI type, const URI rel, double* subScore, double* objScore);
};


int shareURICount(list<URI> l1, list<URI> l2);
