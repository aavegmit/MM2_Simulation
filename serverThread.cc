#include "main.h"
#include <pthread.h>
#include <signal.h>
#include <math.h>

// Server threads
void *server_function(void *arg){
	long sid = (long)arg ;
	struct customerStruct *customer ;


	while(1){
		// Acquire the lock
		pthread_mutex_lock(&mutex) ;
		while (custQ.size() == 0){
			// If shutdown flag set, then exit
			if (shutdown){
				pthread_mutex_unlock(&mutex) ;
				stats->endSimulation = getDiffFromNow() ;
				pthread_exit(0) ;
			}
			// Else wait in the Q
			pthread_cond_wait(&cv, &mutex) ;
		}
		// If shutdown flag up, then exit
		if (shutdown && (custQ.size() == 0)){
			pthread_mutex_unlock(&mutex) ;
			stats->endSimulation = getDiffFromNow() ;
			pthread_exit(0) ;
		}
		// Dequeue a customer
		customer = custQ.front() ;

		// Increment the customer served counter
		++stats->customersServed ;

		// Pop the customer out
		custQ.pop() ;
		struct timeval now ;

		// Get the current timestamp
		gettimeofday(&now, NULL) ;

		// Find the queuing delay
		customer->queuingD = getDiff(now, customer->entersAt) ;
		stats->avCustQ += (custQ.size() + 1) * getDiff(now, stats->avCustQtemp) ;
		stats->avCustQtemp = now ;
		printf("%sms: c%d leaves Q1, time in Q1 = %.3fms\n",getTimestamp(), customer->id, customer->queuingD) ;

		// Release the lock
		pthread_mutex_unlock(&mutex) ;


		printf("%sms: c%d begin service at s%ld\n",getTimestamp(), customer->id, sid) ;
		//  Work on the customer
		double inter ;
		if(optionT)
			inter = trace[customer->id - 1]->service ;
		else
			inter = getInterval(pa->exp, pa->mu) ;
		usleep(inter*1000) ;

		// populate the customer structure with service time
		customer->service = inter ;

		// Computation of service time statistics
		stats->serviceTime += inter ;
		gettimeofday(&(customer->departedAt), NULL) ;
		printf("%sms: c%d departs from s%ld, service time = %.3fms ",getTimestamp(), customer->id,sid, inter) ;
		printf("time in system = %.3fms\n", getDiff(customer->departedAt, customer->arrivesAt)) ;

		// Stat calculations
		stats->totalTimeSpent += getDiff(customer->departedAt, customer->arrivesAt) ;
		stats->totalTimeSpentSq += pow(getDiff(customer->departedAt, customer->arrivesAt) , 2) ;
		stats->serverBusy[sid] += getDiff(customer->departedAt, now) ;

	}

	return 0 ;
}
