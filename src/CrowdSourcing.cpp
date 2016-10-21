#include "CrowdSourcing.h"
#include <fstream>
#include <list>
#include <math.h>
#include <algorithm>
#include <json.h>
#include "debug.h"
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

vector<ColPair> Crowdsourcing::getColMatching(int jobID) const{
	vector<ColPair> matching;
	//read the file for the matching answer
	return matching;
}
bool Crowdsourcing::hasJobFinished(int jobID) const{
	//check whether the job has finished
	return true;
}

int SchemaMatcher::askTablePattern( WebTable& webTable,unsigned int maxQuestion) const{
	//generate a list of candidate table pattern for the web table.

	priority_queue<TablePattern> tps = this->tpGen.generatePatterns(webTable, 2);

//	cout << "Table Pattern Size: " <<tps.size() << endl;
	//Convert the p-q of table patterns to vector of table patterns
	vector<TablePattern> tpv;
	while(!tps.empty()) {
		TablePattern tp = tps.top();
		tpv.push_back(tp);
		tps.pop();
	}
	//compute the entropy of each column and sort the column based on the entropy in decreasing order
	map<string,map<URI,double>> colTypeDistr = this->getColTypeDistribution(tpv);
//	cout << "Col Type Dist Size: " << colTypeDistr.size() << endl;
	vector<pair<string,double>> colEntropy;
	for(const auto& kv:colTypeDistr) {
		string col = kv.first;
		map<URI,double> typeDistr = kv.second;
		double entropy;
		for(const auto& typeProb:typeDistr){
			entropy += -(typeProb.second * log2(typeProb.second));
		}
		colEntropy.push_back(make_pair(col, entropy));
	}
	sort(colEntropy.begin(),colEntropy.end(),
			[](pair<string,double> p1, pair<string,double> p2)->bool{
				return p1.second > p2.second;
	});
//	for(pair<string,double> colEnt:colEntropy) {
//		cout << colEnt.first << ": " << colEnt.second << endl;
//	}
	//prepare min(maxQuestion, colEntropy.size()) questions
	Json::Value queryColNode;
	map<string,vector<URI>> candidateCorrespondences;
	for(unsigned int i = 0;i < maxQuestion;i++) {
		if(i == colEntropy.size()) break;
		string col = colEntropy[i].first;
		double entropy = colEntropy[i].second;
		if(entropy < 1e-6) break;
		queryColNode.append(col);
		vector<URI> candidateTypes;
		for(const auto& typeProb:colTypeDistr[col]){
			candidateTypes.push_back(typeProb.first);
		}
		candidateCorrespondences[col] = candidateTypes;

//		cout << col << endl;
//		for(URI t:candidateTypes) cout << "\t" << t << endl;
//		cout << endl;
	}
	//prepare for the question and return the job ID
	int jobID = this->crowdPlatform.postColTypeCorrespondece(candidateCorrespondences, webTable);

	// write the table patterns, the query columns order by entropy to disk
	Json::Value candidateTPNode;
	for(const TablePattern& tp:tpv){
		candidateTPNode.append(tp.serialize());
	}
	Json::Value root;
	root["tps"] = candidateTPNode;
	root["query_cols"] = queryColNode;

	ofstream tpQuestionFile;
	tpQuestionFile.open("tp_questions/TP_" + to_string(jobID) + ".json",std::ios::out);
	tpQuestionFile << root.toStyledString();
	tpQuestionFile.close();

	return jobID;
}

TablePattern SchemaMatcher::getTablePattern(int jobID) const{

	if(!isTablePatternReady(jobID)) return TablePattern();
	ifstream tpQuestionFile("tp_questions/TP_" + to_string(jobID) + ".json",ifstream::binary);
	if(!tpQuestionFile.is_open()) return TablePattern();

	Json::Value root;
	Json::Reader reader;

	if(!reader.parse(tpQuestionFile, root)){
		cout << "Fail to parse table pattern job " + to_string(jobID) << endl;
		return TablePattern();
	}

	list<TablePattern> tps;
	Json::Value tpsNode = root["tps"];
	for(unsigned int i = 0;i < tpsNode.size();i++) {
		tps.push_back(TablePattern::deserialize(tpsNode[i]));
	}
//	cout << "Table Pattern Count: " << tps.size() << endl;
	vector<string> queryCols;
	Json::Value queryColsNode = root["query_cols"];
	for(unsigned int i = 0;i < queryColsNode.size();i++) {
		queryCols.push_back(queryColsNode[i].asString());
	}
//	cout << "Query Col: " << endl;
//	for(string col:queryCols) cout << col << " ";
//	cout << endl;

	map<string,URI> correspondence = this->crowdPlatform.getColTypeCorrespondence(jobID);

	list<TablePattern> preTps;
	for(string queryCol: queryCols) {
		auto colIt = correspondence.find(queryCol);
		if(colIt == correspondence.end()) {cout << "Fatal: Cannot find type correspondence for column " + queryCol; }
		URI type = colIt->second;
		//Before filter, take the snapshot of original candidate table patterns
		preTps = tps;

		//filter out all the table patterns that violate this column's correspondence
		for(auto tpIt = tps.begin();tpIt != tps.end();) {
			vector<CKEntry> ckEntries = tpIt->getCKEntries();
			auto targetEntryIt = find_if(ckEntries.begin(), ckEntries.end(),[queryCol, type](const CKEntry& ck)->bool{
				return ck.getColName() == queryCol && ck.getType() == type;
			});

			if( targetEntryIt != ckEntries.end()) {
				tpIt++;
			}else{
			//This table pattern violates this crowdsourcing correspondence. It shall be removed.
				tpIt = tps.erase(tpIt);
			}
		}//for tpit
//		cout <<endl;
//		cout << "Col: " << queryCol << " " << "Type: " << type <<endl;
//		cout << "Remaining Table Patterns: " << endl << endl;
//		for(const TablePattern& tp: tps){
//			cout << tp << endl;
//			cout << endl;
//		}
		if(tps.empty()) break;
	}//for queryCol

	list<TablePattern> candidateTps = tps.empty()?preTps:tps;

	//return the table pattern with the max probability
	list<TablePattern>::iterator maxTpIt =
			max_element(tps.begin(),tps.end(),
							[](const TablePattern& lhs, const TablePattern& rhs)->bool {
								return lhs.getProbability() < rhs.getProbability();
							}
						);

	return *maxTpIt;
}



bool SchemaMatcher::isTablePatternReady(int jobID) const{
	return this->crowdPlatform.hasJobFinished(jobID);
}

int SchemaMatcher::askSchemaMatching(const WebTable& wt1, const WebTable& wt2, unsigned int maxQuestion) const {
	//Get the jointTypeDistribution for wt1 and wt2
//	map<ColPair,TypeDistribution> colPairTypeDistr = this->tpGe


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

map<ColPair,TypeDistribution> SchemaMatcher::matchSchema(const WebTable& wt1,
										  const WebTable& wt2) const{

	priority_queue<TablePattern> tpq1 = this->tpGen.generatePatterns(wt1, 2);
	priority_queue<TablePattern> tpq2 = this->tpGen.generatePatterns(wt2, 2);

	vector<TablePattern> tpv1 = pq2v(&tpq1);
	vector<TablePattern> tpv2 = pq2v(&tpq2);

	map<string, TypeDistribution> colTypeDist1 = this->getColTypeDistribution(tpv1);
	map<string, TypeDistribution> colTypeDist2 = this->getColTypeDistribution(tpv2);

	map<ColPair,TypeDistribution> colPairDist = this->computeJointDist(colTypeDist1,colTypeDist2);
	return colPairDist;

}

map<ColPair, TypeDistribution> SchemaMatcher::computeJointDist(const map<string, TypeDistribution>& ctd1,
		 	 	 	 	 	 	 	 	 	 	const map<string, TypeDistribution>& ctd2) const{
	map<ColPair, TypeDistribution> jointDist;

	map<string, TypeDistribution>::const_iterator ctd1It = ctd1.begin();

	for(;ctd1It != ctd1.end();ctd1It++) {
		string col1 = ctd1It->first;
		TypeDistribution td1 = ctd1It->second;
		map<string, TypeDistribution>::const_iterator ctd2It = ctd2.begin();
		for(;ctd2It != ctd2.end();ctd2It++) {
			string col2 = ctd2It->first;
			TypeDistribution td2 = ctd2It->second;
			TypeDistribution typeJointDist = getTypeJointDist(td1,td2);

			if (typeJointDist.size() > 0) {
				ColPair colPair(col1,col2);
				jointDist[colPair] = typeJointDist;
			}//End of if
		}//End of for ctd2It
	}//End of for ctd1it
	return jointDist;
}

vector<TablePattern> SchemaMatcher::pq2v(priority_queue<TablePattern>* tpq)const{
	vector<TablePattern> tpv;
	while(!tpq->empty()){
		TablePattern tp = tpq->top();
		tpq->pop();
		tpv.push_back(tp);
	}
	return tpv;
}


TypeDistribution SchemaMatcher::getTypeJointDist(const TypeDistribution& td1, const TypeDistribution& td2)const{
	TypeDistribution jointDist;
	TypeDistribution::const_iterator td1It = td1.begin();

	for(;td1It != td1.end();td1It++) {
		URI type = td1It->first;
		double prob1 = td1It->second;
		TypeDistribution::const_iterator typeIt = td2.find(type);
		if (typeIt == td2.end()){
			//This type is not found in the type distribution of the other column
			continue;
		}
		double prob2 = typeIt->second;
		jointDist[type] = prob1 * prob2;
	}
	return jointDist;
}

/*The structure of return results:
 * key: colID
 * value: possible mapping type URI and probability
 * 		key:type URI
 * 		value: probability (double)
 */
map<string,TypeDistribution> SchemaMatcher::getColTypeDistribution(const vector<TablePattern>& tps)const{
	map<TablePattern,double> patternProb = getTpProbability(tps);
	map<string,TypeDistribution> result;

	auto patternProbIt = patternProb.begin();
	for(;patternProbIt != patternProb.end();patternProbIt++) {
		TablePattern tp = patternProbIt->first;
		double tpProb = patternProbIt->second;
		//cout << "TP Prob: " << tpProb <<endl;

		for(CKEntry ck: tp.getCKEntries()) {
			string col = ck.getColName();
			URI type = ck.getType();
			map<URI,double> urlMatching;
			//cout << "I am fine" <<endl;
			if (result.find(col) == result.end()) result[col] = urlMatching;
			urlMatching = result[col];

			if(urlMatching.find(type) == urlMatching.end()) urlMatching[type] = 0.0;
			double prevProb = urlMatching[type];

			double newProb = prevProb + tpProb;
			urlMatching[type] = newProb;
			result[col] = urlMatching;
		}
	}
	return result;
}


//Probability of a table pattern = its score / (sum of score of all table patterns)
map<TablePattern,double> SchemaMatcher::getTpProbability(const vector<TablePattern>& tps)const {
	double norm = 0.0;
	for (TablePattern tp:tps) {
		double score = tp.getScore();
		norm +=  exp(score);
	}
	map<TablePattern,double> result;

	for (TablePattern tp:tps) {
		double score = tp.getScore();
		double probability = exp(score) / norm;
		result[tp] = probability;
	}
	//Debug


	return result;
}
