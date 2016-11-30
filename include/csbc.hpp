/*
 * C++ Sqlite3 Database Connector
 */

#ifndef __CSBC__
#define __CSBC__

#pragma once

#include <iostream>
#include <string>
#include <sqlite3.h>
#include <map>
#include <list>
using namespace std;

class csbc {
  private:
    string dbPath;
    sqlite3 *db;
    char *zErrMsg = NULL;
    int oc;

  public:

    static map<string,list<string>> result;
    csbc(string dbPath): dbPath(dbPath) {
      oc = sqlite3_open(dbPath.c_str(), &db);
      if(oc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
      }
      else {
        cout << "Open database successfully" << endl;
      }
    }

    ~csbc() {
      sqlite3_close(db);
    }


    void dice_dbops(string sql) {
      if(sqlite3_exec(db,sql.c_str(),NULL,NULL, &zErrMsg) != SQLITE_OK) {
        cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
      }
      else {
        cout << "Operate DB successfully" << endl;
      }
    }

    void dice_select(string sql) {
      if(sqlite3_exec(db,sql.c_str(), callback, NULL, &zErrMsg) != SQLITE_OK) {
        cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
      }
      else {
        cout << "Select successfully" << endl;
      }
    }

    static int callback(void *data, int argc, char **argv, char **azColName){
      int i;
      for(i=0; i < argc; i++){
//        cout <<  azColName[i] << " = " <<  (argv[i] ? argv[i] : "NULL") << endl;
    	  string attrName = string(azColName[i]);
    	  string attrValue = string(argv[i]?argv[i]: "NULL");
    	  csbc::result[attrName].push_back(attrValue);
      }
      return 0;
    }

};

#endif
