#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <pthread.h>

using namespace std ;

// Structure to hold command line params
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

// Structure to hold information related to one customer
struct customerStruct{
	int id ;
	double iat ;
	double service ;
	double queuingD ;
	struct timeval entersAt ;
	struct timeval arrivesAt ;
	struct timeval departedAt ;
} ;

struct custTsfile{
	long iat ;
	long service ;
} ;


// Structure to hold the statistics
struct statistics{
	int customersArrived ;		// Customers arrived by the arriving thread
	double customersDropped ;	// Customers dropped because of queue size
	double customersServed ;	// Customers served by the servers
	double endSimulation ;		// Timestamp when the simulation ends
	double avCustQ ;		// Wighted total of time when Q is not empty	
	double totalIAT ;		// Total Inter Arrival Time
	double serviceTime ;  		// Total service time
	double totalTimeSpent;		// Total time spent in the system by a customer
	double totalTimeSpentSq;	// Total of square of time spent in the system by a customer
	struct timeval avCustQtemp;	// Timestamp when a customer entered the Q last time
	double serverBusy[2];		// Total time when Servers are busy 
};

// Extern variable declaration
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



// Function declarations
void *thread_function(void *arg) ;
void *server_function(void *arg) ;
double getInterval(bool, double) ;
char *getTimestamp() ;
double getDiff(struct timeval, struct timeval) ;
double getDiffFromNow() ;
void InitRandom(long) ;
