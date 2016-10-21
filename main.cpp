#include <iostream>
//#define NDEBUG
#include "debug.h"
#include <fstream>
#include "KnowledgeBase.h"
#include "TablePattern.h"
#include <vector>
#include <queue>
#include <SchemaMatching.h>
#include <algorithm>
#include "include/SchemaMatching.h"
#include "json.h"
using namespace std;
WebTable getExampleWebTable(string name="default_table");
WebTable getExampleWebTable1(string name="default_table");
WebTable getExampleWebTable2(string name="default_table");
void show_typeDistribution();
void show_getColTypes();
void show_getPairRel();
void show_cohScore();
void show_genTP();
void show_matchSchema();
void show_serialize();
void show_crowdTablePattern();
void show_crowSchemaMatching();
int main(){
	//show_getPairRel();
//		show_cohScore();
//	show_genTP();
//	cout << endl << endl;
//	show_typeDistribution();
	//show_matchSchema();
//	show_serialize();
	show_crowSchemaMatching();
//	LOG(LOG_DEBUG,"%d %s",2,"test string");
	//Panic("%s","stop now!");
//	assert(2 == 1);
	//	show_crowdTablePattern();
}


void show_crowSchemaMatching() {
	WebTable wt1 = getExampleWebTable1("WTA");
	WebTable wt2 = getExampleWebTable2("WTB");
	Crowdsourcing cs;
	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	TPGenerator tpGen(&kb);
	SchemaMatcher schemaMatcher(cs,tpGen);

	int jid = schemaMatcher.askSchemaMatching(wt1, wt2, 3);
	vector<ColPair> colPairs = schemaMatcher.getSchemaMatching(jid);

	for_each(colPairs.begin(),colPairs.end(),[](const ColPair colPair){
		LOG(LOG_DEBUG, "[%s]-[%s]",colPair.first.c_str(),colPair.second.c_str());
	});
}
void show_crowdTablePattern(){


	Crowdsourcing cs;
	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	TPGenerator tpGen(&kb);
//
	SchemaMatcher schemaMatcher(cs,tpGen);

	WebTable wt = getExampleWebTable();
//
	int id = schemaMatcher.askTablePattern(wt, 1);
//
	TablePattern tp = schemaMatcher.getTablePattern(id);
	cout << endl;
	cout << "Returned Table Pattern: " << endl;
	cout << tp << endl;
}

void show_serialize() {
	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	WebTable wt = getExampleWebTable();

	cout << endl;
	cout << "Before serializing Table: " << endl;
	wt.print();
	Json::Value wtNode = wt.serialize();
	WebTable wt2 = WebTable::deserialize(wtNode);
	cout << "After Serializing Table: " <<endl;
	wt2.print();
	cout << endl;

	TPGenerator tpGen(&kb);
	priority_queue<TablePattern> tps = tpGen.generatePatterns(wt, 2);
	TablePattern tp = tps.top();

	cout << "Before Serialization: " <<endl;
	cout << tp <<endl;

	Json::Value root = tp.serialize();

	TablePattern tpb = TablePattern::deserialize(root);
	cout << "After Serilization: " << endl;
	cout << tpb << endl;
}
//void show_typeDistribution() {
//
//	KB kb;
//	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
//	WebTable wt = getExampleWebTable();
//	TPGenerator tpGen(&kb);
//	priority_queue<TablePattern> tps = tpGen.generatePatterns(wt, 2);
//
//	cout << "Table Pattern Size: " <<tps.size() << endl;
//	vector<TablePattern> tpv;
//	while(!tps.empty()) {
//		TablePattern tp = tps.top();
//		tpv.push_back(tp);
//		tps.pop();
//	}
////	map<TablePattern,double> tpProb = getTpProbability(tpv);
////	for(auto tpProbIt = tpProb.begin();tpProbIt != tpProb.end();tpProbIt++) {
////		cout << tpProbIt->first << endl;
////		cout << "Prob: " << tpProbIt->second << endl;
////	}
//	map<string,map<URI,double>> result = getColTypeDistribution(tpv);
//
//	//Display the mapping url and probability of each column
//	cout << "=================================================" << endl;
//	for(auto colMapIt = result.begin();colMapIt != result.end();colMapIt++) {
//		cout <<"Col Name: " << colMapIt->first << endl;
//		map<URI,double> urlMapping = colMapIt->second;
//		for (auto urlMapIt = urlMapping.begin();urlMapIt != urlMapping.end();urlMapIt++) {
//			cout << "\t" << "[" << urlMapIt->second <<"] " << urlMapIt->first << endl;
//		}
//		cout << "---------------------------------------------" << endl;
//	}
//
//}
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
WebTable getExampleWebTable(string name) {
	//An example web table in Kartara Implemenataion

	WebTable wt(name);
	vector<string> col1 = { "Rossi", "Klate", "Pirlo" };
	vector<string> col2 = { "Italy", "China", "Japan" };
	vector<string> col3 = { "Rome", "Beijing", "Toky" };

	wt.addCol(col1);
	wt.addCol(col2);
	wt.addCol(col3);
	return wt;
}

WebTable getExampleWebTable1(string name) {
	//An example web table in Kartara Implemenataion

	WebTable wt(name);
	vector<string> col1 = { "Rossi", "Klate" };
	vector<string> col2 = { "Italy", "China"};
	vector<string> col3 = { "Rome", "Beijing" };

	wt.addCol(col1);
	wt.addCol(col2);
	wt.addCol(col3);
	return wt;
}


WebTable getExampleWebTable2(string name) {
	//An example web table in Kartara Implemenataion

	WebTable wt(name);
//	vector<string> col1 = { "Klate", "Pirlo" };
	vector<string> col2 = { "China", "Japan" };
	vector<string> col3 = { "Beijing", "Toky" };
	vector<string> col4 = { "China", "Japan" };
//	wt.addCol(col1);
	wt.addCol(col2);
	wt.addCol(col3);
	wt.addCol(col4);
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

//
//
void show_matchSchema(){
	WebTable wt1 = getExampleWebTable1("WTA");
	WebTable wt2 = getExampleWebTable2("WTB");
	wt1.print();
	cout << "--------------------------------" << endl;
	wt2.print();
	cout << "--------------------------------" << endl;

	Crowdsourcing cs;
	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	TPGenerator tpGen(&kb);
//
	SchemaMatcher schemaMatcher(cs,tpGen);
	map<ColPair,TypeDistribution> matchedCol = schemaMatcher.matchSchema(wt1, wt2);

	for(auto matchedIt = matchedCol.begin();matchedIt != matchedCol.end();matchedIt++) {
		ColPair colPair = matchedIt->first;
		cout << colPair.first << "-" << colPair.second << endl;
		TypeDistribution jointDist = matchedIt->second;
		for(auto jointDistIt = jointDist.begin();jointDistIt != jointDist.end();jointDistIt++) {
			cout << "\t" << "[" << jointDistIt->second << "] " << jointDistIt->first << endl;
		}
	}
}
