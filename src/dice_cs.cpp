/*
 * dice_cs.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: ruanpingcheng
 */

#include "dice_cs.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>     /* srand, rand */
#include <map>
#include <list>
string dice_cs::createQuestionSQL(int jobID, int qid, int csID, string question) {
	ostringstream sqlStream;
	sqlStream <<  "INSERT INTO CSTask (qid, jobid, crowdsourcingid, sub, status) "
			"VALUES ";
	sqlStream << "(" << qid << ",";
	sqlStream << jobID << ",";
	sqlStream << csID << ",";
	sqlStream << "'" << question << "'" << ",";
	sqlStream << "'INIT'" << ");";

	return sqlStream.str();
}

string dice_cs::getAnswerSQL(int qid) {
	ostringstream sqlStream;
	sqlStream << "SELECT result from CSTask where qid = ";
	sqlStream << qid << " AND ";
	sqlStream << "status = 'FINISH';";



	return sqlStream.str();
}
int dice_cs::postQuestion(int jobID, int csID,string question){
	srand(time(NULL));
	int qid = 1000 + rand() % 1000; //JobID is a random integer from [1000,1999]

	string createSQL = dice_cs::createQuestionSQL(jobID, qid, csID, question);

//	cout << "SQL:" << endl;
//	cout << createSQL << endl;
	this->_diceDB->dice_dbops(createSQL);
	cout << "Create Question " << qid << " for Job ID: " << jobID <<endl;

	return qid;
}

bool dice_cs::getQuestionAnswer(int qid, string* answerPtr){
	string sql = getAnswerSQL(qid);
//	cout << "SQL: " << endl;
//	cout << sql << endl;
	this->_diceDB->dice_select(sql);
	list<string> answerLst = csbc::result["result"];

	if(answerLst.size() == 0) {
		return false;
	}else {
		*answerPtr = answerLst.front();
		return true;
	}
}


bool dice_cs::getJobAnswers(int jid, list<string>* answerListPtr) {

	ostringstream sqlStream;
	sqlStream << "SELECT result from CSTask where jobid = ";
	sqlStream << jid << " AND ";
	sqlStream << "status = 'FINISH';";

	this->_diceDB->dice_select(sqlStream.str());

	list<string> answers = csbc::result["result"];

	for(const string& answer: answers) {
		answerListPtr->push_back(answer);
	}

	return true;
}

