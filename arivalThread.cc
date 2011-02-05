#include "main.h"


void *thread_function(void *arg){
	struct customerStruct *customer ;
	printf("Number of customers: %d\n", pa->num) ;
	
	// Insert customers into the Q
	for (int i = 0; i < pa->num ; ++i){
		// If input to be taken from the input file
		if(optionT){


		}
		// Input is from the command line
		else{
			// Sleep for (1 / pa->lambda)
			usleep((1/pa->lambda)*1000000) ;
			printf("c%d arrives, inter-arrival time =\n", i) ;
			customer = (struct customerStruct *)malloc(sizeof(struct customerStruct)) ;
			customer->id = i+1 ;
			pthread_mutex_lock(&mutex) ;
			custQ.push(customer) ;
			pthread_cond_signal(&cv) ;
			pthread_mutex_unlock(&mutex) ;
			
		}


	}




	// free up the memory in the Q, with all of its members

	printf("Arrival Thread exiting\n") ;
	return 0 ;
}




