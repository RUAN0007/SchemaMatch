#include "SchemaMatching.h"
#include "math.h"


/*The structure of return results:
 * key: colID
 * value: possible mapping type URI and probability
 * 		key:type URI
 * 		value: probability (double)
 */
map<string,map<URI,double>> getColTypeDistribution(const vector<TablePattern>& tps){
	map<TablePattern,double> patternProb = getTpProbability(tps);
	map<string,map<URI,double>> result;

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
