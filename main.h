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
	double lambda;		// To hold the value of lambda, default 0.5
	double mu;		// MU, default 0.35
	bool oneServer ;	// default false, implies two servers
	long seedval ;		// To set the seed in srand
	int size ;		// The size of Queue to hold max customers
	int num ;		// Number of customers to be produced
	bool exp ;		// To choose between exp and det
	char tsfile[256] ;	// Saves the TSfile
} ;

// Structure to hold information related to one customer
struct customerStruct{
	int id ;			// Customer ID
	double iat ;			// Inter Arrival time
	double service ;		// Service time for this customer
	double queuingD ;		// Queuing delay
	struct timeval entersAt ;	// Timestamp when customer enters in Q
	struct timeval arrivesAt ;	// Timestamp when customer arrives in the arrival thread
	struct timeval departedAt ;	// Timestamp when customer departs from the system
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
extern struct statistics *stats ;
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
