#include "main.h"
#include <math.h>
#include <signal.h>

#define round(X) (((X)>= 0)?(int)((X)+0.5):(int)((X)-0.5))


char *getTimestamp(){
	char *timestamp ;
	timestamp = (char *)malloc(13) ;
	struct timeval tv1 ;
	gettimeofday(&tv1, NULL) ;
	long int diff = (tv1.tv_sec * 1000000 + tv1.tv_usec ) - (tv.tv_sec * 1000000 + tv.tv_usec) ;
	sprintf(timestamp, "%08ld.%03ld", (long int)diff/1000  , (long int)diff % 1000000 ) ;
	timestamp[12] = '\0' ;
	return timestamp ;
}

double getDiff(struct timeval t2, struct timeval t1){
	long double diff = (t2.tv_sec * 1000000 + t2.tv_usec ) - (t1.tv_sec * 1000000 + t1.tv_usec) ;
	double result = diff / 1000 ;
	return result ;
}

double getExpInterval(double dval, double rate){

	return (1 - exp( -1 *( dval*rate)) )*1000 ;

}

double getInterval(bool exp, double rate){
	// Exponential distribution selected
	if(exp){
		double dval = (double)drand48() ;
		return getExpInterval(dval, rate) ;
	}
	// Deterministic option selected
	else{
		double ms = ((double)1000)/rate ;
		return round(ms) ;
	}

}

void arrival_interrupt(int sig){
	printf("Ctrl + C caught\n") ;
}

void *thread_function(void *arg){
	struct customerStruct *customer ;

	// Unblock the SIGINT signal here
	act.sa_handler = arrival_interrupt ;
	sigaction(SIGINT, &act, NULL) ;
	pthread_sigmask(SIG_UNBLOCK, &newSet, NULL) ;

	// Insert customers into the Q
	for (int i = 0; i < pa->num ; ++i){
		// If input to be taken from the input file
		if(optionT){


		}
		// Input is from the command line
		else{
			double ita ;
			ita = getInterval(pa->exp, pa->lambda);
			usleep(ita*1000) ;
			printf("%sms: c%d arrives, inter-arrival time = %.3fms\n", getTimestamp(), i+1, ita) ;
			customer = (struct customerStruct *)malloc(sizeof(struct customerStruct)) ;
			customer->iat = ita ;
			if ( (int)custQ.size() <= pa->size){
				customer->id = i+1 ;
				pthread_mutex_lock(&mutex) ;
				custQ.push(customer) ;
				gettimeofday(&(customer->entersAt), NULL) ;
				printf("%sms: c%d enters Q1\n", getTimestamp(), i+1) ;
				pthread_cond_signal(&cv) ;
			}
			else{
				printf("%sms: c%d dropped\n", getTimestamp(), i+1) ;
				// May be add this customer in the stats q
			}

			pthread_mutex_unlock(&mutex) ;

		}


	}




	// free up the memory in the Q, with all of its members

	return 0 ;
}




