/*
 * csbc.h
 *
 *  Created on: Dec 1, 2016
 *      Author: ruanpingcheng
 */

#ifndef INCLUDE_CSBC_H_
#define INCLUDE_CSBC_H_

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


    void dice_dbops(string sql);

    void dice_select(string sql);

    static int callback(void *data, int argc, char **argv, char **azColName);

};

#endif




#endif /* INCLUDE_CSBC_H_ */
