import traceback  
import random, math, time, thread, json, os, difflib
from flask import Flask, request, jsonify, send_from_directory
import requests as req
from DBop import DBop
from Queue import Queue
app = Flask(__name__)

import config

timon_url = config.timon_url
debug = config.debug 
port = config.port 
dop = DBop(config.db_path)

detail_api = "/api/entities/"
callback_queue = Queue(maxsize=0)

def get_create_api(job_id):
    return "/api/jobs/"+str(job_id)+"/entities"

def success(data=""):
    res = dict(
        result="success",
        data=data
            )
    return jsonify(res) 

def failure(message):
    res = dict(
        result="failure",
        message=message
            )
    return jsonify(res) 


class Entity:
    def __init__(self):
        self.id=0
        self.info=dict()
        self.job=job_id
        return
    
    def add_info(self,key,value):
        self.info[key]=value
        return
    
    def to_json(self):
        return json.dumps(dict(
                            id=self.id,
                            job=job_id,
                            info=json.dumps(self.info)
                           ))

def get_unposted_questions():
    '''
        return a list of triplet tuples (qid(int), csid(int), question(string))
    '''
    sql = "SELECT qid, crowdsourcingid, sub FROM CSTask WHERE status = 'INIT'"
    return dop.exesql_withResults(sql)
    # return

def update_questions_status(qids, eids):
    '''
    For each tuple in CSTask identified by qid in qids
        update status = POSTED
        update info = Entity + str(eid), E.g, Entity15 if eid = 15 
    return:
        bool to indicate successful operation or not
    '''
    print "Posint Questions ", qids
    print "Entity", eids
    print

    qid_list = "(" + ",".join([str(qid) for qid in qids]) + ") "
    case_clause = "CASE qid "
    for qid,eid in zip(qids, eids):
        info = "Entity" + str(eid)
        case_clause += "WHEN " + str(qid) + " THEN "
        case_clause += "'" +info + "' "
    case_clause += "END "

    sql = "UPDATE CSTask SET "
    sql += "STATUS = 'POSTED', "
    sql += "INFO = " + case_clause
    sql += " WHERE qid in " + qid_list + ";"
    if debug:
        print sql
    return dop.exesql(sql)

def update_answer(eid, answer):
    '''
    Update the CSTask tuple in Dice DB to status = FINISHED and result = answer
    args:
        eid: entity ID for answered question in Timon(int)
        answer: the answer from Timon (str)

    return:
        bool to indicate successful operation or not
    '''

    if debug:
        print "Update Answer for entity", eid

    info = "Entity" + str(eid)
    sql = "UPDATE CSTask SET "
    sql += "STATUS = 'FINISH', "
    sql += "RESULT = '" + answer + "' "
    sql += "WHERE info = '" + info + "'; "

    if debug: 
        print sql 
    return dop.exesql(sql)

def handle_callback():
    while(True):
        try:
            request_type,entity_id=callback_queue.get()
            entity_id = int(entity_id)
            if request_type=="MORE_ENTITY":
                if debug:
                    print "Asking more entites"

                eids = [] # a list of entity IDs, returned by Timon
                qids = [] # a list of unposted question IDs
                for (qid, csid, question) in get_unposted_questions():
                    entity = Entity()
                    entity.add_info("question",question)
                    headers = {'Accept': 'application/json','Content-Type':'application/json;charset=UTF-8'}

                    create_api = get_create_api(csid) 
                    r = req.post(create_api,data=entity.to_json(),headers=headers)
                    if r.ok:
                        result= r.json()
                        eid = int(result["id"])
                        eids.append(eid)
                        qids.append(qid)
                    else:
                        print "Post entity failed..."
                        print r.text 

                update_questions_status(qids, eids)

            elif request_type == "ENTITY_FINISHED":
                if debug:
                    print "Receive Entity ", entity_id
                try:
                    r = req.get(timon_url+detail_api+str(entity_id))
                    if r.ok:
                        entity = r.json()
                    else:
                        print "Get entity %s failed!" % str(entity_id)
                except:
                    print "Get entity %s failed!" % str(entity_id)
               
                answers = str(entity['answers'])

                update_answer(entity_id, answers)

                # Remove this entity from timon
                try:
                    r = req.delete(timon_url+detail_api+str(entity_id))
                    if r.ok:
                        print r.text
                    else:
                        print "delete entity %s failed!" % str(entity_id)
                except:
                    print "delete entity %s failed!" % str(entity_id)

                #done this callback
            else:
                print "unknown request type: %s " % request_type
            callback_queue.task_done()
            time.sleep(0.1)
        except Exception,e: 
            traceback.print_exc()
            print "error occur"
    return

@app.route('/callback')
def callback():
    entity_id = request.args.get("id",0)
    request_type = request.args.get("type","ENTITY_FINISHED") # "MORE_ENTITY"
    callback_queue.put((request_type,entity_id))
    return success() 
   
if __name__ == "__main__":
    # init()
    update_answer(eid=11,answer="demo answer")
    # qids = [0,10,100,3]
    # eids = [10, 11,22,33]
    # update_questions_status(qids, eids)
    # get_unposted_questions()
    # thread.start_new_thread( handle_callback,())
    # app.debug = debug 
    # app.run(host='0.0.0.0', port= port)
