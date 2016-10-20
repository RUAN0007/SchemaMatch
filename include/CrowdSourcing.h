#pragma once
#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include "SchemaMatching.h"
#include "WebTable.h"

class Crowdsourcing {
public:
	/*
	 * Post questions regarding the correspondence between column and type
	 * args:
	 * 	candidateCorrespondence:
	 * 		key: a column ID
	 * 		value:a vector of possible types in knowledgebase
	 * 	wt: the WebTable that holds the column ID
	 * Return the job id
	 */
	int postColTypeCorrespondece(const map<string,vector<URI>>& candidateCorrespondece,
								 const WebTable& wt
								 ) const;

	/*
	 * Get the column type correspondence based on job ID
	 * If the job has not finished, return an empty map
	 */
	map<string, URI> getColTypeCorrespondence(int jobID) const;

	/*
	 * Post question regarding the schema matching
	 * Args:
	 * 	candidateMatching:
	 * 		key: a colID in wt1
	 * 		value: a vector of ColIDs in wt2 which are possible matches for the wt1's colID in the key
	 * 	wt1,wt2: Webtable for the schema matching
	 * 	Return the job id
	 */

	int postColMatching(const map<string, vector<string>>& candidateMatching,
							const WebTable& wt1,
							const WebTable& wt2) const;

	/*
	 * Get the result column matching based on the job ID
	 */
	vector<ColPair> getColMatching(int jobID) const;

	/*
	 * return whether a crowdsourcing job has finished based on the given job id
	 */
	bool hasJobFinished(int jobID) const;
};

class SchemaMatcher {
private:
	const Crowdsourcing crowdPlatform;
	const TPGenerator tpGen;

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
};
