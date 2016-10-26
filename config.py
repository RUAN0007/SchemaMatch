timon_url = "http://localhost:8000"
tp_server_url = "http://localhost:9000"
tp_callback_url = "http://localhost:9000/callback"
tp_port=9000
tp_job_id=1
tp_questionInfoPath = "tp_questions/tp_info.json"
tp_questionDirPath = "tp_questions" #Directory containing table pattern questions that have not been answered
tp_questionFinishPath = "tp_finish" #Directory containing table pattern questions that have been answered
tp_finalAnswerPath = 'tp_answers' #Directory containing aggregate table pattern answers for each job
tp_tempAnswerPath = "cs_temp/tp_answers.json" # A json file that maintains answer for table pattern questions whose job has not all finished. 
tp_postedQuestionPath = "cs_temp/tp_posted_questions.json" # A json file containing questionID -> entity_id for the questions has been posted but not answered. 


sm_server_url = "http://localhost:8500"
sm_callback_url = "http://localhost:8500/callback"
sm_port=8500
sm_job_id=2
sm_questionInfoPath = "sm_questions/sm_info.json"
sm_questionDirPath = "sm_questions" #Directory containing schema matching questions that have not been answered
sm_questionFinishPath = "sm_finish" #Directory containing schema matching questions that have been answered
sm_finalAnswerPath = 'sm_answers' #Directory containing aggregate schema matching answers for each job
sm_tempAnswerPath = "cs_temp/sm_answers.json" # A json file that maintains answer for schema matching questions whose job has not all finished. 
sm_postedQuestionPath = "cs_temp/sm_posted_questions.json" # A json file containing questionID -> entity_id for the questions has been posted but not answered. 



debug=True
