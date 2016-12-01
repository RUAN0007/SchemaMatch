#include "csbc.h"

map<string,list<string>> csbc::result;

 void csbc::dice_dbops(string sql) {
      if(sqlite3_exec(db,sql.c_str(),NULL,NULL, &zErrMsg) != SQLITE_OK) {
        cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
      }
      else {
        cout << "Operate DB successfully" << endl;
      }
    }

    void csbc::dice_select(string sql) {
      if(sqlite3_exec(db,sql.c_str(), callback, NULL, &zErrMsg) != SQLITE_OK) {
        cerr << "SQL error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
      }
      else {
        cout << "Select successfully" << endl;
      }
    }

    int csbc::callback(void *data, int argc, char **argv, char **azColName){
      int i;
      for(i=0; i < argc; i++){
//        cout <<  azColName[i] << " = " <<  (argv[i] ? argv[i] : "NULL") << endl;
    	  string attrName = string(azColName[i]);
    	  string attrValue = string(argv[i]?argv[i]: "NULL");
    	  csbc::result[attrName].push_back(attrValue);
      }
      return 0;
}
