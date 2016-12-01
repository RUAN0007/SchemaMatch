#include <Crowdsourcing.h>
#include <fstream>
#include "debug.h"

int Crowdsourcing::CSID = 2;

//Add jobID into this API
bool Crowdsourcing::postColMatching(int jobID,
									const map<string, vector<string>>& candidateMatching,
									const WebTable& wt1, const WebTable& wt2){


	Json::Value wt1Node = wt1.serialize();
	Json::Value wt2Node = wt2.serialize();
	Json::Value questionInfoNode;
	for(const auto& match: candidateMatching) {

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

		string questionInfo = questionRoot.toStyledString();
		LOG(LOG_DEBUG, "Start Posting Question to Crowdsourcing system. ");
		int qid = this->genericCS->postQuestion(jobID, Crowdsourcing::CSID, questionInfo);

		LOG(LOG_DEBUG, "Posting Question %d to Crowdsourcing system. ", qid);
	}
	return true;
}

map<string,string> Crowdsourcing::getColMatching(int jobID) {
	map<string,string> matching;

	list<string> jobAnswers;
	bool success = this->genericCS->getJobAnswers(jobID, &jobAnswers);
	if (!success) {
		LOG(LOG_WARNING,"Retrieve answer for job %d failed...", jobID);
	}
	Json::Reader reader;
	for(const string& answer: jobAnswers) {
		Json::Value answerNode;
		if (reader.parse(answer, answerNode)) {
			//Assume there is only one question.
			string target_col = answerNode[0]["match"]["match_col"].asString();
			string source_col = answerNode[0]["match"]["source_col"].asCString();

			matching[source_col] = target_col;
			LOG(LOG_DEBUG, "%s -> %s ", source_col.c_str(), target_col.c_str());
		}else{
			LOG(LOG_WARNING,"Fail to parse answer for json %s", answer.c_str());
		}
	}//End of for loop

	return matching;
}
