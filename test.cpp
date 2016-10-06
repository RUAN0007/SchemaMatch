#include <iostream>
#define DEBUG
#include "src/debug.h"
#include <fstream>
#include "src/WebTable.h"

int main(){
	std::ifstream infile("webtable/2WebTables.json");
	string line;

	std::getline(infile, line) ;
	 	string tname = "tname";
	  WebTable wh = WebTable::parseWebTable("tname", line) ;
	 wh.print()	;
}