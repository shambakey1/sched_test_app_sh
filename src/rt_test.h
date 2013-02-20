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

#ifndef RT_TEST_H
#define RT_TEST_H

#include <QObject>
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDateTime>

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

#include <chronos/atomic_int.h>
#include "rt_sched_list.h"
#include "rt_task.h"
#include "test_app_gui.h"


/********************* SH-START **************************/
#include <stm_chronos.hpp>
#include <cmath>
/********************* SH-END **************************/
/****************** SH_SQL_ST *********************/
#include <Util.hpp>
/****************** SH_SQL_END *********************/


/* Priorities */
//#define MAIN_PRIO			51
#define TASK_CREATE_PRIO		45
#define TASK_CLEANUP_PRIO		44
#define TASK_RUN_PRIO			40
/************************* PNF-START *****************************/
#define cm "ECM"
/************************* PNF-END *******************************/
/************************** SH-START *****************************/

extern double PSY;
//extern string cm;

struct lcm_args{
    unsigned long length;    //execution time of the atomic section
    double psy;                 //threshold value used to calculate alpha
    };
/************************** SH-END *****************************/

class Task;

using namespace std;

/* Tester Class */
class RtTester : public QObject
{
	Q_OBJECT

	public:
            
            /*********************** SH-START ****************************/
        //Initialize Counter objects to be passed to tasks
        //Number of objects is specified in stm_chronos.hpp
        /************************ SH-START-2 *************************/
        vector<CounterBench> c_vec;
        double s_max;      //Maximum atomic section percentage
        double s_min;      //Minimum atomic section percentage
        double t_len;      //Total length percentage of all atomic sections
        int n_obj;      //Number of shared objects
        /************************ SH-END-2 *************************/
        /*********************** SH-END ****************************/
        /************************ MIN_OBJ_START **********************/
        double min_obj;      //specifies minimum number of objects per each transaction
                               //0 is default=>random number of objects per transaction
        int num_tasks;        //number of tasks per current experiment
        double wr_per;        //write percentage
        /************************ MIN_OBJ_END **********************/
        /******************* SH_SQL_ST *******************/
        int dataset_id;
        /******************* SH_SQL_END *******************/
	
	RtTester() {
		selector_ = new SchedSelector(TASK_RUN_PRIO);
		mw_ = NULL;
		fd_ = NULL;
                /************************ SH-START **************************/
                cpu_usage_=100;
                /*********************** SH-START-2 ************************/
                s_max=0;
                s_min=0;
                t_len=0;
                n_obj=0;
                /*********************** SH-END-2 ************************/
                /************************ SH-END **************************/
                /******************** MIN_OBJ_START ***********************/
                min_obj=0;
                num_tasks=0;
                /******************** MIN_OBJ_END ***********************/

		InitTester();
	}

	RtTester(Ui_MainWindow *window, QFileDialog *dialog) {
		selector_ = new SchedSelector(TASK_RUN_PRIO);
		mw_ = window;
		fd_ = dialog;
                /************************ SH-START **************************/
                cpu_usage_=100;
                /************************ SH-END **************************/
		QObject::connect(mw_->runButton, SIGNAL(clicked()), this,
				 SLOT(runClicked()));
		QObject::connect(mw_->clearButton, SIGNAL(clicked()),
				 mw_->textEdit, SLOT(clear()));

		/* Add Scheduling Algos to the combo box */
		for(int i = 0; i < selector_->count(); i++) {
			mw_->schedCmbBox->addItem(selector_->AlgoName(i));
		}

		InitTester();
	}

	~RtTester() {
		delete selector_;
	}

	void InitTester() {
		oss = new ostringstream(ostringstream::out);

#ifdef _SC_NPROCESSORS_ONLN
		proc_count_ = sysconf(_SC_NPROCESSORS_ONLN);
		if(proc_count_ < 2 && mw_)
			mw_->mainCPUCmbBox->setEnabled(false);
		else if(mw_)
		{
			for(int i = 0; i < proc_count_; i++)
			{
				mw_->mainCPUCmbBox->addItem(QString::number(i));
			}
		}	
#endif
		// Add the abort device
		if(init_aborts(&aborts_)) {
			cout << "Error: Unable to mmap abort device" << endl;
			cout << "Are you running as root?" << endl;
		}
	}
	/* Performance statistics functions */
	void ClearCounters() {
		sys_total_release_ = 0;
		sys_met_release_ = 0;
		sys_total_util_ = 0;
		sys_met_util_ = 0;
		sys_abort_count_ = 0;
		max_tardiness_ = 0;
	}

	/* Getters for local members*/
	chronos_mutex_t *lock(int lock) const { return lock_list[lock]; }
	int sys_total_release() const { return sys_total_release_; }
	int sys_total_util() const { return sys_total_util_; }
	int sys_met_release() const { return sys_met_release_; }
	int sys_met_util() const { return sys_met_util_; }
	int sys_abort_count() const { return sys_abort_count_; }
	long max_tardiness() const { return max_tardiness_; }
	double dsr_per() const { return (double)sys_met_release()/(double)sys_total_release(); }
	double aur_per() const { return (double)sys_met_util()/(double)sys_total_util(); }
	bool verbose() const { return verbose_; }
	bool locking() const { return locking_; }
	bool nested_locking() const { return nested_locking_; }
	bool use_hua() const { return use_hua_; }
	int main_cpu() const { return main_cpu_; }
	unsigned int run_time_s() const { return run_time_; }
	unsigned int run_time_us() const { return run_time_*MILLION; }
	unsigned long lock_time() const { return lock_time_; }
	int lock_count() const { return lock_count_; }
	double usage_abs() const { return ((double)cpu_usage_)/100; }
	int usage_per() const { return cpu_usage_; }
	double cs_len() const { return ((double)cs_len_)/100; }
	double slope() const { return slope_; }
	struct timespec * getStartTime() { return &g_start_time_; }
	struct timespec * getEndTime() { return &g_end_time_; }
	int chronos() const { return selector_->Sched() != -1; }

	/* Setters */
	void set_lock_time(unsigned long u) { lock_time_ = u; }
	void set_slope(double slope) { slope_ = slope; }

	/* Non-local Getters */
	unsigned long MainCpuMask() const { return pow(2, main_cpu_); }
	int SchedAlgo() const { return selector_->Sched(); }
	QString CurrentAlgoName() const { return selector_->CurrentAlgoName(); }
	int SchedPriority() const{ return selector_->SchedPriority(); }
	int CountTasks() const { return task_list.count(); }

	void AddCpu(int cpu) { domain_mask_ |= (int)pow(2, cpu); }
	Task *FindByTid(int tid);
	/* Console-only functions to set everything up */
	void fileSelectedEx(QString *file, bool no_run);
	void setSchedAlgo(QString sched, bool abort_flag, bool dead_flag,
			   bool pi_flag, bool hua_flag);
	void setParamters(int cpu_x, int end_x, int interval, int run_x, bool batch_flag);
	void setOutputFormat(bool verbose_flag, bool log_flag, bool excel_flag,
			     bool gnu_flag);
	void setLocking(bool lock_flag, bool nested, int cs_len);
	void setMain(int main);
	void startRun();
	/* Main stream for logging */
	ostringstream *oss;
        /************************ SH-START *******************************/
        vector<vector<vector<unsigned long long> > > total_result;
        vector<vector<vector<string> > > total_log;
        double getStmSlope() const;
        void setStmSlope();
        void printTaskResults(){
            /*
            int task_list_size=task_list.size();
            int data_size;
            cout<<"ID\tCommit\tAbr_No\tAbr_Dur\tStart\tDeadline\tEndtime\tResponsetime"<<endl;
            for(int j=0;j<task_list_size;j++){
                cout<<total_result[j][0][0]<<"\t"<<total_result[j][1][0]<<"\t"<<total_result[j][1][1]<<"\t"<<total_result[j][1][2];
                data_size=(total_result[j][2]).size();
                cout<<"\t"<<total_result[j][2][0]<<"\t"<<total_result[j][3][0]<<"\t"<<total_result[j][4][0]<<"\t"<<total_result[j][5][0]<<endl;
                for(int k=1;k<data_size;k++){
                    cout<<"\t\t\t\t"<<total_result[j][2][k]<<"\t"<<total_result[j][3][k]<<"\t"<<total_result[j][4][k]<<"\t"<<total_result[j][5][k]<<endl;
                }
            }
            */
            /***************** SH_SQL_ST ***************/
            setResults(dataset_id,total_result);
            /***************** SH_SQL_END ***************/
        }

unsigned long gcd(double m, double n) {
        double t, r;

        if (m < n) {
                t = m;
                m = n;
                n = t;
        }

        r = fmod(m,n);

        if (r == 0) {
                return (unsigned long) n;
        } else {
                return gcd(n, r);
        }
}

        
        void printTotalLog(){
            //Print log file that shows behavior of CM
            int task_list_size=task_list.size(); //Holds number of tasks
            int instance_size=0;                  //Holds number of instances in each task during experiment
            int data_size=0;            //Holds amount of data for each instance of each task
            for(int i=0;i<task_list_size;i++){
                instance_size=total_log[i].size();
                cout<<"************ Task "<<i<<" ************"<<endl;
                cout<<endl;
                for(int j=0;j<instance_size;j++){
                    data_size=total_log[i][j].size();
                    cout<<"************ instance "<<j<<" ************"<<endl;
                    cout<<endl;
                    for(int k=0;k<data_size;k++){
                        cout<<i<<","<<j<<","<<k<<":"<<total_log[i][j][k];
                    }
                    cout<<endl;
                }
            }
        }
        /*********************** SH-START-2 **************************/
        void setMaxSecPer(double); //Set maximum section length percentage
        void setMinSecPer(double); //Set minimum section length percentage
        void setTotSecPer(double); //Set total length percentage of all atomic sections
        void setNumObj(int);    //Set number of shared objects
        /*********************** SH-END-2 **************************/
        /************************** MIN_OBJ_START ***********************/
        void setMinObj(double); //Sets minimum number of objects per each transaction as a
                                //percentage of total number of objects (note that ceiling is assumed)
        double getMinOjb();     //Returns percentage of minimum number of objects per transaction
                                //relative to total number of objects
        void setNumTasks(int);  //Sets number of tasks per current experiment
        int getNumTasks();      //Returns number of tasks per current experiment
        void setWrPer(double wr){
            wr_per=wr;
        }
        double getWrPer(){
            return wr_per;
        }
        /************************** MIN_OBJ_END ***********************/
        
        /************************** PCM-N-F-START *************************/
        int getSched(){
            return selector_->Sched();
        }
        /************************** PCM-N-F-END *************************/
        /************************ SH-END *******************************/


	public slots:
	void runClicked();
	void changeText(bool value);
	/*********************** SH-START-3 ************************/
	int fileSelected(const QString &file,bool z_op);
        /*********************** SH-END-3 ************************/
        /***************** SH_SQL_ST ************************/
        //Another form of fileSelected that accesses a MySQL databse file
        int fileSelected(string data_set_host,string data_set,string user_name,string user_pass,int dataset_id,bool z_op);
        /***************** SH_SQL_END ************************/
	void onLockingDisable(bool locking);

	private:
	/* Function Declarations */
	int run();
	void initReference();
	void calcLockTime();
	void initialize();
	void extract();
	bool verify();
	void print()
	{
		if(foutput.is_open())
			foutput << oss->str();
		if(mw_) {
	        	mw_->textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
			mw_->textEdit->insertPlainText(QString(oss->str().c_str()));
		}
		else
			cout << oss->str();
		oss->str("");
	}

	/* Members */
	Ui_MainWindow *mw_;
	QFileDialog *fd_;
	SchedSelector *selector_;

	/* CPU Affinity */
	int proc_count_;
	int main_cpu_;
	unsigned long domain_mask_;

	/* CPU Usage */
        int cpu_usage_;
	int cpu_usage_end_;
	int cpu_usage_it_;

	/* Other run options */
	int run_time_;
	int cs_len_;
	int lock_count_;
	unsigned long lock_time_;
	bool locking_;
	bool nested_locking_;
	bool verbose_;
	bool excel_;
	bool gnuplot_;
	bool batch_mode_;
	bool use_hua_;
	bool log_to_file_;

	/* System slope */
	double slope_;

	/* Abort device */
	chronos_aborts_t aborts_;

	/* Run statistics */
	struct timespec g_start_time_;
	struct timespec g_end_time_;

	/* Task and lock lists */
	QList<Task *> task_list;
	QList<chronos_mutex_t *> lock_list;

	/* System level performance statistics */
	int sys_total_release_;// The total number of tasks
	int sys_met_release_;	// The number of tasks that met deadlines
	int sys_total_util_;	// The total utility of all tasks
	int sys_met_util_;	// The total utility of all tasks that met deadlines
	int sys_abort_count_;	// The number of threads aborted
	long max_tardiness_;	// The highest tardiness of any task

	ofstream foutput;
        /************************ SH-START *****************************/
        double stm_slope;                    // slope for execution of one Counter iteration
        /************************ SH-END *****************************/
    };

#endif
