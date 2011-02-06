#include "main.h"


void *server_function(void *arg){
	long sid = (long)arg ;
	struct customerStruct *customer ;

	while(1){
		pthread_mutex_lock(&mutex) ;
		while (custQ.size() == 0){
			pthread_cond_wait(&cv, &mutex) ;
		}
		// Dequeue a customer
		customer = custQ.front() ;
		custQ.pop() ;
		struct timeval now ;
		gettimeofday(&now, NULL) ;
		customer->queuingD = getDiff(now, customer->entersAt) ;
		printf("%sms: c%d leaves Q1, time in Q1 = %.3fms\n",getTimestamp(), customer->id, customer->queuingD) ;
		pthread_mutex_unlock(&mutex) ;
		

		printf("%sms: c%d begin service at s%ld\n",getTimestamp(), customer->id, sid) ;
		//  Work on the customer
		double inter ;
		inter = getInterval(pa->exp, pa->mu) ;
		usleep(inter*1000) ;
		customer->service = inter ;
		printf("%sms: c%d departs from s%ld, service time = %.3f\n",getTimestamp(), customer->id,sid, inter) ;
		printf("                time in system = %.3f\n", customer->queuingD + inter) ;

	}

	return 0 ;
}
