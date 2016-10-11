#pragma once
#include "IKnowledgeBase.h"
#include <map>
#include <vector>
#include <iostream>
using namespace std;
#include "TablePattern.h"

class ColTypeDistribution{
public:
	inline ColTypeDistribution(string colId) {
		this->colId = colId;
	}
	inline void assignURI(URI type, double probability) {
		this->typeProbability[type] = probability;
	}
private:
	string colId;
	map<URI,double> typeProbability;
};
map<TablePattern,double> getTpProbability(const vector<TablePattern>& tps);

map<string,map<URI,double>> getColTypeDistribution(const vector<TablePattern>& tps);
