/***************************************************************************
 *   Copyright (C) 2009-2010 Virginia Tech Real-Time Systems Lab
 *   Written by Matthew Dellinger
 *   mdelling@vt.edu
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

#include "rt_test.h"
#include <sys/resource.h>
/********************* pnf-start *******************/
#include <rstm_hlp.hpp>
#include <chronos/chronos_utils.h>
/******************* pnf-end ***********************/



#define tsprint(x) x.tv_sec << " sec + " << x.tv_nsec << " nsec"

/* Scheduling parameters
 *
 * These have to be global or meta_make_periodic_threads will segfault
 */
static struct sched_param tparam;
static pthread_attr_t attr;

/***************************** SH-START *************************************/
double PSY=0.5;

double RtTester::getStmSlope() const {
            return final_stm_slope;
}
        
void RtTester::setStmSlope(){
    //vector<unsigned long> in_proc(1,2);
    main_warmup(proc_count_);
}

/******************************** SH-START-2 ***********************************/
void RtTester::setMaxSecPer(double a){
    //Set maximum section length percentage
    s_max=a;
}
void RtTester::setMinSecPer(double a){
    //Set minimum section length percentage
    s_min=a;
}
void RtTester::setTotSecPer(double a){
    //Set total length percentage of all atomic sections
    t_len=a;
}
void RtTester::setNumObj(int a){
    //Set number of shared objects
    //Note there is only one CounterBench that can hold multiple counters
    //So, always, c_vec has size 1 or 0 (in case no shared objects at all)
    if(a>0){
        n_obj=a;
        CounterBench cb;     //Initialize CounterBench 
        cb.setMaxObjNo(a);      //Assign maximum number of shared counters
        c_vec.push_back(cb);    //This step is not really needed. It was a design fault to make vector of
                                //BenchCounters. For simplicity, deal with it
    }
}
/******************************** SH-END-2 ***********************************/
/***************************** SH-END *************************************/
/********************* MIN_OBJ_START ****************************/
void RtTester::setMinObj(double a){
    //if(a>0){//If provided or not default
        min_obj=a;
    //}
}

double RtTester::getMinOjb(){
    return min_obj;
}

void RtTester::setNumTasks(int a){
    //sets number of tasks per current experiment
    if(a>0){
        num_tasks=a;
    }
}

int RtTester::getNumTasks(){
    //return number of tasks per current experiment
    return num_tasks;
}
/********************* MIN_OBJ_END ****************************/

Task *RtTester::FindByTid(int tid) {
	for(int i = 0; i < CountTasks(); i++) {
		if(task_list[i]->thread_id() == tid)
			return task_list[i];
	}
	return NULL;
}

void RtTester::changeText(bool value) {
	if(value)
		mw_->label_5->setText("Begin usage (%)");
	else
		mw_->label_5->setText("CPU usage (%)");
}

void RtTester::onLockingDisable(bool enabled) {
	if(!enabled) {
		mw_->piCBox->setChecked(false);
		mw_->noDeadlocksCBox->setChecked(false);
	}
}

/* Calculate the time required to lock and unlock a resource */
void *getRtLockTime(void *p) {
	struct sched_param param;
	chronos_mutex_t *r = new chronos_mutex_t();
	chronos_mutex_init(r);

        param.sched_priority = TASK_RUN_PRIO;
        sched_setscheduler(0, SCHED_FIFO, &param);
	chronos_mutex_lock(r);
	chronos_mutex_unlock(r);
	chronos_mutex_destroy(r);
	delete r;
	pthread_exit(NULL);
}

/* Calculate the time required to lock and unlock a 
 * resource in a real-time task.
 */
void RtTester::calcLockTime() {
	pthread_t t1;
	struct timespec start_time, end_time;
	unsigned long time = 0, temp_time = 0;
	struct sched_param param;
	int prio;

	prio = getpriority(PRIO_PROCESS, 0);

	param.sched_priority = MAIN_PRIO;
        sched_setscheduler(0, SCHED_FIFO, &param);

	for(int i = 0; i < 100; i++) {
		clock_gettime(CLOCK_REALTIME, &start_time);
		pthread_create(&t1, NULL, getRtLockTime, NULL);
		pthread_join(t1, NULL);
		clock_gettime(CLOCK_REALTIME, &end_time);

		temp_time = subtract_ts_mo(&start_time, &end_time);
		if(temp_time > time)
			time = temp_time;
	}

	set_lock_time(time/1000);
	param.sched_priority = prio;
        sched_setscheduler(0, SCHED_OTHER, &param);
}

/**************************** SH_SQL_ST ************************************/
int RtTester::fileSelected(string data_set_host,string data_set,string user_name,string user_pass,int datasetID,bool z_op){
    //Another form of fileSelected that accesses a MySQL database file
    if(!initialized){
        initDB(data_set_host,data_set,user_name,user_pass);
    }
    int dataset_id=datasetID;
    if(!exists(dataset_id)){
        //Database does not exist
        cout<<"Database does not exist"<<endl;
        exit(0);
    }
    
    double utilization = 0;
    unsigned long lcm = 0;
    
    /* Extract tasks' information from dataset */
    vector<struct rt_task> tasks_info;     //Tasks of a dataset with their portions
    tasks_info=readTaskSet(data_set_host,data_set,user_name,user_pass,dataset_id);
    
    /* Empty task_list if it contains any previous tasks */
    while (!task_list.isEmpty()){
        delete task_list.takeFirst();
    }
    
    /* Create tasks according to extracted tasks from database */
    for(unsigned int i=0;i<tasks_info.size();i++){
        Task *t = new Task();
        //The following vectors are used to record final results from the created task
        if(!z_op){
            vector<vector<unsigned long long> > task_result_new;
            vector<unsigned long long> id_out;
            vector<unsigned long long> commit_no;
            vector<unsigned long long> abr_no;
            vector<unsigned long long> abr_dur;
            vector<unsigned long long> start_out;
            vector<unsigned long long> deadline_out;
            vector<unsigned long long> endtime_out;
            vector<unsigned long long> response_out;
            task_result_new.push_back(id_out);
            task_result_new.push_back(commit_no);
            task_result_new.push_back(abr_no);
            task_result_new.push_back(abr_dur);
            task_result_new.push_back(start_out);
            task_result_new.push_back(deadline_out);
            task_result_new.push_back(endtime_out);
            task_result_new.push_back(response_out);
            total_result.push_back(task_result_new);
            total_log.push_back(vector<vector<string> > ());
            t->SetupTask(this, oss, &aborts_, slope_,stm_slope,&c_vec,wr_per);
        }
        if(tasks_info[i].processors[0]==-1){
            //All processors can be used for current task
            for(int j = 0; j < proc_count_; j++) {
                t->AddCpu(j);
                AddCpu(j);
            }
        }
        else{
            for(unsigned int j = 0; j < tasks_info[i].processors.size(); j++){
                t->AddCpu(tasks_info[i].processors[j]);
                AddCpu(tasks_info[i].processors[j]);
            }
        }
        t->SetPeriods(tasks_info[i].period);
        t->set_exec_time(tasks_info[i].wcet);
        if(!z_op){
            t->addPortions(&tasks_info[i]);
            /********************* MIN_OBJ_END **********************/
            cout<<"***************************************"<<endl;
            t->printTaskPortions();
	    cout<<"number of cpus:"<<t->cpus()<<endl;
	    cout<<"main cpu:"<<main_cpu()<<endl;
	    cout<<"main cpu mask:"<<MainCpuMask()<<endl;
            cout<<"***************************************"<<endl;
        }
        /******************* SH-END-3 *************************/
        task_list.append(t);
    }
    if(!z_op){
        for(int i=0;i<task_list.count();i++){
            //Assign a vector for each task to put its records
            task_list[i]->setDebVec(&(total_result[i]),&(total_log[i]));
        }
    }
    /* Calculate the total cpu utilization for the taskset */
    for(int i = 0; i < task_list.count(); i++) {
            utilization += task_list[i]->utilization();
    }

    /* Calculate the hyper period for the task set */
    if(task_list.count())
            lcm = task_list[0]->period();

    for(int i = 1; i < task_list.count(); i++) {
            int GCD = gcd((double)lcm, (double)(task_list[i]->period()));
            lcm = lcm/GCD;
            lcm = lcm * task_list[i]->period();
    }

    print();
    return (int)lcm/MILLION;
}
/**************************** SH_SQL_END ************************************/

/* Read in the file */
/* FIXME: This might be the ugliest function ever written. We have stuff nested
 * seven layers deep.
 */
int RtTester::fileSelected(const QString &file,bool z_op) {
	string line;
	QString qline, error;
	double utilization = 0;
	unsigned long lcm = 0;
	int tasks = 0;

	while (!task_list.isEmpty())
		delete task_list.takeFirst();

	ifstream tsfile (file.toLatin1().data());
        /********************* SH-START-TOD*********************/
        stringstream stm_obj_no;
	stm_obj_no<<file.toLatin1().data()<<"_stm_"<<t_len<<"_"<<s_max<<"_"<<s_min<<"_"<<min_obj<<"_"<<n_obj;
        string stm_file_name=stm_obj_no.str();
        ifstream stmfile(stm_file_name.c_str());
        /********************* SH-END-TOD*********************/
        /******************* MULTIOBJECT-START *********************/
        srand(time(NULL));
        /******************* MULTIOBJECT-END *********************/
	if(tsfile.is_open()) {
		if(mw_)
			mw_->huaCBox->setEnabled(false);

		domain_mask_ = 0;

		while(!tsfile.eof()) {
			getline(tsfile, line);
			qline = QString::fromStdString(line);

			/* Comment */
			if(line[0] == '#')
				continue;
			/* Number of locks */
			else if (line[0] == 'L' && qline.length() > 2) {
				qline.remove(0,2);
				lock_count_ = qline.toInt();

				if(!mw_)
					continue;

				mw_->locksLabel->setNum(lock_count_);
				if(lock_count_ > 0) {
					mw_->lockingCBox->setChecked(true);
					mw_->lockingCBox->setEnabled(true);
					mw_->csTBox->setEnabled(true);
					mw_->piCBox->setEnabled(true);
					mw_->noDeadlocksCBox->setEnabled(true);
				} else {
					mw_->lockingCBox->setChecked(false);
					mw_->lockingCBox->setEnabled(false);
					mw_->csTBox->setEnabled(false);
					mw_->piCBox->setEnabled(false);
					mw_->piCBox->setChecked(false);
					mw_->noDeadlocksCBox->setEnabled(false);
					mw_->noDeadlocksCBox->setChecked(false);
				}
			/* Task */
			} else if (line[0] == 'T' && qline.length() > 2) {
				qline.remove(0,2);
				tasks++;
				QStringList list = qline.split('	', QString::SkipEmptyParts);
				if(list.count() < 3)
					*oss << "Error: task " << tasks 
					     << " in the task set is invalid." << endl;
				else {
					Task *t = new Task();
					//t->SetupTask(this, oss, &aborts_, slope_);
                                        /************************** SH-START-3 ********************************/
                                        //The following vectors are used to record final results from the created task
                                        if(!z_op){
                                            vector<vector<unsigned long long> > task_result_new;
                                            vector<unsigned long long> id_out;
                                            vector<unsigned long long> commit_no;
                                            vector<unsigned long long> abr_no;
                                            vector<unsigned long long> abr_dur;
                                            vector<unsigned long long> start_out;
                                            vector<unsigned long long> deadline_out;
                                            vector<unsigned long long> endtime_out;
                                            vector<unsigned long long> response_out;
                                            task_result_new.push_back(id_out);
                                            task_result_new.push_back(commit_no);
                                            task_result_new.push_back(abr_no);
                                            task_result_new.push_back(abr_dur);
                                            task_result_new.push_back(start_out);
                                            task_result_new.push_back(deadline_out);
                                            task_result_new.push_back(endtime_out);
                                            task_result_new.push_back(response_out);
                                            total_result.push_back(task_result_new);
                                            total_log.push_back(vector<vector<string> > ());
                                            t->SetupTask(this, oss, &aborts_, slope_,stm_slope,&c_vec,wr_per);
                                        }
                                        /************************** SH-END-3 ********************************/
					if(list[0] == "all") {
						for(int j = 0; j < proc_count_; j++) {
							t->AddCpu(j);
							AddCpu(j);
						}
					} else {
						QStringList cpu_list = list[0].split(',', QString::SkipEmptyParts);
						for(int j = 0; j < cpu_list.count(); j++) {
							t->AddCpu(cpu_list[j].toInt());
							AddCpu(cpu_list[j].toInt());
						}
					}
					t->SetPeriods(list[1].toULong());
					t->set_exec_time(list[2].toULong());

					if(list.count() >= 4)
						t->set_utility(list[3].toInt());
					else
						t->set_utility(0);

					/* Read in locks */
					if(list.count() >= 5 && lock_count_ > 0) {
						QStringList lock_list = list[4].split(',', QString::SkipEmptyParts);
						for(int j = 0; j < lock_list.count(); j++) {
							t->AddLock(lock_list[j].toInt());
						}
					}

					if(list.count() >= 6) {
						t->set_abort_utility(list[5].toInt());
						if(t->abort_utility() > t->utility())
							*oss << "Warning: task " << tasks 
							     << " has an abort utility greater than its normal utility."
							     << endl;
						if(mw_)
							mw_->huaCBox->setEnabled(true);
					}
                                        /******************* SH-START-3 *************************/
                                        if(!z_op){
                                            /********************* MIN_OBJ_START **********************/
                                            //The following line is changed in old projects
                                            t->addPortions(t->exec_time(),slope(),getStmSlope(),&stmfile,stm_file_name,s_max,s_min,t_len,n_obj,min_obj,num_tasks,task_list.size());
                                            /********************* MIN_OBJ_END **********************/
                                            cout<<"***************************************"<<endl;
                                            t->printTaskPortions();
                                            cout<<"***************************************"<<endl;
                                        }
                                        /******************* SH-END-3 *************************/
					task_list.append(t);
				}
			}
		}
	}
	tsfile.close();
        
        /************************ SH-START-3 *************************/
        if(!z_op){
            for(int i=0;i<task_list.count();i++){
                //Assign a vector for each task to put its records
                task_list[i]->setDebVec(&(total_result[i]),&(total_log[i]));
            }
        }
        /************************ SH-END-3 *************************/

	/* Calculate the total cpu utilization for the taskset */
	for(int i = 0; i < task_list.count(); i++) {
		utilization += task_list[i]->utilization();
	}

	/* Calculate the hyper period for the task set */
	if(task_list.count())
		lcm = task_list[0]->period();

	for(int i = 1; i < task_list.count(); i++) {
		int GCD = gcd((double)lcm, (double)(task_list[i]->period()));

		lcm = lcm/GCD;
		lcm = lcm * task_list[i]->period();
	}

	/* output taskset info to the gui */
	if(mw_) {
		mw_->nameLabel->setText(file.right(file.length() - file.lastIndexOf('/') - 1));
		mw_->tasksLabel->setNum(tasks);
		mw_->usageLabel->setNum(utilization);
		mw_->hyperPeriodLabel->setNum((int)lcm/MILLION);
	}
	/* Update the HUA box to reflect the task set */
	if(mw_ && mw_->huaCBox->isEnabled() == false)
		mw_->huaCBox->setChecked(false);

	print();
	return (int)lcm/MILLION;
}

void RtTester::setMain(int main) {
	main_cpu_ = main;
}

void RtTester::setParamters(int cpu_x, int end_x, int interval, int run_x, bool batch_flag) {
	batch_mode_ = batch_flag;
	run_time_ = run_x;
	cpu_usage_ = cpu_x;
	
	if(batch_mode_) {
		cpu_usage_end_ = end_x;
		cpu_usage_it_ = interval;
	} else {
		cpu_usage_end_ = cpu_usage_;
		cpu_usage_it_ = 1;
	}	
}

void RtTester::setLocking(bool lock_flag, bool nested, int lock_len) {
	locking_ = lock_flag;
	nested_locking_ = nested;

	if (locking_)
		cs_len_ = lock_len;
	else 
		cs_len_ = 0;
}

void RtTester::setOutputFormat(bool verbose_flag, bool log_flag, bool excel_flag,
			       bool gnu_flag) {
	verbose_ = verbose_flag;
	log_to_file_ = log_flag;
	excel_ = excel_flag;
	gnuplot_ = gnu_flag;
}

void RtTester::setSchedAlgo(QString sched, bool abort_flag, bool dead_flag,
			    bool pi_flag, bool hua_flag) {
	use_hua_ = hua_flag;
	
	selector_->setBaseSched(sched);
	selector_->setSchedOption(SCHED_FLAG_ABORT_IDLE, abort_flag);
	selector_->setSchedOption(SCHED_FLAG_HUA, hua_flag);
	selector_->setSchedOption(SCHED_FLAG_PI, pi_flag);
	selector_->setSchedOption(SCHED_FLAG_NO_DEADLOCKS, dead_flag);
}

/* Start a run*/
void RtTester::startRun() {
	if(verify()) {
		QDateTime dt = QDateTime::currentDateTime();
		QString path = QDir::currentPath() + "/" + dt.toString() + ".log";

		if(use_hua())
			calcLockTime();

		while(cpu_usage_ <= cpu_usage_end_) {
			if(!run()) {
				print();
				break;
			}
			cpu_usage_ += cpu_usage_it_;

			if(log_to_file_)
				foutput.open(path.toLatin1().data(), ios::app);
			print();
			if(foutput.is_open())
				foutput.close();

			if(cpu_usage_ <= cpu_usage_end_)
				sleep(1);
		}
	}

	if (mw_)
		*oss << endl;
	print();
}

/* GUI mode wrapper for startRUN */
void RtTester::runClicked() {
	mw_->runButton->setEnabled(false);
	extract();
	startRun();
	mw_->runButton->setEnabled(true);
}

/* Extract information from the GUI */
void RtTester::extract() {
	/* Extract format options */
	verbose_ = mw_->verboseCBox->isChecked();
	excel_ = mw_->excelCBox->isChecked();
	log_to_file_ = mw_->loggingCBox->isChecked();
	gnuplot_ = mw_->gnuCBox->isChecked();

	/* Extract run options */
	locking_ = mw_->lockingCBox->isChecked();
	batch_mode_ = mw_->batchBox->isChecked();
	use_hua_ = mw_->huaCBox->isChecked();
	cpu_usage_ = mw_->usageTBox->text().toInt();
	if(batch_mode_) {
		cpu_usage_end_ = mw_->usage2TBox->text().toInt();
		cpu_usage_it_ = mw_->iterationTBox->text().toInt();
	} else {
		cpu_usage_end_ = cpu_usage_;
		cpu_usage_it_ = 1;
	}
	run_time_ = mw_->execTBox->text().toInt();

	if(locking_)
		cs_len_ = mw_->csTBox->text().toInt();
	else
		cs_len_ = 0;

	main_cpu_ = mw_->mainCPUCmbBox->currentText().toInt();

	nested_locking_ = false;
	if(mw_->patternCmbBox->currentText() == "Nested")
		nested_locking_ = true;

	selector_->setBaseSched(mw_->schedCmbBox->currentText());
	selector_->setSchedOption(SCHED_FLAG_ABORT_IDLE, mw_->abortIdleCBox->isChecked());
	selector_->setSchedOption(SCHED_FLAG_HUA, mw_->huaCBox->isChecked());
	selector_->setSchedOption(SCHED_FLAG_PI, mw_->piCBox->isChecked());
	selector_->setSchedOption(SCHED_FLAG_NO_DEADLOCKS, mw_->noDeadlocksCBox->isChecked());
}

/* Verify the information extracted */
bool RtTester::verify() {
	bool verified = true; 

	if(task_list.count() == 0) {
		*oss << "No taskset loaded." << endl;
		verified = false;
	}

	if(cpu_usage_ <= 0 || cpu_usage_ > 10000) {
		*oss << "Invalid CPU usage." << endl;
		verified = false;
	}

	if(run_time_ <= 0 || run_time_ > 3600) {
		*oss << "Invalid execution time." << endl;
		verified = false;
	}

	if((cs_len_ <= 0 && locking_) || cs_len_ > 100) {
		*oss << "Invalid critical section length." << endl;
		verified = false;
	}

	if(batch_mode_ && cpu_usage_end_ < cpu_usage_) {
		*oss << "Batchmode selected, but end usage is less than start usage." << endl;
		verified = false;
	}

	if(batch_mode_ && cpu_usage_it_ == 0) {
		*oss << "CPU usage iteration cannot be 0." << endl;
		verified = false;
	}

	return verified;
}

/* Actually begin a run */
int RtTester::run() {
	int i, skip_run = 0;
	struct sched_param param, old_param;
	unsigned long main_mask = MainCpuMask();
	unsigned int len = sizeof(main_mask);

	/* Initialize the stat counters */
	ClearCounters();
        

        /* Declare ourself as a real time task */
	sched_getparam(0, &old_param);
        param.sched_priority = MAIN_PRIO;
        if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
                *oss << "sched_setscheduler failed, are you root?" << endl;
		return 0;
	}

	if (sched_setaffinity(0, len, (cpu_set_t *) &main_mask) < 0) {
		*oss << "sched_setaffinity failed" << endl;
		skip_run = 1;
		goto out_sched;
	}

	if(chronos() && set_scheduler(SchedAlgo(), SchedPriority(), domain_mask_)) {
		*oss << "Selection of RT scheduler failed! Is the scheduler loaded?" << endl;
		skip_run = 1;
		goto out_sched;
	}

	/* Declare and initialize the resources and transactional memory */
	if(locking_) {
		for(i = 0; i < lock_count_; i++) {
			chronos_mutex_t *r = new chronos_mutex_t();
			chronos_mutex_init(r);
			lock_list.append(r);
		}
	}

	if(verbose_) {
		*oss << "Percent of exec time to use per task: " << usage_abs() << endl;
		*oss << "Critical section length: " << cs_len() << endl;
		*oss << "CPU: " << main_cpu() << endl;
	}

	tparam.sched_priority = TASK_CREATE_PRIO;
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_setschedparam(&attr, &tparam);
	/***************** PNF_START **********************/
	//mu_init();
	/********************* PNF_END **********************/
	/* Generate time info for each task */
	for(i = 0; i < CountTasks(); i++) {
		task_list[i]->InitTask(i, verbose_, locking_, nested_locking_);
		task_list[i]->SetUsages(usage_abs(), cs_len());
                /************************* SH-START *********************************/
                task_list[i]->modExecTime(usage_abs());
                /************************** Debug 1 start ***********************/
                //task_list[i]->printTaskPortions();
                //cout<<"****************************************"<<endl;
                /************************** Debug 1 end ***********************/
                /************************* SH-END *********************************/
		task_list[i]->SetRuntime(run_time_us());
		task_list[i]->set_start_time(&g_start_time_);
		atomic_int_add(&sys_total_release_, task_list[i]->GetRunReleases());
		atomic_int_add(&sys_total_util_, task_list[i]->GetRunUtility());

		if(verbose_)
			*oss << "period: " << task_list[i]->period() 
			<< " usec \t usage: " << task_list[i]->unlocked_usage() 
			<< " unlocked + " << task_list[i]->locked_usage() 
			<< " locked usec" << endl;
	}

	for(i = 0; i < CountTasks(); i++)
		task_list[i]->start(&attr);

	clock_gettime(CLOCK_REALTIME, &g_start_time_);
	g_end_time_.tv_sec = g_start_time_.tv_sec + run_time_s();
	g_end_time_.tv_nsec = g_start_time_.tv_nsec;

	/* Sleep while the tasks are run and then print */
	for(int i = 0; i < CountTasks(); i++) {
		long tardiness;
		pthread_join(task_list[i]->thread, NULL);
		sys_met_release_ += task_list[i]->deadlines_met();
		sys_met_util_ += task_list[i]->utility_accrued();
		sys_abort_count_ += task_list[i]->jobs_aborted();
		tardiness = task_list[i]->max_tardiness();
		if(tardiness < max_tardiness_)
			max_tardiness_ = tardiness;
	}

	/* Free resources */
	while (!lock_list.isEmpty()) {
		chronos_mutex_t *r = lock_list.takeFirst();
		chronos_mutex_destroy(r);
		delete r;
	}

out_sched:
	/* Return us to a normal priority */
	sched_setscheduler(0, SCHED_OTHER, &old_param);

	/* Print the results */
	if(skip_run)
		return 0;
	else if(verbose_) {
		*oss << "set start_time: " << tsprint(g_start_time_) << endl;
		*oss << "set end_time: " << tsprint(g_end_time_) << endl;
		*oss << "Total tasks: " << sys_total_release()
		<< ", total deadlines met: " << sys_met_release()
		<< ", total possibly utility: " << sys_total_util()
		<< ", total utility accrued: " << sys_met_util()
		<< ", Total tasks aborted: " << sys_abort_count()
		<< endl;
	} else if(excel_)
		*oss << CurrentAlgoName().toStdString() << "," << usage_per()
		<< ",=" << sys_met_release() << "/" << sys_total_release() 
		<< ",=" << sys_met_util() << "/" << sys_total_util()
		<< endl;
	else if(gnuplot_)
		*oss << usage_abs() << " " << dsr_per() << " " << aur_per() << endl;
	else
		*oss << usage_abs() << "/" << CurrentAlgoName().toStdString()
		<< ": Tasks: " << sys_met_release() << "/" << sys_total_release() 
		<< ", Utility: " << sys_met_util() << "/" << sys_total_util()
		<< ", Aborted: " << sys_abort_count()
		<< ", Tardiness: " << max_tardiness() << endl;
        /********************** SH-START ****************************/
        printTaskResults();
        //cout<<"************ CM Log ************"<<endl;
//        printTotalLog();
        /********************** SH-END ****************************/
	return 1;
}
