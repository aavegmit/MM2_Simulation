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
	sprintf(timestamp, "%08ld.%03ld", (long int)diff/1000  , (long int)diff % 1000 ) ;
	timestamp[12] = '\0' ;
	return timestamp ;
}

double getDiff(struct timeval t2, struct timeval t1){
	long double diff = (t2.tv_sec * 1000000 + t2.tv_usec ) - (t1.tv_sec * 1000000 + t1.tv_usec) ;
	double result = diff / 1000 ;
	return result ;
}

double getDiffInMicro(struct timeval t2, struct timeval t1){
	return (t2.tv_sec * 1000000 + t2.tv_usec ) - (t1.tv_sec * 1000000 + t1.tv_usec) ;
}


double getDiffFromNow(){
	struct timeval t2 ;
	gettimeofday(&t2, NULL) ;
	long double diff = (t2.tv_sec * 1000000 + t2.tv_usec ) - (tv.tv_sec * 1000000 + tv.tv_usec) ;
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

//void arrival_interrupt(int sig){
//	shutdown = 1 ;
//}

void *thread_function(void *arg){
	struct customerStruct *customer ;
	struct timeval temptv, temptv1 ;

	// Initialize stat elements
	stat->customersDropped = 0.0 ;
	stat->customersArrived = 0 ;
	stat->totalIAT = 0.0 ;
	stat->totalTimeSpent = 0.00 ;
	stat->totalTimeSpentSq = 0.00 ;
	stat->customersDropped = 0.0 ;
	stat->serviceTime = 0.0 ;
	stat->avCustQtemp = tv ;

	// Unblock the SIGINT signal here
//	act.sa_handler = arrival_interrupt ;
//	sigaction(SIGINT, &act, NULL) ;
//	pthread_sigmask(SIG_UNBLOCK, &newSet, NULL) ;

	// Insert customers into the Q
	for (int i = 0; i < pa->num ; ++i){
		if (shutdown){
			pthread_mutex_lock(&mutex) ;
			while(custQ.size() != 0)
				custQ.pop() ;
			pthread_cond_broadcast(&cv) ;
			pthread_mutex_unlock(&mutex) ;
			pthread_exit(0) ;
		}
		double ita ;
		// If input to be taken from the input file
		if (optionT)
			ita = trace[i]->iat ;
		else
			ita = getInterval(pa->exp, pa->lambda);
		// Compute the bookkeeping time
		if (i == 0){
			gettimeofday(&tv, NULL) ;
			usleep( (useconds_t)ita*1000 ) ;
		}
		else{
			gettimeofday(&temptv1, NULL) ;
			usleep( (useconds_t)(ita*1000 -  (temptv1.tv_sec * 1000000 + temptv1.tv_usec ) + (temptv.tv_sec * 1000000 + temptv.tv_usec)  ) ) ;
		}

		gettimeofday(&temptv, NULL) ;
		customer = (struct customerStruct *)malloc(sizeof(struct customerStruct)) ;
//		gettimeofday(&(customer->arrivesAt), NULL) ;
		customer->arrivesAt = temptv ;
		++stat->customersArrived ;
		stat->totalIAT += ita ;
		printf("%012.03fms: c%d arrives, inter-arrival time = %.3fms\n", getDiff(customer->arrivesAt, tv), i+1, ita) ;
//		printf("%sms: c%d arrives, inter-arrival time = %.3fms\n", getTimestamp(), i+1, ita) ;
		customer->iat = ita ;
		if ( (int)custQ.size() < pa->size){
			customer->id = i+1 ;
			pthread_mutex_lock(&mutex) ;
			custQ.push(customer) ;
			gettimeofday(&(customer->entersAt), NULL) ;
			stat->avCustQ += (custQ.size() - 1) * getDiff(customer->entersAt, stat->avCustQtemp) ;
			stat->avCustQtemp = customer->entersAt ;
			printf("%sms: c%d enters Q1\n", getTimestamp(), i+1) ;
			pthread_cond_signal(&cv) ;
			pthread_mutex_unlock(&mutex) ;
		}
		else{
			printf("%sms: c%d dropped\n", getTimestamp(), i+1) ;
			// May be add this customer in the stats q
			++stat->customersDropped ;
		}




	}
	pthread_mutex_lock(&mutex) ;
	shutdown = 1 ;
	pthread_cond_broadcast(&cv) ;
	pthread_mutex_unlock(&mutex) ;




	// free up the memory in the Q, with all of its members

	return 0 ;
}




