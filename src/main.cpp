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

#define VERSION "1.0.8"

#include <QApplication>
#include <QFileDialog>
#include <QObject>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include "test_app_gui.h"
#include "rt_test.h"
#include <getopt.h>

#define DBHOST "localhost"
#define DB "test"
#define DBUSER "root"
#define DBPASS "hagerabd"

using namespace std;

void set_version(Ui_MainWindow *ui) {
	ui->versionLabel->setText(VERSION);
}

void print_usage() {
	cout	<< endl << "Console testing application for ChronOS"			<< endl
		<< "------------------------------------------------------ " 		<< endl
		<< "Usage:" 								<< endl 
		<< "  -s scheduler  The name of scheduler - RMA, EDF, DASA etc" 	<< endl
		<< "  -a            Abort on idle"					<< endl
		<< "  -p            Enable priority inheritance"			<< endl
		<< "  -h            Enable HUA abort handlers"				<< endl
		<< "  -d            Enable deadlock prevention"				<< endl
		<< "  -f filename   The name of the file that has the task list"	<< endl
		<< "  -b            Enable the batch mode"				<< endl
		<< "  -c cpu-usage  The cpu-usage"					<< endl
		<< "  -e end-usage  The end cpu-usage (used in batch mode)"		<< endl
		<< "  -i interval   The interval for each iteration in batch"		<< endl
		<< "  -r run-time   The total time the test needs to run"		<< endl
		<< "                This should be the hyper-period (use -z to find)"	<< endl
		<< "  -l cs-length  Enable locking, and provide critical"		<< endl
		<< "                section length"					<< endl
		<< "  -n            Enable nested locking (as opposed to sequential)"	<< endl
		<< "  -v            Enable verbose mode"				<< endl
		<< "  -o            Enable output to a log file"			<< endl
		<< "  -x            Enable log in Excel format"				<< endl
		<< "  -g            Enable log in Gnuplot format"			<< endl
		<< "  -z            Don't run the test, just print the hyper-period"	<< endl
		<< "  -j s_max per        Provide the maximum atomic section length percentage relative to WCET of each task, 0 for none"	<< endl
		<< "  -k s_min per        Provide the minimum atomic section length percentage relative to WCET of each task, 100 for the whole"	<< endl
		<< "  -t total per        Provide the total length percentage of all atomic sections relative to WCET of each task, 0 for none"	<< endl
		<< "  -u s_obj num        Provide the number of shared objects"	<< endl
		<< "  -y min_obj per      Specifies minimum number of objects per each transaction as a percantage of total number of objects "
		<< "                      If 0, or not provided, then random number of objects is assumed" << endl
		<< "  -w num_tasks        Provides number of tasks for current experiment" << endl
		<< "  -q wr_per           Percentage of write operations" << endl
		<< endl
		<< "Examples:"							 	<< endl
		<< "    $sched_test_app -s RMA -f taskset/5t_nl -c 100 -r 20"		<< endl
		<< endl
		<< " OR $sched_test_app -s EDF -f tasknet/10t_nl -c 50 -l 10 -r 20 --a -p -h -d"
		<< endl									<< endl
		<< " NOTE: -a -p -h -d can also be written as -aphd"			<< endl;
}

static void qtMessageHandler(QtMsgType type, const char *msg) {
	switch(type) {
	case QtDebugMsg:
		fprintf(stderr, "Debug: %s\n", msg);
	case QtWarningMsg:
		fprintf(stderr, "Warning: %s\n", msg);
	case QtCriticalMsg:
		fprintf(stderr, "Critical: %s\n", msg);
	case QtFatalMsg:
		fprintf(stderr, "Fatal: %s\n", msg);
		abort();
	}
}

int main(int argc, char **argv) {
	/* Definitions for slope */
	ifstream slopefile ("/usr/local/chronos/slope.conf");
	string line;
	double exec_slope;

	/* Definitions for console mode */
	QString *file, *sched;
	char *file_name = NULL, *sched_name = NULL;
	int c;
    double s_max, s_min, t_per;
    int obj_no;
    double obj_min=0;       //Specifies minimum number of objects per transaction
                            //If 0, then random number of objects per transaction is assumed (DEFAULT)
    int num_tasks=0;        //Total number of tasks per current experiment
    double wr_per=1;        //percentage of write operations. (1-wr_per) is percentage of read operations
	bool lock_flag = false, verbose_flag = false, log_flag = false,
	     no_run = false, excel_flag = false, abort_flag = false,
	     hua_flag = false, deadlock_flag = false, pi_flag = false, 
	     file_set = false, sched_set = false, cmd = false, batch_flag = false,
	     gnu_flag = false, nested_flag = false;
	int run_time = 0, cpu_usage = 0, lock_len = 0, end_usage = 0, interval = 0, main_cpu = 0;

	if(argc > 8)
		cmd = true;

	/* Handle command line arguments */
        /************************ SH_SQL_ST **************************/
        string dataset_host=DBHOST;    //Host of dataset
        string dataset_user=DBUSER;     //username to access dataset
        string dataset_pass=DBPASS;     //password to access dataset
        string dataset=DB;          //database containing dataset
        int dataset_id; //ID of specified dataset

        while(1){
            static struct option long_options[] =
              {
                {"db",required_argument,0,0},
                {"db_host",required_argument,0,0},
                {"db_user",required_argument,0,0},
                {"db_pass",required_argument,0,0},
                {"dataset_id",required_argument,0,0},
                {"sync",required_argument,0,0},
                {"checkpoint",required_argument,0,0},
                {"sh_lev",required_argument,0,0},
                {"transitive",required_argument,0,0},
                {0, 0, 0, 0}
              };
            int option_index = 0;
            c = getopt_long (argc, argv,"c:e:f:i:l:r:s:j:k:t:u:y:w:q:abdghmnopvxz",long_options, &option_index);
            if (c == -1){
                break;
            }
            else if(!strcmp(long_options[option_index].name,"db")){
                dataset=optarg;
            }
            else if(!strcmp(long_options[option_index].name,"db_host")){
                dataset_host=optarg;
            }
            else if(!strcmp(long_options[option_index].name,"db_user")){
                dataset_user=optarg;
            }
            else if(!strcmp(long_options[option_index].name,"db_pass")){
                dataset_pass=optarg;
            }
            else if(!strcmp(long_options[option_index].name,"dataset_id")){
                dataset_id=atoi(optarg);
				DATASET_ID=dataset_id;
            }
            else if(!strcmp(long_options[option_index].name,"sync")){
                sync_alg=optarg;
                sync_alg=upperStr(sync_alg);
                if(!check_sync(sync_alg)){
                	cout<<"Not a valid synchronization technique"<<endl;
                	cout<<"Please enter one of these: ECM, RCM, LCM, PNF, FBLT, OMLP, RNLP, LOCK_FREE"<<endl;
                	exit(0);
                }
                if(isSTM(sync_alg)){
                	// In case of stm synchronization
                	stm::init("Polka","invis-eager",false);
                }
                else if(!sync_alg.compare("OMLP")){
                	lock_pro_id=OMLP_PRO;
                }
                else if(!sync_alg.compare("RNLP")){
                	lock_pro_id=RNLP_PRO;
                }
            }
            else if(!strcmp(long_options[option_index].name,"checkpoint")){
				string cp_val=optarg;
				cp_val=upperStr(cp_val);
				if(!cp_val.compare(upperStr("yes")) || !cp_val.compare(upperStr("y"))){
					setCheckpoint(true);
				}
				//default value for "checkpoint" is false
			}
            else if(!strcmp(long_options[option_index].name,"transitive")){
				string transitive=optarg;
				transitive=upperStr(transitive);
				if(!transitive.compare(upperStr("yes")) || !transitive.compare(upperStr("y"))){
					setTransitiveRetry(true);
				}
				//default value for "transitive" is false
			}
            else if(!strcmp(long_options[option_index].name,"sh_lev")){
				sh_lev=atof(optarg);
			}

            switch (c) {
	    case 0:
		break;
            case 'b' :
                    batch_flag = true;
                    break;

            case 's' :
                    sched_name = optarg;
                    sched_set = true;
                    break;

            case 'f' :
                    file_name = optarg;
                    file_set = true;
                    break;

            case 'c' :
                    cpu_usage = atoi(optarg);
                    break;

            case 'e' :
                    end_usage = atoi(optarg);
                    break;

            case 'i' :
                    interval = atoi(optarg);
                    break;

            case 'l' :
                    lock_flag = true;
                    lock_len = atoi(optarg);			
                    break;

            case 'r' :
                    //run_time = atoi(optarg);
                    break;

            case 'a' :
                    abort_flag = true;
                    break;

            case 'p' :
                    pi_flag = true;
                    break;

            case 'g' :
                    gnu_flag = true;
                    break;

            case 'h' :
                    hua_flag = true;
                    break;

            case 'd' :
                    deadlock_flag = true;
                    break;

            case 'm' :
                    main_cpu = atoi(optarg);
                    break;

            case 'n' :
                    nested_flag = true;
                    break;

            case 'v' :
                    verbose_flag = true;
                    break;

            case 'o' :
                    log_flag = true;
                    break;

            case 'x' :
                    excel_flag = true;
                    break;

            case 'z' :
                    no_run = true;
                    break;

            case 'j':
                //set the maximum length percentage for any atomic section in each task
                s_max=atof(optarg);
                break;

            case 'k':
                //set the minimum length percentage for any atomic sectino in each task
                s_min=atof(optarg);
                break;

            case 't':
                //set the total length percentage for all atomic sections in each task
                t_per=atof(optarg);
                break;

            case 'u':
                //set the number of shared objects
                obj_no=atoi(optarg);
                break;

            case 'q':
                wr_per=atof(optarg);
                break;
            case 'y':
                //set the number of shared objects
                obj_min=atof(optarg);
                break;

            case 'w':
                //set number of tasks in current experiment
                num_tasks=atoi(optarg);
                break;
            case '?' :
                   if (optopt)  printf("bad short opt '%c'\n", optopt);
	           else  printf("bad long opt \"%s\"\n", optarg);
	           break;                                    
            default :
                    print_usage();
                    return 0;
            }
	}
	/* If any character was used without the '-' handle them here */
	for (int index = optind; index < argc; index++)
	{
		print_usage();
		return 0;
	}

	/* If we are lacking a taskset or scheduler, die */
	if (cmd && (file_set == false || sched_set == false))
	{
		print_usage();
		return 0;
	}

	/* FIXME: If batch mode is enabled, has the end-usage and cpu-usage given? */

	file = new QString  ((const char*) file_name);
	sched = new QString ((const char*) sched_name);

	if (slopefile.is_open()) {
		getline (slopefile,line);
		exec_slope = atof(line.c_str());
                slopefile.close();
	} else {
		cout << "Error: No slope found. Please run slope_test_app" << endl;
		return 1;
	}		

	//Configure Error Handler
	qInstallMsgHandler(qtMessageHandler);

	// Lock the memory space
	if(mlockall(MCL_CURRENT|MCL_FUTURE) != 0) {
		cout << "Error: Unable to lock the memory space" << endl;
		cout << "Are you running as root?" << endl;
		return 1;
	}

	//Configure the GUI
	if(!cmd) {
		QApplication app(argc, argv);

		Ui_MainWindow *ui = new Ui_MainWindow();
		QMainWindow *mw = new QMainWindow();
		QFileDialog *fd = new QFileDialog(mw, "Select Taskset", QDir::currentPath());
		ui->setupUi(mw);
		set_version(ui);
		QObject::connect(ui->changeTaskSetButton, SIGNAL(clicked()), fd, SLOT(exec()));

		//Configure the tester
		RtTester *tester = new RtTester(ui, fd);
		tester->set_slope(exec_slope);
        tester->setStmSlope();
		QObject::connect(fd, SIGNAL(fileSelected(const QString&)), tester, SLOT(fileSelected(const QString&)));
		QObject::connect(ui->batchBox, SIGNAL(toggled(bool)), tester, SLOT(changeText(bool)));
		QObject::connect(ui->lockingCBox, SIGNAL(toggled(bool)), tester, SLOT(onLockingDisable(bool)));
		mw->show();

		return app.exec();
	} else {
		RtTester *tester = new RtTester();
		tester->set_slope(exec_slope);
        tester->setStmSlope();
                if(s_max!=0){
                    tester->setMaxSecPer(s_max);
                }
                if(s_min!=0){
                    tester->setMinSecPer(s_min);
                }
                if(t_per!=0){
                    tester->setTotSecPer(t_per);
                }
                if(obj_no!=0){
                    tester->setNumObj(obj_no);
                }	
                tester->setMinObj(obj_min);
                if(num_tasks>0){
                    tester->setNumTasks(num_tasks);
                }
                int lcm=tester->fileSelected(dataset_host,dataset,dataset_user,dataset_pass,dataset_id,no_run,sh_lev,TRANSITIVE);
                run_time=lcm;

		if (no_run == false) {
			tester->setSchedAlgo(*sched, abort_flag, deadlock_flag, pi_flag, hua_flag);
			tester->setParamters(cpu_usage, end_usage, interval, run_time, batch_flag);
			tester->setLocking(lock_flag, nested_flag, lock_len);
			tester->setOutputFormat(verbose_flag, log_flag, excel_flag, gnu_flag);
			tester->setMain(main_cpu);
			tester->setWrPer(wr_per);       //set write percentage
			tester->startRun();
		} else {
		    cout<<run_time<<endl;
		}
		if(isSTM(sync_alg)){
			stm::shutdown(0);
		}
		return 0;
	}

}
