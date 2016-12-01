#include "SchemaMatching.h"
#include <fstream>
#include <list>
#include <math.h>
#include <algorithm>
#include <json.h>
#include "debug.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>     /* srand, rand */

bool SchemaMatcher::creatDiceJob(const Json::Value& root, int jobID) {
	//insert into Job relatio with
	//jobid = jobID
	//crowdsourcingid = timon's job id
	//content = root.to_string()

	ostringstream sqlStream;
	sqlStream <<  "INSERT INTO Jobs (jobid, crowdsourcingid, sql, state, content) "
			"VALUES ";
	sqlStream << "(" << jobID << ",";
	sqlStream << Crowdsourcing::CSID << ",";
	sqlStream << "" << ","; //TODO Later add sql statement here
	sqlStream << "'" << "" << "'" << ","; //TODO Later add state informatio here
	sqlStream << "'" + root.toStyledString() + "'" << ");";

	LOG(LOG_INFO, "Create Dice Job SQL:  %s \n",sqlStream.str().c_str());

	this->db.dice_dbops(sqlStream.str());
	return true;
}

int SchemaMatcher::askSchemaMatching(const WebTable& wt1, const WebTable& wt2, unsigned int maxQuestion) {
	//Get the jointTypeDistribution for wt1 and wt2
	map<ColPair,TypeDistribution> colPairTypeDistr = this->matchSchema(wt1, wt2);

	//key: colID of either wt1 or wt2
	//Value:
	//	key: the matching column of the key
	//	value: the probability of this matching (currently is the sum of probability of matching type)
	map<string,map<string,double>> matchings;

	for(const auto& kv: colPairTypeDistr) {
		ColPair colPair = kv.first;
		string col1 = colPair.first;
		string col2 = colPair.second;

		map<URI,double> typeDistr = kv.second;
		double prob = 0.0;
		for_each(typeDistr.begin(),typeDistr.end(),[&prob](const pair<URI,double>& typeProb){
			prob += typeProb.second;
		});

		matchings[col1][col2] = prob;
//		matchings[col2][col1] = prob;
		LOG(LOG_DEBUG,"[%s]-[%s]:%.3f", col1.c_str(),col2.c_str(),prob);

	}
	Json::Value colsNode;
	Json::Value matchingNode;
	for(const auto& match:matchings) {
		string col = match.first;
		colsNode.append(col);
		map<string,double> matchingCols = match.second;
		Json::Value matchingProbNode;
		Json::Value matchingColNode;
		for(const auto& kv:matchingCols) {
			matchingColNode.append(kv.first);
			matchingProbNode[kv.first] = kv.second;
		}
		matchingProbNode["match_cols"] = matchingColNode;
		matchingNode[col] = matchingProbNode;
	}

	vector<pair<string, map<string,double>>> matchingToSort(matchings.begin(),matchings.end());
//	copy(matchings.begin(),matchings.end(),matchingToSort);

	sort(matchingToSort.begin(), matchingToSort.end(),
			[](const pair<string,map<string,double>>& lhsM, const pair<string,map<string,double>>& rhsM)->bool {

				map<string,double> lhs = lhsM.second;
				map<string,double> rhs = rhsM.second;
				//col with more matching has higher priority
				//if ties, col with greater sum of matching probability wins
				if(lhs.size() == rhs.size()){
					double lhsProb = 0.0;
					for_each(lhs.begin(), lhs.end(),[&lhsProb](const pair<string, double>& matchingProb){
						lhsProb += matchingProb.second;
					});
					double rhsProb = 0.0;
					for_each(rhs.begin(), rhs.end(),[&rhsProb](const pair<string, double>& matchingProb){
						rhsProb += matchingProb.second;
					});

					return lhsProb > rhsProb;
				}else{
					return lhs.size() > rhs.size();
				}
	});

	LOG(LOG_DEBUG,"----------------------Matching To Sort---------------------------");
	for(const auto& matching:matchingToSort) {
		LOG(LOG_DEBUG,"Col: %s", matching.first.c_str());
		for_each(matching.second.begin(),matching.second.end(),[](const pair<string,double>& kv) {
			LOG(LOG_DEBUG, "\t %s [%.3f]",kv.first.c_str(), kv.second);
		});
	}
	LOG(LOG_DEBUG,"---------------------------------------------------------");

	Json::Value queryColNode;
	map<string, vector<string>> matchingOptions;

	unsigned int i = 0;
	for(const pair<string, map<string,double>> matching:matchingToSort) {
		if(i == maxQuestion) break;
		i++;
		string col = matching.first;
		queryColNode.append(col);
		map<string,double> matchingCols = matching.second;

		for(const auto& matchingCol:matchingCols) {
			matchingOptions[col].push_back(matchingCol.first);
		}
		matchingOptions[col].push_back("N.A.");
	}

	LOG(LOG_DEBUG,"----------------------Matching Options-----------------------");

	for(const auto& matching:matchingOptions) {
		LOG(LOG_DEBUG,"Col: %s", matching.first.c_str());
		for_each(matching.second.begin(),matching.second.end(),[](const string& matchingCol) {
			LOG(LOG_DEBUG, "\t %s ",matchingCol.c_str());
		});
	}
	LOG(LOG_DEBUG,"-------------------------------------------------------------");

	srand(NULL);
	int jobID = rand() * 1000; //JobID is a random integer ranging from 0 to 1000
	bool success = this->crowdPlatform.postColMatching(jobID, matchingOptions, wt1, wt2);

	if(!success) {
		LOG(LOG_WARNING, "Posting Questions For Job %d Failed. ", jobID);
	}

	Json::Value root;
	root["query_cols"] = queryColNode;
	root["cols"] = colsNode;
	root["candidate_match"] = matchingNode;

	this->creatDiceJob(root, jobID);
	return jobID;
}



map<string,map<string,double>> SchemaMatcher::filterMatching(
										const map<string,map<string,double>>& candidateMatches,
										ColPair csMatch) {

		//filter out the candidate matches that conflict with this csMatch
		map<string,map<string,double>> remainingMatching;
		for(const auto& candidateMatch:candidateMatches) {
			string col = candidateMatch.first;
			//this col's matching has been determined by the crowdsourcing platform
			if(col == csMatch.first || col == csMatch.second) continue;
			map<string,double> colMatch = candidateMatch.second;
			for(const auto& matchProb: colMatch) {
				string matchCol = matchProb.first;
				//this col cannot be matched with matchCol, as matchCol has been involved in csMatch
				if(matchCol == csMatch.first || matchCol == csMatch.second) continue;
				double prob = matchProb.second;
				remainingMatching[col][matchCol] = prob;
			}//End of for matchProb
		}//End of for candidataMatch
		return remainingMatching;
}

bool SchemaMatcher::getMatchingInfo(int jobID, Json::Value* info) {

	ostringstream sqlStream;
	sqlStream <<  "SELECT content FROM Jobs WHERE jobid = ";
	sqlStream <<   jobID << ";";

	LOG(LOG_INFO, "Get Matching Info SQL: %s",sqlStream.str().c_str());

	this->db.dice_select(sqlStream.str());
	list<string> values = csbc::result["content"];

	if (values.size() == 0){
		return false;
	}else{
	    Json::Reader reader;
	    bool parsingSuccessful = reader.parse( values.front().c_str(), *info );     //parse process
	    if ( !parsingSuccessful )
	    {
	        LOG(LOG_INFO, "Parse Job %d Info Failed. Message: ", jobID, reader.getFormattedErrorMessages().c_str());
	        return false;
	    }
		return true;
	}
}

vector<ColPair> SchemaMatcher::getSchemaMatching(int jobID){
	//Retrieve the candidate column matching from the stored json file
	Json::Value root;
	bool success = this->getMatchingInfo(jobID,&root );

	if (!success) {
		LOG(LOG_FATAL, "Job %d does not exist in dice db job. ", jobID);
		return vector<ColPair>();
	}
	Json::Value colsNode = root["cols"];
	vector<string> cols;
	for(unsigned int i = 0;i < colsNode.size();i++) { cols.push_back(colsNode[i].asString()); }

	//Reconstruct the mapping options

	Json::Value candiateMatchNode = root["candidate_match"];
	map<string,map<string,double>> candidateMatchings;

	for(string col: cols) {
		Json::Value colMatchNode = candiateMatchNode[col];
		Json::Value matchedColNode = colMatchNode["match_cols"];

		vector<string> matchedCols;
		for(unsigned int i = 0;i < matchedColNode.size();i++) matchedCols.push_back(matchedColNode[i].asString());

		for(string matchedCol: matchedCols) {
			double matchingProb = colMatchNode[matchedCol].asDouble();
			candidateMatchings[col][matchedCol] = matchingProb;
		}
	}
//resolve the matching from the column based on the order
	LOG(LOG_DEBUG,"---------------------Construct Candidate Matching---------------");
	for(const auto& matching:candidateMatchings) {
		string col = matching.first;
		map<string,double> colsMatching = matching.second;
		LOG(LOG_DEBUG, "%s",col.c_str());
		for_each(colsMatching.begin(), colsMatching.end(), [colsMatching](const pair<string,double> colMatching) {
			LOG(LOG_DEBUG,"\t%s[%.3f]",colMatching.first.c_str(), colMatching.second);
		});
	}
	LOG(LOG_DEBUG,"----------------------------------------------------------------");

	//get the crowdsourcing answer from job ID
	//If there are more candidate matching, choose the one with high score
	map<string,string> csMatchings = this->crowdPlatform.getColMatching(jobID);

	vector<ColPair> finalMatchings;
	map<string,map<string,double>> remainingMatching;
	for(const auto& csMatch: csMatchings) {
		//take the matching from crowdsourcing answer as the ground truth
		LOG(LOG_DEBUG, "CS Match [%s]-[%s]", csMatch.first.c_str(),csMatch.second.c_str());
		if(csMatch.second != "N.A."){
			finalMatchings.push_back(ColPair(csMatch.first,csMatch.second));
		}

		remainingMatching =
				SchemaMatcher::filterMatching(candidateMatchings, csMatch);
		candidateMatchings = remainingMatching;
		remainingMatching.clear();


		LOG(LOG_DEBUG,"---------------------Remaining Matching---------------");
		for(const auto& matching:candidateMatchings) {
			string col = matching.first;
			map<string,double> colsMatching = matching.second;
			LOG(LOG_DEBUG, "%s",col.c_str());
			for_each(colsMatching.begin(), colsMatching.end(), [colsMatching](const pair<string,double> colMatching) {
				LOG(LOG_DEBUG,"\t%s[%.3f]",colMatching.first.c_str(), colMatching.second);
			});
		}
		LOG(LOG_DEBUG,"----------------------------------------------------------------");
	}//End of for csMatch

	//for the remaining candidate matchings, incorporate the matching with max probability
	while(!candidateMatchings.empty()) {
		string col1 = candidateMatchings.begin()->first;

		map<string,double> colMatching = candidateMatchings.begin()->second;
		const map<string, double>::iterator maxIt =
				max_element(colMatching.begin(), colMatching.end(),
						[](const pair<string,double>& lhs,const pair<string,double>& rhs)->bool {
							return lhs.second < rhs.second;
						});
		string col2 = maxIt->first;
		ColPair csMatch(col1,col2);
		finalMatchings.push_back(ColPair(col1,col2));

		remainingMatching =
				SchemaMatcher::filterMatching(candidateMatchings, csMatch);
		candidateMatchings = remainingMatching;
		remainingMatching.clear();
	}
	return finalMatchings;
}

map<ColPair,TypeDistribution> SchemaMatcher::matchSchema(const WebTable& wt1,
										  const WebTable& wt2) const{

	priority_queue<TablePattern> tpq1 = this->tpGen.generatePatterns(wt1, 2);
	priority_queue<TablePattern> tpq2 = this->tpGen.generatePatterns(wt2, 2);

	vector<TablePattern> tpv1 = pq2v(&tpq1);
	vector<TablePattern> tpv2 = pq2v(&tpq2);

	map<string, TypeDistribution> colTypeDist1 = SchemaMatcher::getColTypeDistribution(tpv1);
	map<string, TypeDistribution> colTypeDist2 = SchemaMatcher::getColTypeDistribution(tpv2);

	map<ColPair,TypeDistribution> colPairDist = SchemaMatcher::computeJointDist(colTypeDist1,colTypeDist2);
	return colPairDist;

}

map<ColPair, TypeDistribution> SchemaMatcher::computeJointDist(const map<string, TypeDistribution>& ctd1,
		 	 	 	 	 	 	 	 	 	 	const map<string, TypeDistribution>& ctd2) {
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

vector<TablePattern> SchemaMatcher::pq2v(priority_queue<TablePattern>* tpq){
	vector<TablePattern> tpv;
	while(!tpq->empty()){
		TablePattern tp = tpq->top();
		tpq->pop();
		tpv.push_back(tp);
	}
	return tpv;
}


TypeDistribution SchemaMatcher::getTypeJointDist(const TypeDistribution& td1, const TypeDistribution& td2){
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
map<string,TypeDistribution> SchemaMatcher::getColTypeDistribution(const vector<TablePattern>& tps){
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
map<TablePattern,double> SchemaMatcher::getTpProbability(const vector<TablePattern>& tps){
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
