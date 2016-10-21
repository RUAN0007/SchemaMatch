#include <Crowdsourcing.h>

int Crowdsourcing::postColTypeCorrespondece(const map<string,vector<URI>>& candidateCorrespondece,
											const WebTable& wt) const {
	//Write the question to file so that a python program can read and post the question
	return 1;

}

map<string, URI> Crowdsourcing::getColTypeCorrespondence(int jobID) const{
	map<string,URI> correspondence;
	//Read the file and return the crowdsourcing result

	//Fake correspondence for testing purpose
	correspondence["default_table_1"] = "https://www.comp.nus.edu.sg/todo_url#Country";
	correspondence["default_table_2"] = "https://www.comp.nus.edu.sg/todo_url#Capital";
	return correspondence;
}


int Crowdsourcing::postColMatching(const map<string, vector<string>>& candidateMatching,
									const WebTable& wt1,
									const WebTable& wt2) const{
	//Write question to file so that a python program can later read
	return 1;
}

map<string,string> Crowdsourcing::getColMatching(int jobID) const{
	map<string,string> matching;
	//read the file for the matching answer

	//for testing purpose
//	matching["WTA_1"] = "WTB_0";
//	matching["WTB_2"] = "N.A.";
	return matching;
}
bool Crowdsourcing::hasJobFinished(int jobID) const{
	//check whether the job has finished
	return true;
}
