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
	 * Compute type distribution for each column based on the candidate table patterns and their score
	 */
	static map<string,TypeDistribution> getColTypeDistribution(const vector<TablePattern>& tps);

	/*
	 * Compute two column's joint type distribution based on the each column's type distribution
	 */
	static TypeDistribution getTypeJointDist(const TypeDistribution& td1, const TypeDistribution& td2);

	/*
	 * Convert a priortiy queue of table patterns to vector
	 */
	static vector<TablePattern> pq2v(priority_queue<TablePattern>* tpq);

	/*
	 * Compute type distribution of a column pair from two tables
	 * based on each table's column's type distribution
	 */
	static map<ColPair, TypeDistribution> computeJointDist(const map<string, TypeDistribution>& ctd1 ,
		 	 	 	 	 	 	 	 	 	 	const map<string, TypeDistribution>& ctd2);

	/*
	 * Compute the probability of each table pattern based on its score
	 * The sum of probability is summed to one.
	 */
	static map<TablePattern,double> getTpProbability(const vector<TablePattern>& tps);

	/*
	 * Filter out the candidate matching options that is in conflict with the proposed match
	 */
	static map<string,map<string,double>> filterMatching(
										const map<string,map<string,double>>& candidateMatches,
										ColPair match);

	/*
	 * Compute type distribution of a column pair where one column in wt1 and the other in wt2
	 */
	map<ColPair,TypeDistribution> matchSchema(const WebTable& wt1, const WebTable& wt2) const;

public:
	inline SchemaMatcher(const Crowdsourcing& csPlatform,const TPGenerator tpGenerator):
			crowdPlatform(csPlatform),tpGen(tpGenerator) {
	}

	~SchemaMatcher(){};


	int askSchemaMatching(const WebTable& wt1, const WebTable& wt2, unsigned int maxQuestion) const;

	vector<ColPair> getSchemaMatching(int jobID) const;

};

