/***************************************************************************
 *   Copyright (C) 2009 by Matthew Dellinger   *
 *   mdelling@vt.edu   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "rt_task.h"
#include <chronos/chronos.h>
/*********************** pnf-START *******************************/
#include <string>
#include <cmath>
/*********************** pnf-END *******************************/

#define tsprint(x) x.tv_sec << " sec + " << x.tv_nsec << " nsec"
#define tsprintp(x) x->tv_sec << " sec + " << x->tv_nsec << " nsec"
#define THREAD_OUTPUT if(me->verbose() && accrue) *me->output() << me->thread_id() 

/*************************** SH-START *********************************/
/*************************** SH-START-2 *********************************/
vector<CounterBench> *coun_obj;
/*************************** SH-END-2 *********************************/
/*************************** SH-END *********************************/

RtTester *tester;

void Task::SetupTask(RtTester *t, ostringstream *output, chronos_aborts_t *a, double slope) {
	tester = t;
	output_ = output;
	exec_slope_ = slope;
	aborts_ = a;
}

/* Hook in here for different kinds of workloads */
void Task::Work(long long time_us) {
	burn_cpu_abort(time_us, exec_slope_, abort_flag_);
}

int Task::start(pthread_attr_t *attr) {
	return pthread_create(&thread, attr, task, (void *)this);
}

void Task::BeginRtSegment() {
	begin_rtseg_self(TASK_RUN_PRIO, utility(), &deadline_, period_ts(),
		unlocked_usage() + locked_usage());
}

void Task::SetupAbortHandler() {
	if(tester->use_hua() && abort_utility() > 0)
		add_abort_handler_selfnodeadline(abort_utility(),
				tester->lock_time()*tester->lock_count());
}

void Task::EndRtSegment() {
	end_rtseg_self(TASK_CLEANUP_PRIO);
}

long Task::timespec_subtract(struct timespec *x, struct timespec *y) {
	long sec, nsec;
	sec = x->tv_sec - y->tv_sec;
	nsec = x->tv_nsec - y->tv_nsec;
	if(nsec < 0) {
		nsec += BILLION;
		sec--;
	}

	return sec * MILLION + nsec/THOUSAND;
}

void Task::UpdateDeadline() {
	int carry;
	unsigned long long nsec, offset = period_ * releases_;

	nsec = start_time_->tv_nsec + offset * THOUSAND;
	carry = nsec/BILLION;

	deadline_.tv_nsec = nsec % BILLION;
	deadline_.tv_sec = start_time_->tv_sec + carry;
}

void *Task::task(void *value) {
	Task *me = (Task *)value;
	unsigned long cpus = me->cpus();
	bool accrue = true;
	long sleeptime;
	struct timespec end_time;

	me->set_thread_id(gettid());
	me->set_abort_flag();
	sched_setaffinity(0, sizeof(long), (cpu_set_t *) &cpus);
        
        /******************************* SH-START ***********************************/
	if(isSTM(sync_alg)){
	        stm::init(sync_alg,"vis-eager",false);
	}
        /******************************* SH-END ***********************************/

	//while(accrue) {
	//	accrue = me->AddRelease();
        /*********************** SH-START ***************************/
	int inst_id=0;	//instance(job) id of current task
        while((accrue = me->AddRelease())){
        /*********************** SH-END ***************************/
		me->UpdateDeadline();

		THREAD_OUTPUT << "/" << me->task_id() << " - Deadline: " << tsprintp(me->deadline()) << endl;

		/* Declare ourself as a real time task */
		if(tester->chronos()) {
			me->SetupAbortHandler();
			me->BeginRtSegment();
		} else {
			struct sched_param param;
			param.sched_priority = TASK_RUN_PRIO;
			sched_setscheduler(0, SCHED_FIFO, &param);
		}
		if(me->locking() && me->nested()) {
			int last;
			for(int lock_num = 0; lock_num < me->CountLocks(); lock_num++) {
				int r = me->lock(lock_num);
				THREAD_OUTPUT << " locking " << r << endl;
				if(chronos_mutex_lock(tester->lock(r)) == -1)
					break;
				else
					last = lock_num;
				THREAD_OUTPUT << " locked " << r << endl;
			}

			THREAD_OUTPUT << " burning - locked" << endl;
			me->Work(me->locked_usage());

			for(int lock_num = last; lock_num >= 0; lock_num--) {
				int r = me->lock(lock_num);
				THREAD_OUTPUT << " unlocking " << r << endl;
				chronos_mutex_unlock(tester->lock(r));
			}
		} else if(me->locking()){
			for(int lock_num = 0; lock_num < me->CountLocks(); lock_num++) {
				int r = me->lock(lock_num);
				THREAD_OUTPUT << " locking " << r << endl;
				if(chronos_mutex_lock(tester->lock(r)) == -1)
					break;
				THREAD_OUTPUT << " locked " << r << endl;
				THREAD_OUTPUT << " burning - locked" << endl;
				me->Work(me->locked_usage()/me->CountLocks());
				THREAD_OUTPUT << " unlocking " << r << endl;
				chronos_mutex_unlock(tester->lock(r));
			}
		}

		THREAD_OUTPUT << " burning - unlocked" << endl;

		//me->Work(me->unlocked_usage());
                /************************* SH-START *******************************/
                int j=me->vec[0][0].size();
                int num_loops;
                /************************ pnf START ******************************/
                //Assign pnf-parameter depending on scheduler
                //struct timespec* pnf_args;
                struct task_in_param cm_args;
                if(tester->getSched()==SCHED_RT_GEDF){
                    //pnf_args=me->deadline();
                    cm_args.time_param=me->deadline();
                }
                else if(tester->getSched()==SCHED_RT_GRMA){
                    //pnf_args=me->period_ts();
                    cm_args.time_param=me->period_ts();
                }
		cm_args.gen_eta=0;
		cm_args.task_run_prio=TASK_RUN_PRIO;
		cm_args.task_util=me->utility();
		cm_args.task_deadline=me->deadline();
		cm_args.task_period=me->period_ts();
		cm_args.task_unlocked=me->unlocked_usage();
		cm_args.task_locked=me->locked_usage();
		cm_args.task_end_prio=TASK_CLEANUP_PRIO;
                /************************ pnf END ******************************/
		/************************* Debug 1 start *********************/
		//struct sched_param param_t;
		//sched_getparam(0, &param_t);
		//cout<<"th_id:"<<(me->thread_id())<<",pri:"<<(param_t.sched_priority)<<endl;
		/************************* Debug 1 end ************************/
                for(int i=0;i<j;i++){
                    if(me->vec[0][0][i]==0){
                        me->Work(me->vec[3][0][i]);
                    }
                    else{
                        /************************** pnf-START ****************************/
                        //Priority cannot be changed during rt_seg. So, we have to end it here
                        //and start it again after transaction finishes
                        /************************** pnf-END ****************************/
                        num_loops=me->vec[3][0][i]/final_stm_slope;
                        //coun_obj->at((int)(me->vec[2][i])).pnf_multi_reset(num_loops,(void*)(pnf_args);
			if(!sync_alg.compare("LCM")){
				(coun_obj->at(0)).lcm_multi_reset(num_loops,PSY,me->vec[3][0][i],(void*)(&cm_args),me->vec[2][i],(me->thread_id()),me->wr_per);
			}
			else if(!sync_alg.compare("ECM") || !sync_alg.compare("RCM")){
	                        (coun_obj->at(0)).multi_reset(num_loops,(void*)(&cm_args),me->vec[2][i],(me->thread_id()),me->wr_per);
			}
			else if(!sync_alg.compare("PNF")){
				me->EndRtSegment();
				(coun_obj->at(0)).multi_reset(num_loops,(void*)(&cm_args),me->vec[2][i],(me->thread_id()),me->wr_per);
				me->BeginRtSegment();	
			}
			else if(!sync_alg.compare("FBLT")){
				cm_args.gen_eta=me->vec[4][0][i]==0?0:(rand()%((int)(me->vec[4][0][i])))/20;
                        	(coun_obj->at(0)).lcm_multi_reset(num_loops,PSY,me->vec[3][0][i],(void*)(&cm_args),me->vec[2][i],(me->thread_id()),me->wr_per);
			}
                        /************************** pnf-START ****************************/
                        /************************** pnf-END ****************************/
                    }
                }
                /************************* SH-END *******************************/
		clock_gettime(CLOCK_REALTIME, &end_time);
                /************************* SH-START *******************************/
//		(me->task_result)->at(0).push_back((unsigned long long)(me->thread_id()));
		(me->task_result)->at(0).push_back((unsigned long long)inst_id);
                (me->task_result)->at(4).push_back((unsigned long long)(subtract_ts_mo(me->period_ts(),me->deadline())));
                (me->task_result)->at(5).push_back(((unsigned long long)(me->deadline()->tv_sec))*BILLION+(me->deadline()->tv_nsec));
                (me->task_result)->at(6).push_back(((unsigned long long)(end_time.tv_sec))*BILLION+(end_time.tv_nsec));
                (me->task_result)->at(7).push_back(((unsigned long long)(end_time.tv_sec))*BILLION+(end_time.tv_nsec)-(subtract_ts_mo(me->period_ts(), me->deadline())));
		inst_id++;
//		(me->log_result)->push_back(stm::printLog());
//		stm::newInst();
                /************************* SH-END *******************************/
                
                /******************* SH_SQL_ST ********************/
                //the following operation are not imporatant with SQL.
                vector<unsigned long long> cur_st_vec=stm::printStatistics();
                vector<unsigned long long> tmp_st_vec=cur_st_vec;
                tmp_st_vec[0]-=me->prev_commits;        //current instance number of commits
                me->prev_commits=cur_st_vec[0];         //update to be used in next instance
                tmp_st_vec[1]-=me->prev_abr_no;         //current instance number of aborts
                me->prev_abr_no=cur_st_vec[1];          //update to be used in next instance
                tmp_st_vec[2]-=me->prev_abr_dur;        //current instance amount of abort duration
                me->prev_abr_dur=cur_st_vec[2];         //update to be used in next instance
                (me->task_result)->at(1).push_back(tmp_st_vec[0]);
                (me->task_result)->at(2).push_back(tmp_st_vec[1]);
                (me->task_result)->at(3).push_back(tmp_st_vec[2]);
                /******************* SH_SQL_END ********************/
		if(tester->chronos())
			me->EndRtSegment();

		if(me->aborted())
			me->AddAborted();
		if(accrue && deadline_met(&end_time, me->deadline())) {
			me->AddDeadline();
			me->AddUtility();

			THREAD_OUTPUT << "/" << me->task_id() << " end time: " << tsprint(end_time)
				<< " - Deadline " << me->releases()
				<< ": " << tsprintp(me->deadline()) << endl;
		}
		else if(me->verbose() && accrue)
			*me->output() << "task failed " << me->thread_id() << "/" << me->task_id()
			<< " end time: " << tsprint(end_time) << " - Deadline "
			<< me->releases() << ": " << tsprintp(me->deadline()) << endl;

		sleeptime = timespec_subtract(me->deadline(), &end_time);
		if(sleeptime > 0) {
			usleep(sleeptime);
		} else
			me->set_max_tardiness(sleeptime);
	}
        /************************* SH-SQL_ST ******************************/
        //comment the following line in the other applications
        //(me->task_result)->at(1)=stm::printStatistics();
        /************************* SH-SQL_END ******************************/

	pthread_exit(NULL);
}

/********************** MULTIOBJECT-START *********************/
/*
vector<double> Task::genObj(int num_obj, double min_obj_per, int num_tasks, int task_id){
    vector<double> gen_obj;
    gen_obj.push_back(0);
    return gen_obj;
}

vector<double> Task::genObj(int num_obj,double min_obj_no,int num_tasks,int task_id){
    //Generates random distinct objects for the current transaction
    vector<double> obj_inx;        //vector of all available objects
    vector<double> gen_obj;        //vector of distinct generated objects to be allocated for the current transaction
    //Specify number of objects for current task
    int m_num_tasks=num_tasks>1?num_tasks-1:1;
    int max_obj_no=ceil((double)(1-num_obj)/(m_num_tasks)*task_id+num_obj);
    
    int obj_loc;                //Chosen object location from obj_inx
    //Initialize obj_inx to all available objects
    for(int i=0;i<num_obj;i++){
        obj_inx.push_back(i);
    }
    //Generate vector of objects for current transaction
    while(max_obj_no>0){
        //srand(time(NULL));
        obj_loc=rand()%num_obj;
        gen_obj.push_back(obj_inx[obj_loc]);
        obj_inx.erase(obj_inx.begin()+obj_loc);
	num_obj--;
        max_obj_no--;
    }
    return gen_obj;
}
*/
vector<double> Task::genObj_def(int num_obj,double min_obj_no,int num_tasks,int task_id){
    //Generates random distinct objects for the current transaction
    vector<double> obj_inx;        //vector of all available objects
    vector<double> gen_obj;        //vector of distinct generated objects to be allocated for the current transaction
    //Specify number of objects for current task
    int min_obj_no_tmp=(int)(ceil(min_obj_no*num_obj));
    int max_obj_no=((int)(rand())%(num_obj-min_obj_no_tmp))+min_obj_no_tmp;
    
    int obj_loc;                //Chosen object location from obj_inx
    //Initialize obj_inx to all available objects
    for(int i=0;i<num_obj;i++){
        obj_inx.push_back(i);
    }
    //Generate vector of objects for current transaction
    while(max_obj_no>0){
        //srand(time(NULL));
        obj_loc=rand()%num_obj;
        gen_obj.push_back(obj_inx[obj_loc]);
        obj_inx.erase(obj_inx.begin()+obj_loc);
        num_obj--;
        max_obj_no--;
    }
    return gen_obj;
}


vector<double> Task::genObj(int num_obj,double min_obj_no,int num_tasks,int task_id){
    //check whether number of objects is correct accroding to condition
    if(min_obj_no<0){
	return genObj_def(num_obj,fabs(min_obj_no),num_tasks,task_id);
    }
    int num_obj_task=2*num_obj/(num_tasks+1);   //holds number of objects per task
    if(!((num_obj_task>=0 && num_obj_task%2==0) || (num_obj==1))){
	cout<<"Not accepted number of objects for transitive retry or single object"<<endl;
	exit(0);
    }
    //Generates random distinct objects for the current transaction
    vector<double> gen_obj;        //vector of distinct generated objects to be allocated for the current transaction
    if(num_obj==1){
	//In case one shared object among all tasks
	gen_obj.push_back(0);
	return gen_obj;
    }
    else{
	int st_ind=task_id*num_obj_task/2;	//first object to be allocated for current task
	int end_ind=st_ind+num_obj_task-1;	//Last object allocated to current task
	for(int i=st_ind;i<=end_ind;i++){
		gen_obj.push_back(i);
	}
    }
    return gen_obj;
}

vector<double> Task::genObj_old(int num_obj,double min_obj_no,int num_tasks,int task_id){
    //Generates random distinct objects for the current transaction
    vector<double> obj_inx;        //vector of all available objects
    vector<double> gen_obj;        //vector of distinct generated objects to be allocated for the current transaction
    /******************** MIN_OBJ_START **********************/
    //The following line is changed from old projects
    int tmp_k=0;
    if(num_tasks>1){
	tmp_k=task_id*((int)ceil((float)(num_obj-1)/(num_tasks-1)));
    }
    else{
	tmp_k=num_obj;
    }
    int tmp_num_obj=num_obj-tmp_k>0?num_obj-tmp_k:1;
    int tmp_min_obj=(int)ceil(min_obj_no*tmp_num_obj);
    tmp_min_obj=tmp_min_obj>0?tmp_min_obj:1;
    int max_obj_no=0;
    while(max_obj_no==0){
	    if(tmp_num_obj==tmp_min_obj){
		max_obj_no=tmp_num_obj;
	    }
	    else{
		max_obj_no=(int)(rand()%(tmp_num_obj-tmp_min_obj))+tmp_min_obj;             //Maximum number of objects per current transaction
	    }
    }
    /******************** MIN_OBJ_END **********************/
    int obj_loc;                //Chosen object location from obj_inx
    //Initialize obj_inx to all available objects
    for(int i=0;i<num_obj;i++){
        obj_inx.push_back(i);
    }
    //Generate vector of objects for current transaction
    while(max_obj_no>0){
        //srand(time(NULL));
        obj_loc=rand()%num_obj;
        gen_obj.push_back(obj_inx[obj_loc]);
        obj_inx.erase(obj_inx.begin()+obj_loc);
	num_obj--;
        max_obj_no--;
    }
    return gen_obj;
}

/********************** MULTIOBJECT-END *********************/
/*********************** SH_SQL_ST *****************************/
void Task::addPortions(struct rt_task *task_info){
    //Another form of addPortions that uses MySQL database file
    Task::vec.push_back(por_type);
    Task::vec.push_back(por_length),
    Task::vec.push_back(por_counter);
    Task::vec.push_back(mod_por_length);
    vec[0].push_back(vector<double>());
    vec[1].push_back(vector<double>());
    vec[3].push_back(vector<double>());
    for(unsigned int i=0;i<task_info->portions.size();i++){
        vec[0][0].push_back(task_info->portions[i].por_type);
        vec[1][0].push_back(task_info->portions[i].por_len);
        vec[3][0].push_back(task_info->portions[i].mod_por_len);
        vec[2].push_back(task_info->portions[i].por_obj);
    }
}
/*********************** SH_SQL_END *****************************/

/************************** SH-START **************************/
/****************** SH-START-2***********************/
void Task::addPortions(double total_exec, double rt_exec, double stm_exec,ifstream* stmfile,string stmfile_name,double max_sec_per, double min_sec_per,double total_sec_per,int num_obj,double min_obj_per_in,int num_tasks_in,int task_id_in){
    //max_sec_per (min_sec_per) is the maximum (minimum) length percentage for any atomic section in the specified task
    //This percentage value is relative to WCET of the task
    //total_sec_per is the total length percentage of all atomic sections in the current task
    /*********************** Debug 2 start *************************/
//    cout<<"Entered addPortions, t_id:"<<task_id_in<<", num_objs:"<<num_obj<<endl;
    /*********************** Debug 2 end *************************/
    try{
        double t_e;
        double curr_exec;    //curr_exec holds either the value of rt_exec or stm_exec
        int index;                  // indicates the current portion either rt or stm
        int n;                      // number of units of current portion
        //set number of shared objects
        num_sh_obj=num_obj;
        //Random seed to random number of objects
        
        Task::vec.push_back(por_type);
        Task::vec.push_back(por_length),
        Task::vec.push_back(por_counter);
        Task::vec.push_back(mod_por_length);
        
        /****************** MULTIOBJECT-START **********************/
        vec[0].push_back(vector<double>());
        vec[1].push_back(vector<double>());
        vec[3].push_back(vector<double>());
        /****************** MULTIOBJECT-END **********************/
        
        if(*stmfile){
            //If the file exists, read structure of each task from it
            char s[256];
            while(true){
                /****************** Debug 3 start ******************/
  //              cout<<"Read a new STM line"<<endl;
                /****************** Debug 3 end ******************/
                stmfile->getline(s,512);
                if(!(strcmp(s,"***"))||stmfile->eof()){     //Check we have not reached the end of file or it is the end for the data structure for the current task
                    break;
                }
                vec[0][0].push_back(atof(s));
                stmfile->getline(s,512);
                vec[1][0].push_back(atof(s));
                vec[3][0].push_back(atof(s));
                stmfile->getline(s,512);
                /********************** MULTIOBJECT-START *****************************/
                //The following line should be commented in other projects as we 
                //are going to read multiple objects per transaction
                //vec[2].push_back(atof(s));
                /*********************** Debug 4 start *********************/
    //            cout<<"Read objecst are "<<s<<endl;
                /*********************** Debug 4 end *********************/
                string objs(s);
                int indx=0;
                int tmp_indx=0;
                vector<double> tmp_obj_no;
                while((indx=objs.find(",",indx))!=string::npos){
                    tmp_obj_no.push_back(atof((objs.substr(tmp_indx,indx-tmp_indx)).data()));
                    tmp_indx=++indx;
                }
                tmp_obj_no.push_back(atof((objs.substr(tmp_indx)).data()));    //To read the last object
                vec[2].push_back(tmp_obj_no);
                /********************* Debug 5 start **********************/
/*
                cout<<"Added objects are ";
                for(int i=0;i<(vec[2].back().size());i++){
                    cout<<vec[2].back().at(i)<<",";
                }
                cout<<endl;
*/
                /********************* Debug 5 end **********************/
                /********************** MULTIOBJECT-END *****************************/
            }
        }
        else{
            //If the file does not exist, create a new one, add a random structure for each task and record that in a new file so it can be used latter
            //Initialize index to either 0 (for rt) or 1 (for stm)
            ofstream ostmfile(stmfile_name.c_str(),fstream::app);
            double max_sec_len=0; //Maximum section length of the current task
            double min_sec_len=0; //Minimum section length of the current task
            double total_len=0;   //Total length of atomic sections of the current task
            //The following if-else cases are for initialization of index and curr_exec values
            if(max_sec_per==0 || total_sec_per==0){
                //In this case, task has no atomic sections at all
                /******************** Debug 5 start *********************/
  //              cout<<"start no STM will be added"<<endl;
                /******************** Debug 5 end *********************/
                n=(int)(total_exec/rt_exec);
                vec[0][0].push_back(0);
                vec[1][0].push_back(n*rt_exec);
                vec[3][0].push_back(n*rt_exec);
                vec[2].push_back(vector<double>(1,-1));
                ostmfile<<vec[0][0].back()<<endl;
                ostmfile<<vec[1][0].back()<<endl;
                ostmfile<<"-1"<<endl;
                /******************** Debug 6 start *********************/
    //            cout<<"end no STM will be added"<<endl;
                /******************** Debug 6 end *********************/
            }
            else if(min_sec_per>max_sec_per){
                //This is wrong, as min should be at most equal to max.
                cout<<"Minimum section length is bigger than the maximum. This is wrong"<<endl;
                cout<<"Please, run the program again with the correct values"<<endl;
                exit(0);
            }
            else if(min_sec_per==1){
                //In this case, all the task is a single atomic section
                n=(int)(total_exec/stm_exec);
                vec[0][0].push_back(1);
                vec[1][0].push_back(n*stm_exec);
                vec[3][0].push_back(n*stm_exec);
                /********************* MULTIOBJECT-START *************************/
                //srand(time(NULL));
                //The following line should be commented in other projects. The new steps
                //is to allocate multiple objects for vec[2], not a single object
                //vec[2].push_back(rand()%num_sh_obj);
                vec[2].push_back(genObj(num_sh_obj,min_obj_per_in,num_tasks_in,task_id_in));
		/********************* MULTIOBJECT-END *************************/
                ostmfile<<vec[0][0].back()<<endl;
                ostmfile<<vec[1][0].back()<<endl;
                /********************* MULTIOBJECT-START *************************/
                //The following line should be commented in other projects. The new steps
                //is to write multiple objects of vec[2], not a single object
                //ostmfile<<vec[2].back()<<endl;
                for(unsigned int z=0;z<(vec[2].back()).size()-1;z++){
                    //1 is subtracted from size to add "," after each object except the last one
                    ostmfile<<(vec[2].back())[z]<<",";
                }
                ostmfile<<(vec[2].back()).back()<<endl; //To print last object from the previous for loop
                /********************* MULTIOBJECT-END *************************/
                               
            }
            else{
                //In this case, length of each atomic section falls between two different levels
                //Specify these two levels, and the total length of all atomic sections in this task
                int tmp_max=(int)(total_exec*max_sec_per/stm_exec);
                max_sec_len=tmp_max*stm_exec;
                int tmp_min=(int)(total_exec*min_sec_per/stm_exec);
                min_sec_len=tmp_min*stm_exec;
                int tmp_total=(int)(total_exec*total_sec_per/stm_exec);
                total_len=tmp_total*stm_exec;
/******************* Debug 13 start ********************/
//cout<<"max_sec_len:"<<max_sec_len<<", min_sec_len:"<<min_sec_len<<endl;
/****************** Debug 13 end **************************/
                //Initialize index, then cur_exec according to the initialized index
                /***************** MULTIOBJECT-START ******************/
                //srand(time(NULL));
                /***************** MULTIOBJECT-END ******************/
                index=rand();
                index=index>RAND_MAX/2?1:0;
		/***************** Debug 20 start **********************/
		index=1;
		/***************** Debug 20 end ************************/
                curr_exec=(1-index)*rt_exec+index*stm_exec;
                //t_e will hold only the amount of time left for the non-atomic sections
                t_e=total_exec-total_len;
                /***************** Debug 5 start *****************/
  //              cout<<"non_atomic_len:"<<t_e<<", atomic_len:"<<total_len<<endl;
                /***************** Debug 5 end *****************/
                do{
                    /***************** Debug 5 start *****************/
    //                cout<<"non_atomic_len:"<<t_e<<", atomic_len:"<<total_len<<endl;
//		    cout<<"pppppppppppppppppppppppppppppppppp"<<endl;
                    /***************** Debug 5 end *****************/
                    if(index==0 && total_len<min_sec_len /* Non-atomic section and no more atomic sections */){
                        /******************** Debug 6 start *******************/
  //                      cout<<"last section (non_atomic) with n ";
                        /******************** Debug 6 end *******************/
                        n=(int)(t_e/rt_exec);
                        /******************* Debug 7 start ******************/
    //                    cout<<n<<endl;
                        /******************* Debug 7 end ******************/
                    }
                    else if(index==0 /* Non-atomic section but still more atomic sections coming*/){
                        /***************** Debug 8 start *******************/
      //                  cout<<"Non-atomic section with n ";
                        /***************** Debug 9 start *******************/
                        do{
			    if(t_e<=curr_exec){
                                n=1;
                            }
                            else{
                                n=(int)(fmod((double)rand(),t_e)/curr_exec);
                            }
                        }while(n==0);
                        /****************** Debug 9 start ***************/
        //                cout<<n<<endl;
                        /****************** Debug 9 end ***************/
                    }
                    else if(index==1 && total_len>=min_sec_len){
                        //Atomic section and still below the required total length of all atomic sections
			//srand(time(NULL));
/****************** Debug 14 start **********************/
//cout<<"atomic section with n ";
/***************** Debug 14 end *************************/
                        double tmp_sec_len=0;
                        if(max_sec_len==min_sec_len){
                            tmp_sec_len=min_sec_len;//if it is calculated as in else state, it returns
                            //a wrong value because of rand()
                        }else{
                            tmp_sec_len=fmod((double)rand(),(max_sec_len-min_sec_len))+min_sec_len;
                        }
                        //tmp_sec_len is a temporary variable to hold the current value of atomic section length
                        tmp_sec_len=tmp_sec_len<total_len?tmp_sec_len:total_len;
                        n=(int)(tmp_sec_len/stm_exec);
                        /****************** Debug 11 start ********************/
  //                      cout<<n<<endl;
                        /****************** Debug 11 end ********************/
                    }
                    else if(index==1 && total_len<min_sec_len){
                        //This case might not happen, but included for caution
/***************** Debug 15 start *********************/
//cout<<"convert atomic to non_atomic"<<endl;
/******************* Debug 15 end **********************/
                        index=0;
                        curr_exec=rt_exec;
                        continue;
                    }
		    else{
			cout<<"Unkown Error"<<endl;
			exit(0);
		    }
                    vec[0][0].push_back(index);
                    vec[1][0].push_back(n*curr_exec);
                    vec[3][0].push_back(n*curr_exec);
                    if(index/*if stm portion, assign a random object to it*/){
                        /***************** Debug 11 start *********************/
  //                      cout<<"New assigned objects to stm sec are ";
                        /***************** Debug 11 end *********************/
                        /********************* MULTIOBJECT-START *************************/
                        //The following line should be commented in other projects. The new steps
                        //is to allocate multiple objects for vec[2], not a single object
                        //vec[2].push_back(rand()%num_sh_obj);
                        vec[2].push_back(genObj(num_sh_obj,min_obj_per_in,num_tasks_in,task_id_in));
                        /********************* MULTIOBJECT-END *************************/
                        /********************* Debug 12 start ********************/
/*
                        for(unsigned int i=0;i<vec[2].back().size();i++){
                            cout<<vec[2].back().at(i)<<",";
                        }
                        cout<<endl;
*/
                        /********************* Debug 12 end ********************/
                        total_len-=(n*curr_exec);
                        total_len=total_len>0?total_len:0;//Just a precaution step
                    }
                    else{
                        vec[2].push_back(vector<double>(1,-1));
                        t_e-=n*curr_exec;
                    }
                    //change index and curr_exec to the other type
                    /*starthere*/
                    index=1-index;
                    curr_exec=(1-index)*rt_exec+index*stm_exec;
                    ostmfile<<vec[0][0].back()<<endl;
                    ostmfile<<vec[1][0].back()<<endl;
                    /********************* MULTIOBJECT-START *************************/
                    //The following line should be commented in other projects. The new steps
                    //is to write multiple objects of vec[2], not a single object
                    //ostmfile<<vec[2].back()<<endl;
                    for(unsigned int z=0;z<(vec[2].back()).size()-1;z++){
                        //1 is subtracted from size to add "," after each object except the last one
                        ostmfile<<(vec[2].back())[z]<<",";
                    }
                    ostmfile<<(vec[2].back()).back()<<endl;     //To print last object after the previous for loop
                    /********************* MULTIOBJECT-END *************************/
                }while(t_e>=rt_exec || total_len>min_sec_len);
                ostmfile<<"***"<<endl;      //To mark the end of recording structure for the current task
                //Print information about the current task to file
                /********************** Debug 12 start **************************/
  //              cout<<"Leaving addPortions"<<endl;
                /********************** Debug 12 end **************************/
            }
        }
    }catch(exception& e){
        cout<<"addPortions Exception: "<<e.what()<<endl;
    }
}
/****************** SH-END-2***********************/

void Task::printTaskPortions(void){
    try{
        unsigned long m_total_exec=0;     //Modified total execution of task
        int tmp_size=0;
        if(Task::vec.empty())
            cout<<"Task structure is empty"<<endl;
        else{
            for(unsigned int i=0;i<vec[0][0].size();i++){
                cout<<"part 0,"<<i<<":"<<vec[0][0][i]<<endl;
                cout<<"part 1,"<<i<<":"<<vec[3][0][i]<<endl;
                /*********************** MULTIOBJECT-START **************************/
                //The following line should be commented in other projects. The new steps
                //is to read multiple objects of vec[2], not a single object
                //cout<<"part 2,"<<i<<":"<<vec[2][i]<<endl;
                tmp_size=vec[2][i].size();
                cout<<"part 2,"<<i<<":";
                for(int j=0;j<tmp_size-1;j++){
                    //Print all objects except the last one. Otherwise, "," will be printed after it
                    cout<<vec[2][i][j]<<",";
                }
                //Print the last object
                cout<<vec[2][i][tmp_size-1]<<endl;
                /*********************** MULTIOBJECT-END **************************/
                m_total_exec+=vec[3][0][i];
            }
        }
        cout<<"Modified total exec:"<<m_total_exec<<endl;
    }catch(exception e){
        cout<<"printTaskStatistics exception: "<<e.what()<<endl;
    }
}

void Task::SetupTask(RtTester *t, ostringstream *output, chronos_aborts_t *a, double slope, double stmlope,vector<CounterBench>* c_in,double wr) {
	tester = t;
	output_ = output;
	exec_slope_ = slope;
	aborts_ = a;
        stm_exec_slope=stmlope;
        coun_obj=c_in;
        /******************* MIN_OBJ_START ********************/
        min_obj=0;              //minimum number of objects per transaction.
        num_tasks=0;
        wr_per=wr;
        /******************* MIN_OBJ_END ********************/
}

void Task::modExecTime(double per){
    int por_count=vec[3][0].size();
    for(int i=0;i<por_count;i++){
        vec[3][0][i]=vec[1][0][i]*per;
    }
}
/************************** SH-END **************************/

