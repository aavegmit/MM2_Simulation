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
		pthread_mutex_unlock(&mutex) ;
		

		//  Work on the customer
		printf("Customer c%d is being taken care by s%d\n", customer->id, (int)sid) ;

	}

	printf("Server Thread %d exiting\n", (int)sid) ;
	return 0 ;
}
