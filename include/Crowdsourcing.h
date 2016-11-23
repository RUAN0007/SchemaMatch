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
	 * Post questions regarding the correspondence between column and type
	 * args:
	 * 	candidateCorrespondence:
	 * 		key: a column ID
	 * 		value:a vector of candidate correspondent types in knowledgebase
	 * 	wt: the WebTable with the column ID
	 *
	 * return:
	 *  the job id
	 */
	int postColTypeCorrespondece(const map<string,vector<URI>>& candidateCorrespondece,
								 const WebTable& wt
								 ) const;

	/*
	 * Get the column type correspondence based on job ID
	 * If the job has not finished, return an empty map
	 *
	 * return:
	 * 	key: column ID
	 * 	value: correspondent concept type
	 */
	map<string, URI> getColTypeCorrespondence(int jobID) const;

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

	/*
	 * return whether a crowdsourcing job has finished based on the given job id
	 */
	bool hasJobFinished(int jobID) const;
};
