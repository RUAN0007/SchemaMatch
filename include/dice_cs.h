/*
 * dice_cs.h
 *
 *  Created on: Nov 30, 2016
 *      Author: ruanpingcheng
 */

#ifndef INCLUDE_DICE_CS_H_
#define INCLUDE_DICE_CS_H_

#include <iostream>
#include <string>
#include "csbc.h"

using namespace std;

class dice_cs {
public:
	dice_cs(){};
	dice_cs(string dbPath){
		this->_diceDB = new csbc(dbPath);
	};

	~dice_cs() {
		delete this->_diceDB;
	}

	/*
	 * Post the DICE crowdsourcing question by writing the question info to relation CSTask in DICE DB
	 * args:
	 * 	jobID: ID of the query
	 * 	csID: ID to identify which type of question in crowdsourcing platform, corresponding jobID in Timon
	 * 	question: Necessary question information. If for Timon, it must be in json format.
	 *
	 * return:
	 * 	questionID to identify this question and later use it to retrieve answer
	 */
	int postQuestion(int jobID, int csID, string question);

	/*
	 * Retrieve answer based on the qid
	 * args:
	 * 	qid: questionID to identify the question
	 * 	answerPtr:the pointer to locate a string which answer will be written here
	 * return:
	 * 	a bool value to indicate whether this answer has been answered or not.
	 */
	bool getQuestionAnswer(int qid, string* answerPtr);
	/*
	 * Retrieve all finished questions' answer for the job by jobiD
	 * args:
	 * 	jid: jobID to identify the job
	 * 	answerListPtr:the pointer to locate a list of string which all answer will be written here
	 * return:
	 * 	a bool value to indicate whether the operation is successful or not.
	 */
	bool getJobAnswers(int jid, list<string>* answerListPtr);

	/*
	 * Prepare SQL to insert crowdsourcing question to CSTask in DICE DB
	 */
	static string createQuestionSQL(int jobID, int qid, int csID, string question);

	/*
	 * Prepare SQL to retrieve answer from CSTask in DICE DB based on question ID
	 */
	static string getAnswerSQL(int qid);

private:

	csbc* _diceDB;


};

#endif /* INCLUDE_DICE_CS_H_ */
