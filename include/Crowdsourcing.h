#pragma once
#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include "KnowledgeBase.h"
#include "WebTable.h"
#include "json.h"

class Crowdsourcing {
public:

	/*
	 * Post question regarding the schema matching
	 * Args:
	 * 	candidateMatching:
	 * 		key: a colID in wt1
	 * 		value: a vector of ColIDs in wt2 which are possible matches for the wt1's colID in the key
	 * 	wt1,wt2: Webtable for the schema matching
	 *
	 * Return:
	 * 	the ID of this crowdsourcing job
	 */

	int postColMatching(const map<string, vector<string>>& candidateMatching,
							const WebTable& wt1,
							const WebTable& wt2) const;

	/*
	 * Get the result column matching based on the job ID
	 *
	 * Return:
	 * 	key: col ID in wt1
	 * 	value:col ID in wt2
	 */
	map<string,string> getColMatching(int jobID) const;

};
