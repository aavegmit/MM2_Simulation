#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include "main.h"


using namespace std ;

void usage(){
	printf("Usage:\t ./mm2 [-lambda lambda] [-mu mu] [-s] \\ \n\t[-seed seedval] [-size sz] \\ \n\t[-n num] [-d {exp|det}] [-t tsfile]\n") ;
	exit(0) ;
}


/* Main function for mm2
 * Command line argument parsing and
 * calling respective methods
 */
int main(int argc, char **argv){
	struct params *pa ;
	pa = (struct params *)malloc(sizeof(struct params)) ;
	memset(pa->tsfile, '\0', sizeof(pa->tsfile)) ;

	struct customerStruct *customer ; 


	int optionT = 0 ;
	//default values
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
				else if (strcmp(*argv, "-s") == 0) {
					pa->oneServer = true ;
				}
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
		printf("\tseed = %d\n", pa->seedval) ;
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
		printf("\ntsfile = %s\n", pa->tsfile) ;


	// Allocate memory to pa->num number of customers
	customer = (struct customerStruct *)malloc(sizeof(struct customerStruct)*pa->num) ;


	// Thread creation and join code taken from WROX Publications book
	// Create a new thread
	pthread_t a_thread[pa->num] ;
	void *thread_result ;
	int res ;
	for (int i = 0; i < pa->num; i++){
		res = pthread_create(&a_thread[i], NULL, thread_function, (void *)i);
		if (res != 0) {
			perror("Thread creation failed");
			exit(EXIT_FAILURE);
		}
	}







	// Thread Join code taken from WROX Publications
	for (int i = 0; i < pa->num; ++i){
		printf("Waiting for thread %d to finish...\n", i);
		res = pthread_join(a_thread[i], &thread_result);
		if (res != 0) {
			perror("Thread join failed");
			exit(EXIT_FAILURE);
		}
	}
	//	printf("Thread joined, it returned %s\n", (char *)thread_result);





} // end of main function



void *thread_function(void *arg){
	int tid = (long)arg ;
	printf("Thread %d exiting\n", tid) ;
	return 0 ;
}
