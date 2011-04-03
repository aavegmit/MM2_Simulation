#include "main.h"
#include <math.h>
#include <signal.h>

#define round(X) (((X)>= 0)?(int)((X)+0.5):(int)((X)-0.5))


// Get the time difference from start simulation in printable format
char *getTimestamp(){
//	char *timestamp ;
//	timestamp = (char *)malloc(13) ;
	memset(timestamp1, '\0', 13) ;
	struct timeval tv1 ;
	gettimeofday(&tv1, NULL) ;
	long int diff = (tv1.tv_sec * 1000000 + tv1.tv_usec ) - (tv.tv_sec * 1000000 + tv.tv_usec) ;
	sprintf(timestamp1, "%08ld.%03ld", (long int)diff/1000  , (long int)diff % 1000 ) ;
	timestamp1[12] = '\0' ;
	return timestamp1 ;
}

// Get time difference in milliseconds between two timestamps
double getDiff(struct timeval t2, struct timeval t1){
	long double diff = (t2.tv_sec * 1000000 + t2.tv_usec ) - (t1.tv_sec * 1000000 + t1.tv_usec) ;
	double result = diff / 1000 ;
	return result ;
}

// Get time difference in microseconds
double getDiffInMicro(struct timeval t2, struct timeval t1){
	return (t2.tv_sec * 1000000 + t2.tv_usec ) - (t1.tv_sec * 1000000 + t1.tv_usec) ;
}

// Get the time difference from now and start simulation in milliseconds
double getDiffFromNow(){
	struct timeval t2 ;
	gettimeofday(&t2, NULL) ;
	long double diff = (t2.tv_sec * 1000000 + t2.tv_usec ) - (tv.tv_sec * 1000000 + tv.tv_usec) ;
	double result = diff / 1000 ;
	return result ;
}

// Random value according to poision distribution
double getExpInterval(double dval, double rate){

	rate *= -1 ;
	return (log(1-dval) / (rate) ) * 1000 ;
//	return (1 - exp( -1 *( dval*rate)) )*1000 ;
	//	return (1 - exp( -1 *( dval*rate*1000)) ) ;

}

// Get interval based on det or exp approach
double getInterval(bool exp, double rate){
	double retVal ;
	// Exponential distribution selected
	if(exp){
		double dval = (double)drand48() ;
		retVal =  getExpInterval(dval, rate) ;
	}
	// Deterministic option selected
	else{
		double ms = ((double)1000)/rate ;
		retVal = round(ms) ;
	}
	if (retVal < 1)
		return 1 ;
	else if(retVal > 10000)
		return 10000 ;
	else
		return retVal ;

}

//void arrival_interrupt(int sig){
//	shutdown = 1 ;
//}

// The arrival thread, produces customers and puts them in Q
void *thread_function(void *arg){
	struct customerStruct *customer ;
	struct timeval temptv, temptv1 ;



	int jdFlag  = 0 ;


//	// Initialize stat elements
//	stats->customersDropped = 0.0 ;
//	stats->customersArrived = 0 ;
//	stats->totalIAT = 0.0 ;
//	stats->totalTimeSpent = 0.00 ;
//	stats->totalTimeSpentSq = 0.00 ;
//	stats->customersDropped = 0.0 ;
//	stats->serviceTime = 0.0 ;
//	stats->avCustQtemp = tv ;

	// Unblock the SIGINT signal here
	//	act.sa_handler = arrival_interrupt ;
	//	sigaction(SIGINT, &act, NULL) ;
	//	pthread_sigmask(SIG_UNBLOCK, &newSet, NULL) ;

	// Insert customers into the Q
	for (int i = 0; i < pa->num ; ++i){
		// If shutdown flag is up, then exit accordingly
		if (shutdown){
			pthread_mutex_lock(&mutex) ;
			while(custQ.size() != 0)
				custQ.pop() ;
			pthread_cond_broadcast(&cv) ;
			pthread_mutex_unlock(&mutex) ;
			pthread_exit(0) ;
		}
		double ita, serv ;
		// If input to be taken from the input file
		if (optionT){
//			ita = trace[i]->iat ;
			ita = traceIat[i] ;
			serv = traceSer[i] ;
		}
		else{
			ita = getInterval(pa->exp, pa->lambda);
			serv = getInterval(pa->exp, pa->mu);
		}

		// Compute the bookkeeping time
		if (i == 0){
			gettimeofday(&tv, NULL) ;
			if (!jdFlag)
				temptv1 = tv ;
			stats->avCustQtemp = tv ;
			usleep( ita*1000 ) ;
		}
		else{
			gettimeofday(&temptv1, NULL) ;
			long int actualIat = ita*1000 -  (temptv1.tv_sec * 1000000 + temptv1.tv_usec ) + (temptv.tv_sec * 1000000 + temptv.tv_usec)  ;
			if (actualIat > 0){
				// Sleep for ita time minus the bookkeeping time
				usleep( (useconds_t) actualIat  ) ;
			}
			else{
				usleep(0) ;
			}
		}

		// To compute the bookkeeping time
		gettimeofday(&temptv, NULL) ;

		if (!jdFlag)
		// To find the actual inter arrival time
			ita = getDiff(temptv, temptv1) ;

		// Create a custoemr object
		customer = (struct customerStruct *)malloc(sizeof(struct customerStruct)) ;

		// Populate the customer arrival time
		customer->arrivesAt = temptv ;

		// Increment the customers arrived statistics
		++stats->customersArrived ;

		// Increment the Total inter arrival time for final statistics
		stats->totalIAT += ita ;
		if(jdFlag)
			printf("%012.03fms: c%d arrives, inter-arrival time = %.3fms\n", getDiff(customer->arrivesAt, tv), i+1, ita) ;
		else
			printf("%sms: c%d arrives, inter-arrival time = %.3fms\n", getTimestamp(), i+1, ita) ;
		customer->iat = ita ;
		customer->service = serv ;
		if ( (int)custQ.size() < pa->size){
			// Set the customer ID
			customer->id = i+1 ;

			// Acquire the lock
			pthread_mutex_lock(&mutex) ;

			// Push the customer in Q
			custQ.push(customer) ;

			// Find the current timestamp
			gettimeofday(&(customer->entersAt), NULL) ;
			stats->avCustQ += (custQ.size() - 1) * getDiff(customer->entersAt, stats->avCustQtemp) ;
			stats->avCustQtemp = customer->entersAt ;
			printf("%sms: c%d enters Q1\n", getTimestamp(), i+1) ;

			// Signal a server to wake up
			pthread_cond_broadcast(&cv) ;

			// Release the lock
			pthread_mutex_unlock(&mutex) ;
		}
		else{
			// Drop the customer since the Q is full
			printf("%sms: c%d dropped\n", getTimestamp(), i+1) ;

			// Increment the customers dropped statistics 
			++stats->customersDropped ;
		}




	}

	// Acquire the lock
	pthread_mutex_lock(&mutex) ;
	shutdown = 1 ;
	// Broadcast all the servers to wake up 
	pthread_cond_broadcast(&cv) ;

	// Release the lock
	pthread_mutex_unlock(&mutex) ;


	return 0 ;
}




