#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <pthread.h>

using namespace std ;

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
	int id ;
	struct timeval iat ;
	struct timeval service ;
} ;

extern struct params *pa ;
extern int optionT ;
extern queue<struct customerStruct *> custQ ;
extern pthread_mutex_t mutex ;
extern pthread_cond_t cv ;

void *thread_function(void *arg) ;
void *server_function(void *arg) ;
