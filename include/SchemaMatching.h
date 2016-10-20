#pragma once
#include "KnowledgeBase.h"
#include <map>
#include <vector>
#include <iostream>
using namespace std;
#include "TablePattern.h"

typedef pair<string,string> ColPair;
typedef map<URI,double> TypeDistribution;

/*
 * Compute the probability of each TablePattern
 */
map<TablePattern,double> getTpProbability(const vector<TablePattern>& tps);

/*
 * Compute the type distribution for some columns
 */
map<string,TypeDistribution> getColTypeDistribution(const vector<TablePattern>& tps);

/*
 * Compute the agreed type distribution for a pair of columns
 */
map<ColPair,TypeDistribution> matchSchema(const WebTable& wt1, const WebTable& wt2);

