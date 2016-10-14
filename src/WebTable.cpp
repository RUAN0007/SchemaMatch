
#include "json.h"
#include "json-forwards.h"
#include "WebTable.h"
#define DEBUG
#include "debug.h"


WebTable::WebTable(string tname) {
	 this->name = tname;
}

void WebTable::addCol(string colHeader, vector<string> values){
	this->values[colHeader] = values;
	//DEBUG_STDOUT(colHeader);
	this->colHeaders.push_back(colHeader);
}
void WebTable::addCol(vector<string> values) {
	int headerSize = this->colHeaders.size();
	string newHeader = this->name + "_" + to_string(headerSize);
	this->addCol(newHeader,values);
}
void WebTable::setKeyColID(int keyID) {
	this->keyColID = keyID;
}

vector<string> WebTable::getColValues(string colHeader) const{
  map<string,vector<string>>::const_iterator colValIt = this->values.find(colHeader);

  if(colValIt == this->values.end()) return vector<string>();

  return colValIt->second;
}

map<string,vector<string>> WebTable::getValues() const{
  return this->values;
}


WebTable WebTable::parseWebTable(string tableName,string line)  {
	Json::Reader reader;  
 	Json::Value root;  
 	WebTable wt = WebTable(tableName);
 	reader.parse(line,root);

    const Json::Value cols = root["relation"];  
	
  if(reader.parse(line,root))  
  {  
  	  bool hasHeader = root["hasHeader"].asBool();
      string orientation = root["tableOrientation"].asString();  
      if(orientation.compare("HORIZONTAL") == 0) {
      	for (uint colID = 0;colID < cols.size();colID++) {
      		Json::Value col = cols[colID];
      		string colHeader = hasHeader?col[0].asString():tableName + "_" + to_string(colID);
      		uint rowID = hasHeader?1:0;
      		vector<string> values;
      		for (;rowID < col.size(); rowID++) {
      			values.push_back(col[rowID].asString());
      		}
      		wt.addCol(colHeader,values);

      	}
      }else if (orientation.compare("VERTICAL") == 0){
      	//VERTICAL Table
      	Json::Value col1 = cols[0];
      	vector<string> headers;
      	vector<vector<string>> values;

      	if(hasHeader) {
      		for(uint rid = 0;rid < col1.size();rid++) {
      			headers.push_back(col1[rid].asString());
      		}
      	}else {
      		for (uint rid = 0;rid < col1.size();rid++) {
      			headers.push_back(tableName + "_" + to_string(rid));
      		}
      	}
      	for (uint i = 0;i < headers.size();i++) {
      		vector<string> v; //Create an empty vector
      		values.push_back(v);
      	}
      	uint startCol = hasHeader?1:0;
      	for (uint colID = startCol;colID < cols.size();colID++) {
      		Json::Value col = cols[colID];
      		for (uint rowID = 0;rowID < col.size(); rowID++) {
      			values.at(rowID).push_back(col[rowID].asString());
      		}
      	}

      	auto headerIt = headers.begin();
      	int i = 0;
      	for(;headerIt != headers.end();headerIt++) {
      		wt.addCol(*headerIt,values.at(i));
      		i++;	
      	}
      }else{
      	DEBUG_STDOUT("Unrecognzied Web Table Orientation: " + tableName);
      }
  }else{
      	DEBUG_STDOUT("Error Parsing String" + line);
  }
    return wt;
}

void WebTable::print() {
  cout << "------------------------------------------------" << endl;
  cout << "Name: " << this->name << endl;
  cout << "Header: ";
  auto headerIt = this->colHeaders.begin();
  for (;headerIt != this->colHeaders.end();headerIt++) {
    cout << *headerIt << ", ";
  }
  cout << endl;

	headerIt = this->colHeaders.begin();
	for (;headerIt != this->colHeaders.end();headerIt++) {
		cout << "\t" << *headerIt << "-> ";
		vector<string> value = this->values[*headerIt];
		auto vit = value.begin();
		for (;vit != value.end();vit++) {
			cout << *vit << ", ";
		}
		cout << endl;

	}
  cout << "------------------------------------------------" << endl;


}
