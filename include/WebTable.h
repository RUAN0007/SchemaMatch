#pragma once
#include <vector>
#include <map>
#include <string>
#include <json.h>
using namespace std;
class WebTable{
private:
	string name;
	vector<string> 	colHeaders;
	map<string,vector<string>> values;
	int keyColID = -1;
public: 
	WebTable(){};
	WebTable(string const tableName);
	void addCol(string colHeader,vector<string> values);
	void addCol(vector<string> values);
	void setKeyColID(int keyID);
	vector<string> getColValues(string colHeader) const;
	map<string,vector<string>> getValues() const;
	vector<string> getColHeaders() const {return this->colHeaders;} ;
	void print() ;
	static WebTable parseWebTable(string tname, string line) ;

	inline Json::Value serialize() const{
		Json::Value jsonObj;
		jsonObj["name"] = this->name;

		Json::Value headerNode;
		for(string header:this->colHeaders) headerNode.append(header);
		jsonObj["headers"] = headerNode;

		for(auto valueIt = this->values.begin();valueIt != this->values.end();valueIt++) {
			string header = valueIt->first;
			vector<string> value = valueIt->second;
			Json::Value valueNode;
			for(string v:value) valueNode.append(v);
			jsonObj[header] = valueNode;
		}
		return jsonObj;
	}

	inline static WebTable deserialize(Json::Value jsonObj) {
		string name = jsonObj["name"].asString();
		WebTable wt(name);
		Json::Value headerNode = jsonObj["headers"];

		for(unsigned int i = 0;i < headerNode.size();i++) {
			string header = headerNode[i].asString();
			Json::Value valueNode = jsonObj[header];
			vector<string> value;
			for(unsigned int j = 0;j < valueNode.size();j++) {
				value.push_back(valueNode[j].asString());
			}//End for for j
			wt.addCol(header, value);
		}//End of for
		return wt;
	}

	inline static WebTable deserialize(string valueStr, bool* failed) {
		Json::Reader reader;
		Json::Value jsonObj;

		WebTable wt;
		if(reader.parse(valueStr, jsonObj)) {
			*failed = false;
			wt = WebTable::deserialize(jsonObj);
		}else{
			*failed = true;
		}
		return wt;
	}
};




