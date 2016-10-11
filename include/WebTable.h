#pragma once
#include <vector>
#include <map>
#include <string>

using namespace std;
class WebTable{
private:
	string name;
	vector<string> 	colHeaders;
	map<string,vector<string>> values;
	int keyColID = -1;
public: 
	WebTable(string const tableName);
	void addCol(string colHeader,vector<string> values);
	void addCol(vector<string> values);
	void setKeyColID(int keyID);
	vector<string> getColValues(string colHeader);
	map<string,vector<string>> getValues();
	vector<string> getColHeaders() {return this->colHeaders;};
	void print();
	static WebTable parseWebTable(string tname, string line);
};




