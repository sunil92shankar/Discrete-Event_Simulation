#ifndef __COMPONENTS_H_
#define __COMPONENTS_H_


////////////////////////////////////////////////////////////////////////////////////////
// EVENT TYPES:                                                                       //
//  GENERATE: Generation of a new customer                                            //
//      Parameters: Component where customer is generated                             //
//  ARRIVAL: Customer arrives at a queue station, fork, or exit component             //
//      Parameters: Pointer to arriving customer and component where customer arrives //
//  DEPARTURE: a customer departs from a queue station                                //
//      Parameters: ...                                                               //
////////////////////////////////////////////////////////////////////////////////////////

// Event types: Generate new customer, arrival at a component, departure from queue station
#define     GENERATE    0
#define     ARRIVAL     1
#define     DEPARTURE   2


////////////////////////////////////////////////////////
//a global variable to store infomation of components //
////////////////////////////////////////////////////////
struct {
	int ComponentType;
	void* Comp;     //infomation of component
} Component[MAXCOMPONENTS];


////////////////////////////
//Structures for customer //
////////////////////////////

//Linked List structure used in customer
typedef struct LinkedList {
	int QSID;
	double WaitTime;
	struct LinkedList* next;
} List;

//Struct for Customer
typedef struct customer {
	double CreationTime;
	double ExitTime;
	double ArrivalTime;		//time when customer arrives in each QS
	double WaitTime;		//wait time in each QS
	double totalWaitTime;		//total wait time in all QS
	List* QSList;	//store the QS id that customer has entered and wait time in each QS 
	struct customer* next;  //used for FIFO queue
} Customer;

/////////////////////////////////////////////////////////////////////
// A single data structure is used to handle all three event types //
/////////////////////////////////////////////////////////////////////
typedef struct EventData {
	int EventType;              // Type of event (GENERATE, ARRIVAL, DEPARTURE)
	Customer *Cust;      // Arriving or departing customer; unused for GENERATE events
	int CompID;                 // ID of component where customer created, arrives, or departs
} EventData;

////////////////////////////////////
//////Structs for Components////////
////////////////////////////////////
typedef struct Generator {
	double IntArrTime;   //interarrival time
	int DestComp;   //destination component
} Gen;

typedef struct Exit {
	int count;  //number of customers that have exited
} Exit;

//FIFO queue for Queue Station
typedef struct FIFOQ {
	Customer *first;
	Customer *last;
} FIFOQ;

typedef struct Queue_Station {
	double service_time;
	int DestComp;
	FIFOQ *q;
	int length;     //length of FIFO queue
} QS;

typedef struct Fork {
	int K;
	double* prob;     //K probabilities
	int* DestComp;     //K output ports
} Fork;


///////////////////////////////////////
////Initialize the output statistics //
///////////////////////////////////////
int EnterCustomer = 0;
int ExitCustomer = 0;
double Min_inSystem  = 1000000.0;
double Max_inSystem = -1;
double Total_inSystem = 0;
double Min_inQueue = 1000000.0;
double Max_inQueue = 0;
double Total_inQueue = 0;
double Total_each_Queue[MAXCOMPONENTS] = {0};
int QueueCount[MAXCOMPONENTS] = {0};
int QueueID = 0;	//store the max component ID of QS



/////////////////////
// Component types //
/////////////////////
#define     GENERATOR       0
#define     FORK            1
#define     QUEUE_STATION   2
#define     EXIT            3



#endif