#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "main.h"
#include <queue>
#include <signal.h>
#include <math.h>

using namespace std ;

// Interrupt handler when SIGINT is caught
void arrival_interrupt(int sig){
	shutdown = 1 ;
	while(custQ.size() != 0)
		custQ.pop() ;
}

// The correct usage of the command describing all
// the command line arguments
void usage(){
	printf("Usage:\t ./mm2 [-lambda lambda] [-mu mu] [-s] \\ \n\t[-seed seedval] [-size sz] \\ \n\t[-n num] [-d {exp|det}] [-t tsfile]\n") ;
	exit(0) ;
}



// Global variables
struct params *pa ;
struct statistics *stats ;
struct custTsfile **trace ;
int optionT ;
queue<struct customerStruct *> custQ ;
pthread_mutex_t mutex ;
pthread_cond_t cv ;
struct timeval tv ;
sigset_t newSet ;
struct sigaction act ;
int shutdown = 0 ;



/* 
 *  Main function for mm2
 * Command line argument parsing and
 * calling respective methods
 */
int main(int argc, char **argv){
	pa = (struct params *)malloc(sizeof(struct params)) ;
	memset(pa->tsfile, '\0', sizeof(pa->tsfile)) ;

	stats = (struct statistics *)malloc(sizeof(struct statistics)) ;


	// Handling the signal
	sigemptyset(&newSet) ;
	sigaddset(&newSet, SIGINT) ;
	pthread_sigmask(SIG_BLOCK, &newSet, NULL) ;

	// default values for the params
	optionT = 0 ;
	pa->lambda = 0.5 ;
	pa->mu = 0.35 ;
	pa->oneServer = false ;
	pa->seedval = 0 ;
	pa->size = 5 ;
	pa->num = 20 ;
	pa->exp = true ;

	// Parsing the command line
	if (argc > 1){
		argv++ ;

		for (int i = 0 ; i < argc-1 ; i++, argv++) {
			if (*argv[0] == '-') {
				// If command line option is lambda
				if (strcmp(*argv, "-lambda") == 0) {
					++i, argv++; 
					if (i >= (argc-1)) {
						usage() ;
					}
					if (!atof(*argv) || atof(*argv) < 0 ){
						printf("Bad Lambda value\n") ;
						exit(0) ;
					}
					pa->lambda = atof(*argv) ;
				// If command line option is mu	
				} else if (strcmp(*argv, "-mu") == 0) {
					++i, argv++; 
					if (i >= (argc-1)) {
						usage() ;
					}
					if (!atof(*argv) || atof(*argv) < 0 ){
						printf("Bad mu value\n") ;
						exit(0) ;
					}
					pa->mu = atof(*argv) ;

				}
				// If command line option is s, hence one server
				else if (strcmp(*argv, "-s") == 0) {
					pa->oneServer = true ;
				}
				// If command line option is Seed, sets the srand seed
				else if (strcmp(*argv, "-seed") == 0) {
					++i, argv++; 
					if (i >= (argc-1)) {
						usage() ;
					}
					if (!atof(*argv) || atof(*argv) < 0 ){
						printf("Bad seed value\n") ;
						exit(0) ;
					}
					pa->seedval = atof(*argv) ;
				}
				// Number of customers
				else if (strcmp(*argv, "-n") == 0) {
					++i, argv++; 
					if (i >= (argc-1)) {
						usage() ;
					}
					if (!atof(*argv) || atof(*argv) < 0 ){
						printf("Bad num value\n") ;
						exit(0) ;
					}
					pa->num = atof(*argv) ;
				}
				// The max size of Q
				else if (strcmp(*argv, "-size") == 0) {
					++i, argv++; 
					if (i >= (argc-1)) {
						usage() ;
					}
					if (!atof(*argv) || atof(*argv) < 0 ){
						printf("Bad size value\n") ;
						exit(0) ;
					}
					pa->size = atof(*argv) ;
				}
				// To chosse if exp or det 
				else if (strcmp(*argv, "-d") == 0) {
					++i, argv++; 
					if (i >= (argc-1)) {
						usage() ;
					}
					if(strcmp(*argv, "exp") == 0){
						pa->exp = true ;
					} 
					else if(strcmp(*argv, "det") == 0){
						pa->exp = false ;
					}
					else
						usage() ;
				}
				// Tsfile file name
				else if (strcmp(*argv, "-t") == 0) {
					optionT = 1 ;
					++i, argv++; 
					if (i >= (argc-1)) {
						usage() ;
					}
					strncpy(pa->tsfile, *argv, sizeof(pa->tsfile)) ;
				}
				else
					usage() ;
			} 
			else {
				usage() ;
			}
		}
	}

	//Display the params
	printf("Parameters:\n") ;
	if(!optionT){
		printf("\tlambda = %f\n", pa->lambda) ;
		printf("\tmu = %f\n", pa->mu) ;
	}
	if(pa->oneServer){
		printf("\tsystem = M/M/1\n") ;
	}
	else{
		printf("\tsystem = M/M/2\n") ;
	}
	if (!optionT)
		printf("\tseed = %ld\n", pa->seedval) ;
	printf("\tsize = %d\n", pa->size) ;
	printf("\tnumber = %d\n", pa->num) ;
	if(!optionT){
		if(pa->exp){
			printf("\tdistribution = exp\n") ;
		}
		else{
			printf("\tdistribution = det\n") ;
		}
	}
	else
		printf("\ttsfile = %s\n", pa->tsfile) ;



	// Init random seed
	if (optionT){
		FILE *fp ;
		int tnum;
		long tnum1 ;
		if( (fp = fopen(pa->tsfile, "r")) == NULL){
			printf("File open unsuccessfull\n") ;
			exit(0) ;
		}
		if(!fscanf (fp, "%d", &tnum)){
			printf("Error in Trace file\n") ;
			exit(0) ;
		}	
		pa->num = tnum ;
		trace = (struct custTsfile **)malloc(tnum) ;
		// Allocate num amount of memory to trace structures
		for (int i = 0; i < tnum; ++i){
			trace[i] = (struct custTsfile *)malloc(sizeof(struct custTsfile)) ;
			if(!fscanf (fp, "%ld", &tnum1)){
				printf("Error in Trace file\n") ;
				exit(0) ;
			}	
			trace[i]->iat = tnum1 ;
			if(!fscanf (fp, "%ld", &tnum1)){
				printf("Error in Trace file\n") ;
				exit(0) ;
			}	
			trace[i]->service = tnum1 ;
		}
	}
	else{
		// If optionT is chosen, then set the seed in srand
		InitRandom(pa->seedval) ;
	}

	printf("00000000.000ms: emulation begins\n") ;


	// Thread mutex initailization
	int mres ;
	mres = pthread_mutex_init(&mutex, NULL);
	if (mres != 0) {
		perror("Mutex initialization failed");
		exit(EXIT_FAILURE);
	}

	// Thread CV initialization
	int cres ;
	cres = pthread_cond_init(&cv, NULL) ;
	if (cres != 0) {
		perror("CV initialization failed");
		exit(EXIT_FAILURE);
	}


	// Thread creation and join code taken from WROX Publications book
	// Create a new thread
	pthread_t a_thread ;
	int res ;
	res = pthread_create(&a_thread, NULL, thread_function, (void *)NULL);
	if (res != 0) {
		perror("Thread creation failed");
		exit(EXIT_FAILURE);
	}



	// Create server thread(s)
	int numServer = 1 ;
	if (!pa->oneServer)
		++numServer ;
	pthread_t s_thread[2] ;
	void *sthread_result ;
	for(int j = 0; j < numServer; j++){
		res = pthread_create(&s_thread[j], NULL, server_function, (void *)j);
		if (res != 0) {
			perror("Thread creation failed");
			exit(EXIT_FAILURE);
		}
	}


	// Unblock the SIGINT signal here
	act.sa_handler = arrival_interrupt ;
	sigaction(SIGINT, &act, NULL) ;
	pthread_sigmask(SIG_UNBLOCK, &newSet, NULL) ;

//	 Thread Join code taken from WROX Publications
//		res = pthread_join(a_thread, &thread_result);
//		if (res != 0) {
//			perror("Thread join failed");
//			exit(EXIT_FAILURE);
//		}



	// Wait for the server thread(s) to join
	for(int j = 0; j < numServer; j++){
		res = pthread_join(s_thread[j], &sthread_result);
		if (res != 0) {
			perror("Thread join failed");
			exit(EXIT_FAILURE);
		}
	}



	//	printf("Thread joined, it returned %s\n", (char *)thread_result);

	// Printf the statistics after server threads are finished
	printf("\n\nStatistics:\n") ;
	printf("\taverage inter-arrival time = %012.3fms\n", (stats->totalIAT /  stats->customersArrived) ) ;
	printf("\taverage service time = %012.3fms\n\n", (stats->serviceTime / stats->customersServed) ) ;

	printf("\taverage number of customers in Q1 = %015.6f\n", (stats->avCustQ / stats->endSimulation ));
	printf("\taverage number of customers in S1 = %.6f\n", (stats->serverBusy[0] / stats->endSimulation));
	printf("\taverage number of customers in S2 = %.6f\n\n", (stats->serverBusy[1] / stats->endSimulation));

	printf("\taverage time spent in system = %012.3fms\n\n", (stats->totalTimeSpent / stats->customersServed));

	printf("\tstandard deviation for time spent in system = %012.3fms\n\n", sqrt((stats->totalTimeSpentSq / stats->customersServed)- pow( (stats->totalTimeSpent / stats->customersServed),2)   )) ;

	printf("\tCustomer drop probbility = %.2f\n", (stats->customersDropped / stats->customersArrived) ) ;
			


} // end of main function


// Set seed for random function
void InitRandom(long l_seed)
{
	// If no seedval is given
	if (l_seed == 0L) {
		time_t localtime=(time_t)0;

		time(&localtime);
		srand48((long)localtime);
	} else {
		srand48(l_seed);
	}
}


