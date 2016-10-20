#include "CrowdSourcing.h"

int Crowdsourcing::postColTypeCorrespondece(const map<string,vector<URI>>& candidateCorrespondece,
											const WebTable& wt) const {
	//Write the question to file so that a python program can read and post the question
	return 1;

}

map<string, URI> Crowdsourcing::getColTypeCorrespondence(int jobID) const{
	map<string,URI> correspondence;
	//Read the file and return the crowdsourcing result
	return correspondence;
}


int Crowdsourcing::postColMatching(const map<string, vector<string>>& candidateMatching,
									const WebTable& wt1,
									const WebTable& wt2) const{
	//Write question to file so that a python program can later read
	return 1;
}

vector<ColPair> Crowdsourcing::getColMatching(int jobID) const{
	vector<ColPair> matching;
	//read the file for the matching answer
	return matching;
}

bool Crowdsourcing::hasJobFinished(int jobID) const{
	//check whether the job has finished
	return false;
}

int SchemaMatcher::askTablePattern(const WebTable& webTable,int maxQuestion) const{
	//generate a list of candidate table pattern for the web table.
	//get column type distribution
	//compute the entropy of each column type and rank them
	// write the table patterns, the column type distribution ordered by the entropy to disk

	//prepare for the question and return the job ID
	int jobID = 1;
	return jobID;
}

bool SchemaMatcher::isTablePatternReady(int jobID) const{
	return this->crowdPlatform.hasJobFinished(jobID);
}

TablePattern SchemaMatcher::getTablePattern(int jobID) const {
	//retrieve the answer from crowdsourcing platform
	//read the candidate table patterns and column type distribution from the file

	//resolve the column type correspondence from the column with the highest entropy first
	//until there are at least one table pattern.

	//return the table pattern with the highest score/probability if more than one
	return TablePattern();
}

int SchemaMatcher::askSchemaMatching(const WebTable& wt1, const WebTable& wt2, int maxQuestion) const {
	//Get the jointTypeDistribution for wt1 and wt2
	//reorganize the jointTypeDistribution to map<string,vector<pair<string,double>>>
	//write the above to file based on the below order

	//prepare the k matching question for the columns.
	//A column with more matching shall have priorities for question
	//For the same number of matching, break the tie using the sum of score column high score has the higher probabilityjjjjjjjj

	int jobID = 2;
	return jobID;
}

bool SchemaMatcher::isSchemaMatchingReady(int jobID) const {
	return this->crowdPlatform.hasJobFinished(jobID);
}

vector<ColPair> SchemaMatcher::getSchemaMatching(int jobID) const {
	//get the crowdsourcing answer from job ID
	//Retrieve the candidate column matching
	//resolve the matching from the column based on the order

	//If there are more candidate matching, choose the one with high score
	return vector<ColPair>();

}
