#pragma once
#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include "TablePattern.h"
#include "Crowdsourcing.h"

typedef pair<string,string> ColPair;
typedef map<URI,double> TypeDistribution;

class SchemaMatcher {
private:
	const Crowdsourcing crowdPlatform;
	const TPGenerator tpGen;
/*
 * Compute the type distribution for some columns
 */
	static map<string,TypeDistribution> getColTypeDistribution(const vector<TablePattern>& tps);
	static TypeDistribution getTypeJointDist(const TypeDistribution& td1, const TypeDistribution& td2);
	static vector<TablePattern> pq2v(priority_queue<TablePattern>* tpq);

	static map<ColPair, TypeDistribution> computeJointDist(const map<string, TypeDistribution>& ctd1 ,
		 	 	 	 	 	 	 	 	 	 	const map<string, TypeDistribution>& ctd2);

	static map<TablePattern,double> getTpProbability(const vector<TablePattern>& tps);

	static map<string,map<string,double>> filterMatching(
										const map<string,map<string,double>>& candidateMatches,
										ColPair match);

public:
	inline SchemaMatcher(const Crowdsourcing& csPlatform,const TPGenerator tpGenerator):
			crowdPlatform(csPlatform),tpGen(tpGenerator) {
	}

	~SchemaMatcher(){};

	/*
	 *return job id
	 */
	int askTablePattern(WebTable& webTable,unsigned int maxQuestion) const;

	bool isTablePatternReady(int jobID) const;

	/*
	 * Get the generated table pattern based on the job ID
	 * return empty table pattern if the table pattern has not been generated yet.
	 */
	TablePattern getTablePattern(int jobID) const;

	int askSchemaMatching(const WebTable& wt1, const WebTable& wt2, unsigned int maxQuestion) const;

	bool isSchemaMatchingReady(int jobID) const;

	vector<ColPair> getSchemaMatching(int jobID) const;
/*
 * Compute the agreed type distribution for a pair of columns
 */
	map<ColPair,TypeDistribution> matchSchema(const WebTable& wt1, const WebTable& wt2) const;

};

