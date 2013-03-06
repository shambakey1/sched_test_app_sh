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

/* This should really probably get broken down into two separate classes - a 
 * rt_task class and then a dummy task that inheritst from that...
 */
#ifndef RT_TASK_H
#define RT_TASK_H

#include <QObject>
#include <QString>

#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <chronos/chronos_utils.h>
#include <chronos/chronos_aborts.h>
#include "rt_test.h"
#include <vector>
#include <stm_chronos.hpp>
#include <rstm_hlp.hpp>

extern vector<CounterBench> *coun_obj;                       //Total Objects to be assigned from to different transactional parts

class RtTester;

using namespace std;

/* A generic task */
class Task
{
	public:
        unsigned long long prev_commits;        //Holds previous number of commits for current thread
        unsigned long long prev_abr_no;         //Holds previous number of abort number for current thread
        unsigned long long prev_abr_dur;        //Holds previous amount of abort duration for current thread
        vector<unsigned long long> cur_st_vec;	//Records number of success attempts for a tx, number of failure attemps, and time for failure attemps
	Task() {
		output_ = NULL;
		cpus_org_=0;
		cpus_ = 0;
		exec_time_ = 0;
		locked_usage_ = 0;		unlocked_usage_ = 0;
		period_ = 0;
		utility_ = 0;
		abort_utility_ = 0;
		utilization_ = 0;
		period_ts_.tv_sec = 0;
		period_ts_.tv_nsec = 0;
        prev_commits=0;
        prev_abr_no=0;
        prev_abr_dur=0;
	}

	/* Initialize anything needed for another run of the task */
	void SetupTask(RtTester *tester, ostringstream *output, chronos_aborts_t *a, double slope);

	void InitTask(int id, bool verbose, bool locking, bool nested) {
		task_id_ = id;
		releases_ = 0;
		verbose_ = verbose;
		locking_ = locking;
		nested_ = nested;
		aborted_ = 0;
		deadlines_met_ = 0;
		utility_accrued_ = 0;
		max_tardiness_ = 0;
	}

	void InitCpus() { cpus_ = 0; cpus_org_=0; }
	void AddCpu(int cpu) { cpus_ += pow(2, cpu);cpus_org_++; }
	int AddRelease() {
		releases_++;
		return releases_ <= max_releases_;
	}
	void AddLock(int i) { lock_list.append(i); }
	void AddDeadline() { deadlines_met_++; }
	void AddUtility() { utility_accrued_ += utility_; }
	void AddAborted() { aborted_++; }
	int CountLocks() const { return lock_list.count(); }
	void SetUsages(double usage, double cs) {
		set_unlocked_usage(exec_time_ * (usage - (usage * cs)));
		set_locked_usage(exec_time_ * usage * cs);
	}
	void SetPeriods(unsigned long p) {
		set_period(p);
		set_period_ts(p);
	}
	void SetRuntime(unsigned long runtime) {
		for(unsigned int i = 0; i < runtime; i++) {
			if(runtime < period_*i) {
				max_releases_ = i - 1;
				break;
			}
		}
	}
	unsigned long GetRunUtility() { return max_releases_ * utility_; }
	int GetRunReleases() { return max_releases_; }


	/* Getters */
	int jobs_aborted() const { return aborted_; }
	int deadlines_met() const { return deadlines_met_; }
	int utility_accrued() const { return utility_accrued_; }
	bool locking() const { return locking_; }
	bool nested() const { return nested_; }
	bool verbose() const { return verbose_; }
	unsigned long cpus() const { return cpus_; }
	unsigned long cpus_org() const {return cpus_org_;}
	unsigned long exec_time() const { return exec_time_; }
	unsigned long unlocked_usage() const { return unlocked_usage_; }
	unsigned long locked_usage() { return locked_usage_; }
	unsigned long period() const { return period_; }
	struct timespec * period_ts() { return &period_ts_; }
	struct timespec *deadline() { return &deadline_; }
	int utility() const { return utility_; }
	int abort_utility() const { return abort_utility_; }
	double utilization() const { return (double)exec_time_/period_; }
	int releases() const { return releases_; }
	int lock(int i) const { return lock_list[i]; }
	int thread_id() const { return thread_id_; }
	int task_id() const { return task_id_; }
	ostringstream *output() const { return output_; }
	char aborted() const { return *abort_flag_; }
	long max_tardiness() const { return max_tardiness_; }

	/* Setters */
	void set_exec_time(unsigned long e) { exec_time_ = e; }
	void set_utility(int u)	{ utility_ = u; }
	void set_abort_utility(int u) { abort_utility_ = u; }
	void set_period(unsigned long p) { period_ = p; }
	void set_period_ts(unsigned long p) {
		period_ts_.tv_sec = p/MILLION;
		period_ts_.tv_nsec = p*THOUSAND - period_ts_.tv_sec*MILLION*THOUSAND;
	}
	void set_unlocked_usage(unsigned long u) { unlocked_usage_ = u; }
	void set_locked_usage(unsigned long u) { locked_usage_ = u; }
	void set_thread_id(int id) { thread_id_ = id; }
	void set_start_time(struct timespec *t) { start_time_ = t; }
	void set_abort_flag() { abort_flag_ = get_abort_ptr_tid(aborts_, thread_id_); }
	void set_max_tardiness(long t) { if(t < max_tardiness_) max_tardiness_ = t; }

	void Work(long long time_us);

	void UpdateDeadline();
	void BeginRtSegment();
	void EndRtSegment();
	void SetupAbortHandler();

	/* The pthread for this task */
	int start(pthread_attr_t *attr);
	pthread_t thread;
    void addPortions(struct rt_task *task_info);  //Another form of addPortions that uses MySQL database information
	void addPortions(double,double,double,ifstream*,string,double,double,double,int,double,int,int);
	int num_sh_obj;    //number of shared objects
    vector<unsigned long long> printStatistics();   //It performs the same as stm::printStatistics but for lock-free
    struct timespec lock_start;	//Record start time to accquire locks for current critical section
    struct timespec lock_success1;       //Record start time to acctually accessing objects after locks has been acquired
    struct timespec lock_end;	//Record end time to release locks for current critical section

	void printTaskPortions(void);
    //The following line is commented out in original projects
    //void SetupTask(RtTester *tester, ostringstream *output, chronos_aborts_t *a, double slope, double stmslope,vector<CounterBench>* c_in);
    void SetupTask(RtTester *tester, ostringstream *output, chronos_aborts_t *a, double slope, double stmslope,vector<CounterBench>* c_in,double wr);
        void modExecTime(double per);
        int getMaxReleases(){
            return max_releases_;
        }
        void setDebVec(vector<vector<unsigned long long> > *in_vec,vector<vector<string> > *in_log){
            //Sets the address of the input vector to record final results of the task
            task_result=in_vec;
            log_result=in_log;
        }
        pthread_t* getCurTh(){   //Returns a pointer for the current thread
	    thread=pthread_self();
            return &thread;
        }
        vector< vector<unsigned long long> > *task_result;//Holds the data to be print for this task
		vector< vector<string> > *log_result;		//Holds log results for the CM
        vector<double> genObj_def(int,double,int,int);
        vector<double> genObj(int num_obj,double min_obj_per,int num_tasks,int task_id);        //Generates random distincit objects for the current transaction
		vector<double> genObj_old(int num_obj,double min_obj_no,int num_tasks,int task_id);	//>= ceiling(min_obj*num_obj)
		vector< vector<vector<double> > > vec;        // Will hold structure of task as the real time and stm portions
		                                    // and their corresponding length
		vector<vector <double> > por_type;             // 0 for rt, 1 for stm
		vector<vector <double> > por_length;           // time length of that portion of the task
        vector<vector <double> > por_counter;       //Maps which counter assigned to which transactional port
                                                //In this stage, it is one counter object per transaction.
                                                //Later, it can be multiple objects per transactional section
                                                //whether nested or not
        vector<vector <double> > mod_por_length;  //same as por_length but multiplied by the cpu_usage parameter
        vector<vector <double> > task_eta;      //specifies eta for each transaction. Used with FBLT
        double stm_exec_slope;
        double wr_per;  //write operation percentage
        double min_obj;              //minimum number of objects per transaction.
        int num_tasks;

	private:
	/* The actual task function */
	static void *task(void *value);
	static long timespec_subtract(struct timespec *x, struct timespec *y);

	chronos_aborts_t *aborts;
	double exec_slope_;

	/* Task properties */
	unsigned long period_;
	int utility_;
	int abort_utility_;
	struct timespec period_ts_;
	unsigned long exec_time_;

	/* Thread information */
	unsigned long cpus_org_;	//decimal count of number of cpus
	unsigned long cpus_;	//binary count of number of cpus

	int task_id_, thread_id_;

	/* Run information */
	ostringstream *output_;
	bool verbose_, locking_, nested_;
	unsigned long locked_usage_;
	unsigned long unlocked_usage_;
	int utilization_;
	int releases_;
	int max_releases_;
	struct timespec *start_time_;
	struct timespec deadline_;

	/* Run statistics */
	int aborted_;
	int deadlines_met_;
	int utility_accrued_;
	long max_tardiness_;

	/* The pointer to the abort device */
	chronos_aborts_t *aborts_;
	char *abort_flag_;

	/* Locks */
	QList<int> lock_list;
};

#endif
