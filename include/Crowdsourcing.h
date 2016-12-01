#pragma once
#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include "KnowledgeBase.h"
#include "WebTable.h"
#include "json.h"
#include "dice_cs.h"

class Crowdsourcing {
public:
	static int CSID;
	/*
	 * Post question regarding the schema matching
	 * Args:
	 * 	jobID
	 * 	candidateMatching:
	 * 		key: a colID in wt1
	 * 		value: a vector of ColIDs in wt2 which are possible matches for the wt1's colID in the key
	 * 	wt1,wt2: Webtable for the schema matching
	 *
	 * Return:
	 * 	whether the question posting is successful or not.
	 */

	bool postColMatching(int jobID, const map<string, vector<string>>& candidateMatching,
							const WebTable& wt1,
							const WebTable& wt2) ;

	/*
	 * Get the result column matching based on the job ID
	 *
	 * Return:
	 * 	key: col ID in wt1
	 * 	value:col ID in wt2
	 */
	map<string,string> getColMatching(int jobID);

	Crowdsourcing(dice_cs* csConnector) {
		this->genericCS = csConnector;
	}
private:
	dice_cs* genericCS;
};
