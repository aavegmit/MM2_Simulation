#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

struct params{
	double lambda;
	double mu;
	bool oneServer ;
	int seedval ;
	int size ;
	int num ;
	bool exp ;
	char tsfile[256] ;
} ;

struct customerStruct{
	struct timeval iat ;
	struct timeval service ;
} ;

void *thread_function(void *arg) ;
