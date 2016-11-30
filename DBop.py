'''
Created on 2016-11-29
@author: daijian
'''

import sqlite3
import sys

class DBop:

    def __init__(self,dbfile):
        self.dbfile=dbfile
    
    def exesql(self,sqlstatement):
        try:
            self.dbconn=sqlite3.connect(self.dbfile)
            cursor=self.dbconn.execute(sqlstatement)
            self.dbconn.commit()
            return True
        except sqlite3.Error, e:
            print "Error %s:" % e.args[0]
            return False
#             sys.exit(1)
        finally:
            if self.dbconn:
                self.dbconn.close()

    def exesql_withResults(self,sqlstatement):
        try:
            self.dbconn=sqlite3.connect(self.dbfile)
            cursor=self.dbconn.execute(sqlstatement)
            self.dbconn.commit()
            data=[]
            for row in cursor:
                data.append(row)
            self.dbconn.close()
            return data
        except sqlite3.Error, e:
            print "Error %s:" % e.args[0]
#             sys.exit(1)
        finally:
            if self.dbconn:
                self.dbconn.close()