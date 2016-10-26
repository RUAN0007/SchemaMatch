#include <Crowdsourcing.h>
#include <fstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "debug.h"
string tpQuestionDir = "tp_questions/";
string tpAnswerDir = "tp_answers/";
string tpInfoPath = tpQuestionDir + "tp_info.json";
string smQuestionDir = "sm_questions/";
string smInfoPath = smQuestionDir + "sm_info.json";
string smAnswerDir = "sm_answers/";

int Crowdsourcing::postColTypeCorrespondece(const map<string,vector<URI>>& candidateCorrespondece,
											const WebTable& wt) const {

	//Deal with case that has not candidate correspondences

	Json::Value infoRoot;
	Json::Reader reader;

	ifstream tpInfoFile(tpInfoPath, ifstream::binary);
	if(tpInfoFile.good()){

		if(tpInfoFile.is_open() && !reader.parse(tpInfoFile, infoRoot)){
			LOG(LOG_FATAL, "Fail to parse %s as a json file. ", tpInfoPath.c_str());
		}
	}
	srand(time(NULL));
	int jobID = rand() % 1000; //JobID is a random integer from [0,999]

	Json::Value questionInfoNode;
	//Write the question to file so that a python program can read and post the question
	Json::Value wtNode = wt.serialize();

	for(const auto& corr: candidateCorrespondece) {

		int questionID = rand() % 9000 + 1000; //QuestionID is a random integer from [1000,9999]
		questionInfoNode.append(questionID);
		string col = corr.first;
		vector<URI> types = corr.second;

		Json::Value questionRoot;
		Json::Value corrNode;
		Json::Value typesNode;
		for(const URI& type:types) {
			typesNode.append(type);
		}//End of for URI
		corrNode[col] = typesNode;
		questionRoot["correspondence"] = corrNode;
		questionRoot["webtable"] = wtNode;

		ofstream tpQuestionFile;
		tpQuestionFile.open(tpQuestionDir + "TP" + to_string(questionID) + ".json",std::ios::out);
		tpQuestionFile << questionRoot.toStyledString();
		tpQuestionFile.close();
	}

	//cout <<questionInfoNode.toStyledString() << endl;
	infoRoot[to_string(jobID)] = questionInfoNode;
//	cout << infoRoot.toStyledString() << endl;
	ofstream infoFile;
	infoFile.open(tpInfoPath,std::ios::out);
	infoFile << infoRoot.toStyledString();
	infoFile.close();
	return jobID;

}

map<string, URI> Crowdsourcing::getColTypeCorrespondence(int jobID) const{
	map<string,URI> correspondence;
	//Read the file and return the crowdsourcing result

	//Fake correspondence for testing purpose
//	correspondence["default_table_1"] = "https://www.comp.nus.edu.sg/todo_url#Country";
//	correspondence["default_table_2"] = "https://www.comp.nus.edu.sg/todo_url#Capital";

	string fpath = tpAnswerDir + to_string(jobID) + ".json";
	ifstream answerFile(fpath, ifstream::binary);
	if(!answerFile.is_open()) {
		LOG(LOG_FATAL,"%s does not exist.",fpath.c_str() );
		return correspondence;
	}

	Json::Value root;
	Json::Reader reader;

	if(!reader.parse(answerFile, root)){
		LOG(LOG_FATAL, "Fail to parse %s as a json file. ", fpath.c_str());
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


int Crowdsourcing::postColMatching(const map<string, vector<string>>& candidateMatching, const WebTable& wt1, const WebTable& wt2) const{
	//Write question to file so that a python program can later read
	Json::Value infoRoot;

	Json::Reader reader;

	ifstream smInfoFile(smInfoPath, ifstream::binary);
	if(smInfoFile.good()){

		if(!reader.parse(smInfoFile, infoRoot)){
			LOG(LOG_FATAL, "Fail to parse %s as a json file. ", tpInfoPath.c_str());
		}
	}
	srand(time(NULL));
	int jobID = rand() % 1000; //select a random 4-digit integer as a job ID

	Json::Value wt1Node = wt1.serialize();
	Json::Value wt2Node = wt2.serialize();

	Json::Value questionInfoNode;
	for(const auto& match: candidateMatching) {
		int questionID = rand() % 9000 + 1000; //QuestionID is a random integer from [1000,9999]
		questionInfoNode.append(questionID);

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

		ofstream smQuestionFile;
		smQuestionFile.open(smQuestionDir + "SM" + to_string(questionID) + ".json",std::ios::out);
		smQuestionFile << questionRoot.toStyledString();
		smQuestionFile.close();
	}

	infoRoot[to_string(jobID)] = questionInfoNode;

	ofstream smInfoOutputFile;
	smInfoOutputFile.open(smInfoPath,std::ios::out);
	smInfoOutputFile << infoRoot.toStyledString();
	smInfoOutputFile.close();
	return jobID;
}

map<string,string> Crowdsourcing::getColMatching(int jobID) const{
	map<string,string> matching;
	//read the file for the matching answer

	//for testing purpose
//	matching["WTA_1"] = "WTB_0";
//	matching["WTB_2"] = "N.A.";
	string fpath = smAnswerDir + to_string(jobID) + ".json";
	ifstream answerFile(fpath, ifstream::binary);
	if(!answerFile.is_open()) {
		LOG(LOG_FATAL,"%s does not exist.",fpath.c_str() );
		return matching;
	}

	Json::Value root;
	Json::Reader reader;

	if(!reader.parse(answerFile, root)){
		LOG(LOG_FATAL, "Fail to parse %s as a json file. ", fpath.c_str());
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
