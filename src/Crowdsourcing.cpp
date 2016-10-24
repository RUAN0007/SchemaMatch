#include <Crowdsourcing.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "debug.h"

int Crowdsourcing::postColTypeCorrespondece(const map<string,vector<URI>>& candidateCorrespondece,
											const WebTable& wt) const {

	//Deal with case that has not candidate correspondences

	//Write the question to file so that a python program can read and post the question
	Json::Value wtNode = wt.serialize();

	Json::Value colNode;
	Json::Value corrNode;
	for(const auto& corr: candidateCorrespondece) {
		string col = corr.first;
		vector<URI> types = corr.second;

		colNode.append(col);
		Json::Value typesNode;
		for(const URI& type:types) {
			typesNode.append(type);
		}//End of for URI
		corrNode[col] = typesNode;
	}

	Json::Value root;
	root["webtable"] = wtNode;
	root["cols"] = colNode;
	root["correspondence"] = corrNode;


	srand(time(NULL));
	int jobID = rand() % 1000; //select a random 4-digit integer as a job ID
	ofstream tpQuestionFile;
	tpQuestionFile.open("cs_questions/" + to_string(jobID) + ".json",std::ios::out);
	tpQuestionFile << root.toStyledString();
	tpQuestionFile.close();
	return jobID;

}

map<string, URI> Crowdsourcing::getColTypeCorrespondence(int jobID) const{
	map<string,URI> correspondence;
	//Read the file and return the crowdsourcing result

	//Fake correspondence for testing purpose
//	correspondence["default_table_1"] = "https://www.comp.nus.edu.sg/todo_url#Country";
//	correspondence["default_table_2"] = "https://www.comp.nus.edu.sg/todo_url#Capital";

	string fpath = "cs_answers/" + to_string(jobID) + ".json";
	ifstream answerFile(fpath, ifstream::binary);
	if(!answerFile.is_open()) {
		LOG(LOG_FATAL,"%s does not exist.",fpath );
		return correspondence;
	}

	Json::Value root;
	Json::Reader reader;

	if(!reader.parse(answerFile, root)){
		LOG(LOG_FATAL, "Fail to parse %s as a json file. ", fpath);
		return correspondence;
	}

	Json::Value colNode = root["cols"];
	Json::Value corrNode = root["correspondence"];
	for(unsigned int i = 0;i < colNode.size();i++) {
		string col = colNode[i].asString();

		URI type = corrNode[col].asString();
		correspondence[col] = type;
	}
	return correspondence;
}


int Crowdsourcing::postColMatching(const map<string, vector<string>>& candidateMatching,
									const WebTable& wt1,
									const WebTable& wt2) const{
	//Write question to file so that a python program can later read
	Json::Value wt1Node = wt1.serialize();
	Json::Value wt2Node = wt2.serialize();
	Json::Value colNode;
	Json::Value matchingsNode;
	for(const auto& match: candidateMatching) {
		string col1 = match.first;
		colNode.append(col1);
		vector<string> matchCols = match.second;
		Json::Value matchColNode;
		for(string matchCol:matchCols) {
			matchColNode.append(matchCol);
		}
		matchingsNode[col1] = matchColNode;
	}

	Json::Value root;
	root["wt1"] = wt1Node;
	root["wt2"] = wt2Node;
	root["cols"] = colNode;
	root["matchings"] = matchingsNode;
	srand(time(NULL));
	int jobID = rand() % 1000; //select a random 4-digit integer as a job ID
	ofstream smQuestionFile;
	smQuestionFile.open("cs_questions/" + to_string(jobID) + ".json",std::ios::out);
	smQuestionFile << root.toStyledString();
	smQuestionFile.close();
	return jobID;
}

map<string,string> Crowdsourcing::getColMatching(int jobID) const{
	map<string,string> matching;
	//read the file for the matching answer

	//for testing purpose
//	matching["WTA_1"] = "WTB_0";
//	matching["WTB_2"] = "N.A.";
	string fpath = "cs_answers/" + to_string(jobID) + ".json";
	ifstream answerFile(fpath, ifstream::binary);
	if(!answerFile.is_open()) {
		LOG(LOG_FATAL,"%s does not exist.",fpath );
		return matching;
	}

	Json::Value root;
	Json::Reader reader;

	if(!reader.parse(answerFile, root)){
		LOG(LOG_FATAL, "Fail to parse %s as a json file. ", fpath);
		return matching;
	}

	Json::Value colNode = root["cols"];
	Json::Value matchesNode = root["matches"];
	for(unsigned int i = 0;i < colNode.size();i++) {
		string col = colNode[i].asString();
		string matchCol = matchesNode[col].asString();
		matching[col] = matchCol;
	}
	return matching;
}
bool Crowdsourcing::hasJobFinished(int jobID) const{
	//check whether the job has finished by checking whether the jobID json file exists in cs_answers/

	string fpath = "cs_answers/" + to_string(jobID) + ".json";
	ifstream answerFile(fpath, ifstream::binary);
	return answerFile.good();
}
