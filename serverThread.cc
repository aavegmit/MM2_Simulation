#include "main.h"
#include <pthread.h>
#include <signal.h>



void *server_function(void *arg){
	long sid = (long)arg ;
	struct customerStruct *customer ;


	while(1){
		pthread_mutex_lock(&mutex) ;
		while (custQ.size() == 0){
			if (shutdown){
				pthread_mutex_unlock(&mutex) ;
				stat->endSimulation = getDiffFromNow() ;
				pthread_exit(0) ;
			}
			pthread_cond_wait(&cv, &mutex) ;
		}
		if (shutdown && (custQ.size() == 0)){
			pthread_mutex_unlock(&mutex) ;
			stat->endSimulation = getDiffFromNow() ;
			pthread_exit(0) ;
		}
		// Dequeue a customer
		customer = custQ.front() ;
		++stat->customersServed ;
		custQ.pop() ;
		struct timeval now ;
		gettimeofday(&now, NULL) ;
		customer->queuingD = getDiff(now, customer->entersAt) ;
		stat->avCustQ += (custQ.size() + 1) * getDiff(now, stat->avCustQtemp) ;
		stat->avCustQtemp = now ;
		printf("%sms: c%d leaves Q1, time in Q1 = %.3fms\n",getTimestamp(), customer->id, customer->queuingD) ;
		pthread_mutex_unlock(&mutex) ;


		printf("%sms: c%d begin service at s%ld\n",getTimestamp(), customer->id, sid) ;
		//  Work on the customer
		double inter ;
		if(optionT)
			inter = trace[customer->id - 1]->service ;
		else
			inter = getInterval(pa->exp, pa->mu) ;
		usleep(inter*1000) ;
		customer->service = inter ;
		stat->serviceTime += inter ;
		gettimeofday(&(customer->departedAt), NULL) ;
		printf("%sms: c%d departs from s%ld, service time = %.3fms ",getTimestamp(), customer->id,sid, inter) ;
		printf("time in system = %.3fms\n", getDiff(customer->departedAt, customer->arrivesAt)) ;
		stat->totalTimeSpent += getDiff(customer->departedAt, customer->arrivesAt) ;

	}

	return 0 ;
}
