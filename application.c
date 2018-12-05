#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "sim.h"
#include "interface.h"
#include "components.h"
// 


void Generate(EventData*);
void Arrival(EventData*);
void Departure(EventData*);
double urand();
double randexp( double);
void ChangeWaitTime(FIFOQ* q, double current_time);


//////////////////////////////////
//Make Functions for Components //
//////////////////////////////////

//Make Exit component. 
int MakeExit(int ExitID) {
    Component[ExitID].ComponentType = EXIT; //EXIT: an integer declared to represent exit component
    Exit *p = (Exit*) malloc (sizeof(Exit));
    if (p == NULL) {
        fprintf(stderr, "malloc error\n"); return 0;
        return 0;
    }
    p->count = 0;
    Component[ExitID].Comp = p;
    //printf ("Creating Exit Component, ID=%d\n", ExitID);
    return 1;
}

//Make Generator Component
int MakeGenerator (int GenID, double IntTime, int DestID) {
    Gen *p;
    EventData *d;

    // Add component to master list; Caller is responsible for handling set up errors
    Component[GenID].ComponentType = GENERATOR;
    //printf ("Creating Generator Component, ID=%d, Interarrival time=%f, Destination=%d\n", GenID, IntTime, DestID);

    // Allocate space for component, fill in parameters
    if ((p = (Gen*) malloc (sizeof(Gen))) == NULL) {
        fprintf(stderr, "malloc error\n"); return 0;
    }
    p->IntArrTime = IntTime;
    p->DestComp = DestID;
    Component[GenID].Comp = p;

    // schedule initial, first generator event for this component
    if ((d = (EventData*)malloc (sizeof(EventData))) == NULL) {
        fprintf(stderr, "malloc error\n"); return 0;
    }
    d->EventType = GENERATE;
    d->Cust = NULL;
    d->CompID = GenID;
    Schedule (IntTime, d);  // should use random draw here
    return 1;
}

//Make Queue Station Component
int MakeQStation (int QSID, double service_time, int DestID) {
    QS* newQS = (QS*) malloc (sizeof(QS));
    if (newQS == NULL) {
        fprintf(stderr, "malloc error\n"); return 0;
    }
    newQS -> service_time = service_time;
    newQS -> DestComp = DestID;
    newQS -> length = 0;
    newQS -> q = (FIFOQ*) malloc (sizeof(FIFOQ));
    newQS -> q -> first = NULL;
    newQS -> q -> last = NULL;
    Component[QSID].ComponentType = QUEUE_STATION;
    Component[QSID].Comp = newQS;
    if (QSID > QueueID) {
        QueueID = QSID;
    }
    //printf ("Creating Queue_Station Component, ID=%d, Service time=%f, Destination=%d\n", QSID, service_time, DestID);
    return 1;
}

//Make Fork Component
int MakeFork(int ForkID, int K, double probabilities[], int DestID[]) {
    Fork* newFork = (Fork*) malloc (sizeof(Fork));
    if (newFork == NULL) {
        fprintf(stderr, "malloc error\n"); return 0;
    }
    newFork -> K = K;
    newFork -> prob = probabilities;
    newFork -> DestComp = DestID;
    Component[ForkID].ComponentType = FORK;
    Component[ForkID].Comp = newFork;
    //printf ("Creating Fork Component, ID=%d, K=%f\n ", ForkID, K);
    return 1;
}

//////////////////////////////////////////////
//Fuctions to compute and output statistics //
//////////////////////////////////////////////
STAT* GetStat() {
    STAT* p = (STAT*) malloc (sizeof(STAT));
    p -> EnterCustomer = EnterCustomer;
    p -> ExitCustomer = ExitCustomer;
    p -> Min_inSystem = Min_inSystem;
    p -> Max_inSystem = Max_inSystem;
    p -> Avg_inSystem = Total_inSystem / ExitCustomer;
    p -> Min_inQueue = Min_inQueue;
    p -> Max_inQueue = Max_inQueue;
    p -> Avg_inQueue = Total_inQueue / ExitCustomer;
    memcpy(p -> QueueCount, QueueCount, sizeof(QueueCount));
    for (int i = 0; i <= QueueID; i++) {
        if (QueueCount[i] == 0) continue;
        p -> Avg_each_Queue[i] = Total_each_Queue[i] / QueueCount[i];
    }
    
    return p;
}

void OutStat(STAT* p) {
    printf("Number of Customers Entering the system: %d.\n ", p ->EnterCustomer);
    printf("Number of Customers Exiting the system: %d.\n", p-> ExitCustomer);
    printf("Min time of a customer remains in the system: %f.\n ", p->Min_inSystem);
    printf("Max time of a customer remains in the system: %f.\n ", p->Max_inSystem);
    printf("Avg time of a customer remains in the system: %f.\n ", p->Avg_inSystem);
    printf("Min time of a customer waits in queues: %f.\n ", p -> Min_inQueue);
    printf("Max time of a customer waits in queues: %f.\n ", p -> Max_inQueue);
    printf("Avg time of a customer waits in queues: %f.\n ", p-> Avg_inQueue);
    
    for (int i = 0; i <= QueueID; i++) {
        if (p->QueueCount[i] == 0) continue;
        printf("Avg time in QS %d: %f\n", i, p -> Avg_each_Queue[i]);
    }
}

// General Event Handler Procedure defined in simulation engine interface called by simulation engine
void EventHandler (void *data)
{
    struct EventData *d;

    // coerce type so the compiler knows the type of information pointed to by the parameter data.
    d = (struct EventData *) data;
    // call an event handler based on the type of event
    if (d->EventType == GENERATE) Generate (d);
    else if (d->EventType == ARRIVAL) Arrival (d);
    else if (d->EventType == DEPARTURE) Departure (d);
    else {fprintf (stderr, "Illegal event found\n"); exit(1); }
    free (d); // Release memory for event paramters
}

/////////////////////////////////////////////////////////

// event handler for generate events
void Generate (EventData *e)
{
    EventData *d;
    Customer *NewCust;
    double ts;
    Gen *pGen;  // pointer to info on generator component

    if (e->EventType != GENERATE) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
    printf ("Processing Generate event at time %f, Component=%d\n", CurrentTime(), e->CompID);

    if (Component[e->CompID].ComponentType != GENERATOR) {fprintf(stderr, "bad componenet type\n"); exit(1);}
    // Get pointer to state information for generator component
    pGen = (Gen *) Component[e->CompID].Comp;

    // Create a new customer
    if ((NewCust = (Customer*) malloc (sizeof(Customer))) == NULL) {
        fprintf(stderr, "malloc error\n"); exit(1);
    }
    NewCust->CreationTime = CurrentTime();
    NewCust->ExitTime = 0.0;    // undefined initially
    NewCust->ArrivalTime = 0.0;  // undefined initially
    NewCust->WaitTime = 0.0;    // undefined initially
    NewCust->totalWaitTime = 0.0;
    NewCust->next = NULL;       // undefined initially
    NewCust->QSList = NULL;
    EnterCustomer ++;

    // Schedule arrival event at component connected to generator
    if ((d = (EventData*) malloc (sizeof(EventData))) == NULL) {
        fprintf(stderr, "malloc error\n"); exit(1);
    }
    d->EventType = ARRIVAL;
    d->Cust = NewCust;
    d->CompID = pGen->DestComp;
    ts = CurrentTime();
    Schedule (ts, d);
    printf("Schedule Arrival at Generator at time: %f\n", ts);

    // Schedule next generation event
    if ((d = malloc (sizeof(EventData))) == NULL) {
        fprintf(stderr, "malloc error\n"); exit(1);
    }
    d->EventType = GENERATE;
    d->CompID = e->CompID;
    ts = CurrentTime() + randexp(pGen->IntArrTime);
    Schedule (ts, d);
    printf("Schedule Generate at time: %f\n", ts);
}

// event handler for arrival events
void Arrival (EventData *e)
{
    EventData *d;
    double ts;
    int Comp;                   // ID of component where arrival occurred
    Exit *pExit;         // pointer to info on exit component
    QS *pQS;        // pointer to info on QS component
    Fork *pFork;        // pointer to info on Fork component

    if (e->EventType != ARRIVAL) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
    printf ("Processing Arrival event at time %f, Component=%d\n", CurrentTime(), e->CompID);

    // processing depends on the type of component arrival occurred at
    if (Component[e->CompID].ComponentType == EXIT) {
        pExit = (Exit *) Component[e->CompID].Comp;
        (pExit->count)++;       // number of exiting customers
        e ->Cust -> ExitTime = CurrentTime();
        ExitCustomer = pExit->count;

        //Compute Statistics
        double CustomerTime = e -> Cust ->ExitTime - e -> Cust -> CreationTime;
        Total_inSystem += CustomerTime;
        //renew min time of customer remains in system
        if (CustomerTime < Min_inSystem)
            Min_inSystem = CustomerTime;
        //renew max time of customer remains in system
        if (CustomerTime > Max_inSystem)
            Max_inSystem = CustomerTime;

        if (e->Cust ->totalWaitTime > Max_inQueue)
            Max_inQueue = e ->Cust -> totalWaitTime;
        if (e-> Cust -> totalWaitTime < Min_inQueue)
            Min_inQueue = e ->Cust -> totalWaitTime;
        Total_inQueue += e -> Cust -> totalWaitTime;

        List* pList = e -> Cust -> QSList;
        while (pList) {
            //printf("QSID = %d, count = %d\n", pList->QSID, QueueCount[pList->QSID]);
            Total_each_Queue[pList->QSID] += pList -> WaitTime;
            QueueCount[pList->QSID] += 1;
            pList = pList -> next;
        }

        //if(e->Cust->QSList!=NULL){
        //    FreeList(e->Cust->QSList);
        //}
        free(e->Cust->QSList);
        free (e->Cust);         // release memory of exiting customer
    }
    else if (Component[e->CompID].ComponentType == QUEUE_STATION) {
        pQS = (QS*) Component[e -> CompID].Comp;
        e -> Cust -> ArrivalTime = CurrentTime();
        //If the QS is idle, the customer gets service and departs for the output port
        if (pQS -> length == 0) {
            if ((d = (EventData*) malloc (sizeof(EventData))) == NULL) {
                fprintf(stderr, "malloc error\n"); exit(1);
            }
            d -> EventType = DEPARTURE;
            d -> Cust = e -> Cust;
            d -> CompID = e -> CompID;
            pQS -> q -> first = e ->Cust;
            pQS -> q -> last = e -> Cust;
            pQS -> q -> last -> next = NULL;
            pQS -> length ++;   //the first customer in queue is the one being served
            ts = CurrentTime() + randexp(pQS -> service_time);
            Schedule(ts, d);
            printf("Schedule Departure at time: %f\n", ts);
        }

        //Else, the customer is put into FIFO queue.
        else {
            pQS -> q -> last -> next = e -> Cust;
            pQS -> q -> last = e -> Cust;
            pQS -> q -> last -> next = NULL;
            pQS -> length ++;
            printf("Enqueue. Current queue length: %d.\n", pQS->length);
        }
    }
    else if (Component[e->CompID].ComponentType == FORK) {
        pFork = (Fork*) Component[e->CompID].Comp;
        //Initialize new event
        if ((d = (EventData*) malloc (sizeof(EventData))) == NULL) {
            fprintf(stderr, "malloc error\n"); exit(1);
        }
        d -> EventType = ARRIVAL;
        d -> Cust = e -> Cust;
        //Decide the output port of the customer
        double *SumProb = (double*) malloc ((pFork -> K + 1) * sizeof(double));
        SumProb[0] = 0;
        for (int i = 0; i < pFork -> K; i++) {
            SumProb[i + 1] = SumProb[i] +  pFork ->prob[i];
            //printf("SumProb %d = %f\n", i+1, SumProb[i+1]);
        }
        double r = urand();
        for (int i = 0; i < pFork -> K; i++) {
            if (r >= SumProb[i] && r < SumProb[i + 1]) {
                d -> CompID = pFork -> DestComp[i];
            }
        }
        free(SumProb);
        ts = CurrentTime();
        Schedule(ts, d);
        printf("Schedule Arrival at Fork at time: %f. DestID = %d\n", ts, d->CompID);

    }
    else {
        fprintf (stderr, "Bad component type for arrival event\n");
        //printf("ComponentType: %d\n", Component[e->CompID].ComponentType);
        exit(1);
    }
}

//event handler for departure events. Note that departure only happens in QS.
void Departure (EventData *e) {
    EventData *d;
    double ts;
    QS* pQS;

    if (e->EventType != DEPARTURE) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
    printf ("Processing Departure event at time %f, Component=%d\n", CurrentTime(), e->CompID);

    // processing depends on the type of component arrival occurred at
    if (Component[e->CompID].ComponentType != QUEUE_STATION) {
        fprintf (stderr, "Bad component type for departure event\n"); exit(1);
    }

    if ((d = (EventData*) malloc (sizeof(EventData))) == NULL) {
        fprintf(stderr, "malloc error\n"); exit(1);
    }

    pQS = (QS *)Component[e->CompID].Comp;

    //arrange arrival for the first customer in queue
    //printf("Length: %d\n", pQS -> length);
    d -> EventType = ARRIVAL;
    d -> Cust = pQS ->q -> first;
    if (pQS -> length == 1) {
        pQS -> q -> first = NULL;
        pQS -> q -> last = NULL;
    }
    else {
        pQS -> q -> first = pQS -> q -> first -> next;
    }
    pQS -> length --;
    d ->CompID = pQS -> DestComp;
    ts = CurrentTime();
    ChangeWaitTime(pQS-> q, CurrentTime());   //renew the wait time of customers in queue
    d -> Cust ->totalWaitTime += d -> Cust -> WaitTime;

    //put current QS id and wait time in this QS into customer
    List* new_pList;
    if ((new_pList = (List*) malloc (sizeof(List))) == NULL) {
        fprintf(stderr, "malloc error\n"); exit(1);
    }
    new_pList-> QSID = e->CompID;
    new_pList-> WaitTime = d -> Cust -> WaitTime;
    new_pList-> next = NULL;
    //printf("new QSID = %d\n", new_pList->QSID);

    List* pList = d -> Cust -> QSList;
    if (pList == NULL)
        d -> Cust -> QSList = new_pList;
    else {
        while ((pList -> next) != NULL) {
            pList = pList -> next;
        }
        pList -> next = new_pList;
    }

    Schedule(ts, d);
    printf("Schedule Arrival at QS %d at time: %f. FIFO Queue Length: %d.\n", e->CompID, ts, pQS -> length);

    //Schedule next departure
    //if the FIFO queue is empty
    if (pQS -> length == 0) {
        return;
    }

    //else, schedule departure for the current first customer
    if ((d = (EventData*) malloc (sizeof(EventData))) == NULL) {
        fprintf(stderr, "malloc error\n"); exit(1);
    }
    d -> EventType = DEPARTURE;
    d -> Cust = pQS -> q -> first;
    d -> CompID = e -> CompID;
    ts = CurrentTime() + randexp(pQS -> service_time);
    Schedule(ts, d);
    printf("Schedule Departure at time: %f. CompID:%d\n", ts, d->CompID);
}

//When a customer is leaving QS, change the wait time of customers waiting in the FIIFO queue.
void ChangeWaitTime(FIFOQ* q, double current_time) {
    if (q -> first == NULL || q -> last == NULL) {
        return;
    }
    Customer* p = q -> first;
    while (p) {
        p -> WaitTime = current_time - p-> ArrivalTime;
        p = p -> next;
    }
}

////////////////////////////////////////////
//////////Maths Functions///////////////////
////////////////////////////////////////////
double urand() {
    int n = rand();
    while (n == RAND_MAX)
        n = rand();
    return (double)n / RAND_MAX;
}

double randexp( double U ) {
    return -U * (log(1.0 - urand()));
}

////To call RunSim() from the configuration part
void runsim_config(double endtime) {
    RunSim(endtime);
}

// Free Memory
void Free(){
    //free components
    for(int i = 0; i < MAXCOMPONENTS; i++){
        if(Component[i].Comp == NULL) break;

        //free customers remains in queue station
        if(Component[i].ComponentType == QUEUE_STATION){
            FIFOQ* q = ((QS*)Component[i].Comp) -> q;
            if(q -> first != NULL){
                Customer* Cust = q -> first;
                Customer* temp;
                while(Cust){
                        temp = Cust;
                        Cust = Cust -> next;
                        //FreeList(temp -> QSList);
                        free(temp -> QSList);
                        free(temp);                    
                }    
            }
            free(q);
        }

        free(Component[i].Comp);
    }
    //printf("End Free\n");
}
