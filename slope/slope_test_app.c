/* Calculate the slope for a machine
 *
 * Compile with g++ -Wall slope_test_app.c -o slope -O0 -lpthread -lrt
 */

#include <sys/mman.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <chronos/chronos.h>
#include <chronos/chronos_utils.h>

using namespace std;

double EXEC_SLOPE = 0.006;
double MOVED = 0.002;

struct timespec deadline, endtime, period;

void evaluate_slope()
{
	clock_gettime(CLOCK_REALTIME, &deadline);
	deadline.tv_sec += 1;

	//actual task block
	burn_cpu(1000000, EXEC_SLOPE);
	clock_gettime(CLOCK_REALTIME, &endtime);

	if(endtime.tv_sec > deadline.tv_sec || (endtime.tv_sec == deadline.tv_sec
		&& endtime.tv_nsec > deadline.tv_nsec))
	{
		EXEC_SLOPE = EXEC_SLOPE + MOVED;
	}
	else
	{
		EXEC_SLOPE = EXEC_SLOPE - MOVED;
		MOVED = MOVED*2/3;
	}
}

void *task1(void *p)
{
	int i = 0;
	pthread_detach(pthread_self());
	struct sched_param param;
	param.sched_priority = 48;
	unsigned long cpu_mask = 1;
	unsigned int len = sizeof(cpu_mask);

	sched_setaffinity(0, len, (cpu_set_t *) &cpu_mask);
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

	for(i = 0; i < 50; i++) {
		evaluate_slope();
		printf("Slope is %12.11lf\n", EXEC_SLOPE);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	ofstream slopefile;
	pthread_t t1;
	struct sched_param param;
	unsigned long main_mask = 1;
	unsigned int len = sizeof(main_mask);
	bool test = true;
	char c, optstring[]= "t";

	while ((c = getopt (argc, argv, optstring)) != -1)
	{
		switch (c) {
		case 't' :
			test = false;
			printf("Running in test mode...\n");
			break;
		}
	}

	sched_setaffinity(0, len, (cpu_set_t *) &main_mask);

	period.tv_sec = 1;

	param.sched_priority = 49;
	pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

	pthread_create(&t1, NULL, task1, NULL);
	pthread_join(t1, NULL);

	if(test) {
		slopefile.open("/usr/local/chronos/slope.conf", ios::out | ios::trunc);
		if(slopefile.is_open()) {
			printf("Final slope of %f\n", EXEC_SLOPE);
			slopefile << EXEC_SLOPE << endl;
			slopefile.close();
		} else
			printf("Error writing to file... are you root?\n");
	}

	return 0;
}


