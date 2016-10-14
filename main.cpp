#include <iostream>
#define DEBUG
#include "debug.h"
#include <fstream>
#include "KnowledgeBase.h"
#include "TablePattern.h"
#include <vector>
#include <queue>
#include <SchemaMatching.h>
#include <algorithm>
// #include "include/IKnowledgeBase.h"

using namespace std;
WebTable getExampleWebTable();
void show_typeDistribution();
void show_getColTypes();
void show_getPairRel();
void show_cohScore();
void show_genTP();
int main(){
	//show_getPairRel();
//		show_cohScore();
	show_genTP();
	show_typeDistribution();

}
void show_typeDistribution() {

	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	WebTable wt = getExampleWebTable();
	TPGenerator tpGen(&kb);
	priority_queue<TablePattern> tps = tpGen.generatePatterns(wt, 2);

	cout << "Table Pattern Size: " <<tps.size() << endl;
	vector<TablePattern> tpv;
	while(!tps.empty()) {
		TablePattern tp = tps.top();
		tpv.push_back(tp);
		tps.pop();
	}
//	map<TablePattern,double> tpProb = getTpProbability(tpv);
//	for(auto tpProbIt = tpProb.begin();tpProbIt != tpProb.end();tpProbIt++) {
//		cout << tpProbIt->first << endl;
//		cout << "Prob: " << tpProbIt->second << endl;
//	}
	map<string,map<URI,double>> result = getColTypeDistribution(tpv);

	//Display the mapping url and probability of each column
	cout << "=================================================" << endl;
	for(auto colMapIt = result.begin();colMapIt != result.end();colMapIt++) {
		cout <<"Col Name: " << colMapIt->first << endl;
		map<URI,double> urlMapping = colMapIt->second;
		for (auto urlMapIt = urlMapping.begin();urlMapIt != urlMapping.end();urlMapIt++) {
			cout << "\t" << "[" << urlMapIt->second <<"] " << urlMapIt->first << endl;
		}
		cout << "---------------------------------------------" << endl;
	}

}
void show_genTP() {

	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	WebTable wt = getExampleWebTable();
	TPGenerator tpGen(&kb);
	priority_queue<TablePattern> tps = tpGen.generatePatterns(wt, 2);

	cout << "Table Pattern Size: " <<tps.size() << endl;
//	TablePattern tp = tps.top();
//	for (CKEntry ck: tp.getCKEntries()) {
//		cout <<ck<< endl;
//	}
//	for (PKEntry pk: tp.getPKEntries()) {
//		cout <<pk<< endl;
//	}
	while(!tps.empty()) {
		TablePattern tp = tps.top();
		tps.pop();
		cout << tp<< endl << endl;
	}
}
WebTable getExampleWebTable() {
	//An example web table in Kartara Implemenataion

	WebTable wt("Kartara");
	vector<string> col1 = { "Rossi", "Klate", "Pirlo" };
	vector<string> col2 = { "Italy", "China", "Japan" };
	vector<string> col3 = { "Rome", "Beijing", "Toky" };

	wt.addCol(col1);
	wt.addCol(col2);
	wt.addCol(col3);
	return wt;
}


void show_getColTypes(){

	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	WebTable wt = getExampleWebTable();
	vector<string> value = wt.getColValues("Kartara_1");

	TPGenerator tpGen(&kb);
	priority_queue<CKEntry> q = tpGen.getColTypes("Kartara_1", value);
	cout << q.size() << endl;
	while (!q.empty()) {
		CKEntry e = q.top();
		cout <<e<<endl;
		q.pop();
	}
}


void show_getPairRel() {

	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	WebTable wt = getExampleWebTable();
	vector<string> value = wt.getColValues("Kartara_1");

	 vector<string>value2 = wt.getColValues("Kartara_2");
	 TPGenerator tpGen(&kb);

     priority_queue<PKEntry> relations = tpGen.getPairRels("Kartara_1",value,"Kartara_2",value2);
     cout <<"Candidate Rel Size:" << relations.size() <<endl;
     while(!relations.empty()) {
    	 PKEntry e = relations.top();
    	 relations.pop();
    	 cout << e <<endl;
     }
}

void show_cohScore() {

	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");

	URI type = kb.listCandidateTypes("Beijing").front();
	URI rel = kb.listCandidateRelations("China","Beijing").front();

	cout <<type<<endl;
	cout <<rel <<endl;

	 TPGenerator tpGen(&kb);
	 double subScore;
	 double objScore;
	 tpGen.getCoherenceScore(type,rel,&subScore,&objScore);
	 cout <<"SubScore: "<<subScore << endl;
	 cout <<"ObjScore: " << objScore << endl;
}
