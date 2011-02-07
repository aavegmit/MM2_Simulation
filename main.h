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
	long seedval ;
	int size ;
	int num ;
	bool exp ;
	char tsfile[256] ;
} ;

struct customerStruct{
	int id ;
	double iat ;
	double service ;
	double queuingD ;
	struct timeval entersAt ;
} ;

struct custTsfile{
	long iat ;
	long service ;
} ;

struct statistics{
	int customersArrived ;
	int customersServiced ;
	int customersDropped ;
};

extern struct params *pa ;
extern struct custTsfile **trace ;
extern struct statistics *stat ;
extern int optionT ;
extern queue<struct customerStruct *> custQ ;
extern pthread_mutex_t mutex ;
extern pthread_cond_t cv ;
extern timeval tv ;
extern sigset_t newSet ;
extern struct sigaction act ;
extern int shutdown;


void *thread_function(void *arg) ;
void *server_function(void *arg) ;
double getInterval(bool, double) ;
char *getTimestamp() ;
double getDiff(struct timeval, struct timeval) ;
void InitRandom(long) ;
