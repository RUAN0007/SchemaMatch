#include "SchemaMatching.h"
#include "math.h"
#include <queue>


vector<TablePattern> pq2v(priority_queue<TablePattern>* tpq);
TypeDistribution getTypeJointDist(const TypeDistribution& td1, const TypeDistribution& td2);
map<ColPair, TypeDistribution> computeJointDist(const map<string, TypeDistribution>& ctd1,
		 	 	 	 	 	 	 	 	 	 	const map<string, TypeDistribution>& ctd2);
/*The structure of return results:
 * key: colID
 * value: possible mapping type URI and probability
 * 		key:type URI
 * 		value: probability (double)
 */
map<string,TypeDistribution> getColTypeDistribution(const vector<TablePattern>& tps){
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
map<TablePattern,double> getTpProbability(const vector<TablePattern>& tps) {
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

map<ColPair,TypeDistribution> matchSchema(const WebTable& wt1,
										  const WebTable& wt2){

	KB kb;
	kb.init("http://epic.d1.comp.nus.edu.sg:8890/sparql");

	TPGenerator tpGen(&kb);
	priority_queue<TablePattern> tpq1 = tpGen.generatePatterns(wt1, 2);
	priority_queue<TablePattern> tpq2 = tpGen.generatePatterns(wt2, 2);

	vector<TablePattern> tpv1 = pq2v(&tpq1);
	vector<TablePattern> tpv2 = pq2v(&tpq2);

	map<string, TypeDistribution> colTypeDist1 = getColTypeDistribution(tpv1);
	map<string, TypeDistribution> colTypeDist2 = getColTypeDistribution(tpv2);

	map<ColPair,TypeDistribution> colPairDist = computeJointDist(colTypeDist1,colTypeDist2);
	return colPairDist;

}

map<ColPair, TypeDistribution> computeJointDist(const map<string, TypeDistribution>& ctd1,
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

vector<TablePattern> pq2v(priority_queue<TablePattern>* tpq){
	vector<TablePattern> tpv;
	while(!tpq->empty()){
		TablePattern tp = tpq->top();
		tpq->pop();
		tpv.push_back(tp);
	}
	return tpv;
}


TypeDistribution getTypeJointDist(const TypeDistribution& td1, const TypeDistribution& td2){
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
