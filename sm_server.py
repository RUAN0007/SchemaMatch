import traceback  
import random, math, time, thread, json, os, difflib
from flask import Flask, request, jsonify, send_from_directory
import requests as req
from Queue import Queue
app = Flask(__name__)

import config

timon_url = config.timon_url
server_url = config.sm_server_url
job_id = config.sm_job_id
debug = config.debug 
port = config.sm_port 
questionInfoPath = config.sm_questionInfoPath
tempAnswerPath = config.sm_tempAnswerPath
postedQuestionPath = config.sm_postedQuestionPath
questionDirPath = config.sm_questionDirPath
questionFinishPath = config.sm_questionFinishPath
finalAnswerPath = config.sm_finalAnswerPath

create_api = "/api/jobs/"+str(job_id)+"/entities"
detail_api = "/api/entities/"

callback_queue = Queue(maxsize=0)

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



def handle_callback():
    j2q = load_question_info()
    q2j = dict()
    for jid,qids in j2q.iteritems():
        for qid in qids:
            q2j[qid] = jid

    q2e = load_posted_questions()
    # print "init e2q: ",e2q
    e2q = {e:q for q,e in q2e.iteritems()}
    temp_answers = load_temp_answers()
    while(True):
        try:
            request_type,entity_id=callback_queue.get()
            entity_id = int(entity_id)
            if request_type=="MORE_ENTITY":
                print "Asking more entites"

                # Get all sm questions in folder
                # Filter out the question has been posted
                # Update the posted question list and write this list to file
                # Send the remaining questions to Timon
                entityCreated = False
                qids, to_post_questions = getUnpostedQuestion(q2e)
                for qid, question in zip(qids, to_post_questions): 
                   # print "Create entity for qid ", qid
                    entityCreated = True
                    entity = Entity()
                    entity.add_info("question",question)
                    headers = {'Accept': 'application/json','Content-Type':'application/json;charset=UTF-8'}
                    r = req.post(timon_url+create_api,data=entity.to_json(),headers=headers)
                    if r.ok:
                        result= r.json()
                        eid = int(result["id"])
                        q2e[qid] = eid
                        e2q[eid] = qid
                    else:
                        print "Post entity failed..."
                        print r.text 

                    #print "q2e: ", q2e
                    #print "e2q: ",e2q
                    dump_posted_questions(q2e)

                # retrieve the updated sm info from file
                # Construct two data structure
                #   j2q: jobID -> a list of Qid
                #   q2j: Qid -> jobID
                if entityCreated:
                    
                    load_question_info()


            elif request_type == "ENTITY_FINISHED":
                try:
                    r = req.get(timon_url+detail_api+str(entity_id))
                    if r.ok:
                        entity = r.json()
                    else:
                        print "Get entity %s failed!" % str(entity_id)
                except:
                    print "Get entity %s failed!" % str(entity_id)
               
                answers = json.loads(entity['answers'])
                info = json.loads(entity['info'])
                question = info['question']

                qid = e2q[entity_id]
                jid = q2j[qid]

                # print "Finishing Entity ID: ", entity_id
                # print "Answers: ", answers
                # print "Info: ",info
                # print "Question: ",question
                # print "Qid: ", qid
                # print "Jid: ", jid
                # print "j2q: ",j2q
                # print "q2j: ",q2j
                e2q.pop(entity_id,None)
                q2e.pop(qid,None)
                # print "Poping this entity/question: "
                # print "e2q: ",e2q
                # print "q2e: ",q2e
                dump_posted_questions(q2e)

                desert_question(qid) 


                temp_answers[qid] = answers[0]["match"]
                #Check whether all the questions in the job is finished
                job_finished = True
                for q_in_j in j2q[jid]:
                    if not temp_answers.has_key(q_in_j):
                        job_finished = False
                        break

                if job_finished:
                    print "Finish Job ",jid
                    sm_answers = dict()
                    match_answers = dict()
                    cols = []
                    for q_in_j in j2q[jid]:
                        #TODO prepare answers
                        # Only one kv value in temp_answers[q_in_j]
                        for col,match_col in temp_answers[q_in_j].iteritems():
                            cols.append(col)
                            match_answers[col] = match_col;

                    sm_answers["cols"] = cols
                    sm_answers["matches"] = match_answers


                    dump_job_answer(jid, sm_answers)

                dump_temp_answers(temp_answers)

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
                print "unkown request type: %s " % request_type
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

def getUnpostedQuestion(q2e):
    '''
        Get a list of unposted questions
    '''
    new_questions = []
    qids = []
    for question_filename in os.listdir(questionDirPath):
        if question_filename.endswith("sm_info.json"):
            continue

        qid = question_filename.split(".")[0]
        if q2e.has_key(qid):
            #This question has been posted before
            continue

        qids.append(qid)
        question_file = os.path.join(questionDirPath, question_filename)
        with open(question_file) as f:
            question = json.load(f)
        new_questions.append(question)

    return qids, new_questions


def desert_question(qid):

    #Move json questions to finish dir
    #This procedure is optional
    question_filename = qid + ".json"
    from_path = os.path.join(questionDirPath, question_filename)
    to_path = os.path.join(questionFinishPath,question_filename)
    print "from path: ", from_path
    print "to path: ", to_path
    # input()
    os.rename(from_path,to_path)

def dump_job_answer(jid, sm_answers):

    answer_file_path = os.path.join(finalAnswerPath, str(jid) + ".json")
    with open(answer_file_path,"w") as f:
        json.dump(sm_answers, f)


def load_question_info():
    print "Loading Question Info: "
    with open(questionInfoPath) as f:
        j2q = json.load(f)

    print "j2q: ",j2q
    return j2q

def load_posted_questions():
    q2e = dict()
    if not os.path.isfile(postedQuestionPath):
        return q2e
    with open(postedQuestionPath) as f:
        q2e = json.load(f)

    return q2e

def dump_posted_questions(q2e):

    with open(postedQuestionPath,"w") as f:
        json.dump(q2e,f)

def load_temp_answers():
    temp_answers = dict()
    if not os.path.isfile(tempAnswerPath):
        return temp_answers
    with open(tempAnswerPath) as f:
        temp_answers = json.load(f)
    return temp_answers

def dump_temp_answers(temp_answers):
    with open(tempAnswerPath, "w") as f:
        print "Dumping temp answers"
        json.dump(temp_answers,f)

   
if __name__ == "__main__":
    # init()
    thread.start_new_thread( handle_callback,())
    app.debug = debug 
    app.run(host='0.0.0.0', port= port)
