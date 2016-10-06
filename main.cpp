#include <iostream>
#define DEBUG
#include "debug.h"
#include <fstream>
#include "KnowledgeBase.h"
#include "TablePattern.h"
#include <vector>
#include <queue>
// #include "include/IKnowledgeBase.h"

using namespace std;
WebTable getExampleWebTable();
int main(){

	// vector<CKEntry> q;
	// CKEntry ck3("1","3",1.4);

	// CKEntry ck1("1","2",1.2);
	// CKEntry ck2("1","3",1.3);
	// q.push_back(ck2);
	// q.push_back(ck1);
	// q.push_back(ck3);

	// for(auto e:q){
	// 	cout << e.getScore() << endl;
	// }
 //    // while( !q.empty() ){
 //    // 	auto e = q.top();
 //    //     cout << e.getScore() << endl;
 //    //     q.pop();
 //    // }
 //    cout << (ck1 > ck2) << endl;
	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");
	cout << kb.getNumberOfTypes() << endl;
	list<URI> uris = kb.listCandidateTypes("China");

	for(list<URI>::iterator it = uris.begin(); it != uris.end(); it++)
	{
		std::cout << *it << endl;
	}

	cout << uris.front() << endl;

	cout << "Instance Count: " << kb.countType(uris.front()) << endl;
	cout << "TypeS Count: " <<kb.getNumberOfTypes() << endl;

	// WebTable wt =   getExampleWebTable();
	// vector<string> value = wt.getColValues("Kartara_1");
	// wt.print();

	// TPGenerator tpGen(&kb);

	// queue<CKEntry> q = tpGen.getColTypes("Kartara_1",value);
	// cout << q.size() << endl;
 //    while( !q.empty() ){
 //    	CKEntry e = q.front();
 //    	q.front();
 //    	cout << e.getColName() << endl;
 //    	cout << e.getType() << endl;
 //        cout << e.getScore() << endl;
 //        q.pop();
 //    }
 
}

WebTable getExampleWebTable() {
	//An example web table in Kartara Implemenataion

	WebTable wt("Kartara");
	vector<string> col1 = {"Rossi","Klate","Pirlo"};
	vector<string> col2 = {"Italy", "China", "Japan"};
	vector<string> col3 = {"Rome","Beijing","Toky"};

	wt.addCol(col1);
	wt.addCol(col2);
	wt.addCol(col3);
	return wt;
}
