#include <Crowdsourcing.h>
#include <fstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "debug.h"

//Add jobID in this API
bool dumpQuestionInf(Json::Value questionRoot, int questionID) {
/*
 * Dump the schema matching question info in json format
 */
		ofstream smQuestionFile;
		smQuestionFile.open(smQuestionDir + "SM" + to_string(questionID) + ".json",std::ios::out);
		smQuestionFile << questionRoot.toStyledString();
		smQuestionFile.close();
		return true;
}

//Add jobID into this API
int Crowdsourcing::postColMatching(const map<string, vector<string>>& candidateMatching, const WebTable& wt1, const WebTable& wt2) const{

	srand(time(NULL));
	int jobID = rand() % 1000; //select a random 4-digit integer as a job ID

	Json::Value wt1Node = wt1.serialize();
	Json::Value wt2Node = wt2.serialize();

	Json::Value questionInfoNode;
	for(const auto& match: candidateMatching) {
		int questionID = rand() % 9000 + 1000; //QuestionID is a random integer from [1000,9999]
		questionInfoNode.append("SM" + to_string(questionID));

		Json::Value questionRoot;
		Json::Value matchingNode;
		Json::Value matchColNode;

		string col1 = match.first;
		vector<string> matchCols = match.second;
		for(string matchCol:matchCols) {
			matchColNode.append(matchCol);
		}
		matchingNode[col1] = matchColNode;

		questionRoot["wt1"] = wt1Node;
		questionRoot["wt2"] = wt2Node;
		questionRoot["match"] = matchingNode;

		dumpQuestionInf(questionRoot, questionID);
	}
	dumpJobInfo(infoRoot, jobID);
	return jobID;
}
/*
 * Retrieve the answer of schema matching job based on jobID
 */
Json::Value getSMAnswerJson(int jobID) {

	Json::Value root;

	//Retrieve from CSTask based on jobID
	//Check their statuses and collect their answers


	return root;
}

map<string,string> Crowdsourcing::getColMatching(int jobID) const{
	map<string,string> matching;
	//read the file for the matching answer //for testing purpose //	matching["WTA_1"] = "WTB_0"; //	matching["WTB_2"] = "N.A.";

	Json::Value root = getSMAnswerJson(jobID);
	Json::Value colNode = root["cols"];
	Json::Value matchesNode = root["matches"];
	for(unsigned int i = 0;i < colNode.size();i++) {
		string col = colNode[i].asString();
		string matchCol = matchesNode[col].asString();
		matching[col] = matchCol;
	}
	return matching;
}
