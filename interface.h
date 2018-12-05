#ifndef INTERFACE_H
#define INTERFACE_H

// maximum number of components that can be defined for the simulation
#define MAXCOMPONENTS    100

//struct to store output data
typedef struct statistics{
    int EnterCustomer;
    int ExitCustomer;
    double Min_inSystem;
    double Max_inSystem;
    double Avg_inSystem;
    double Min_inQueue;
    double Max_inQueue;
    double Avg_inQueue;
    double Avg_each_Queue[MAXCOMPONENTS];
    int QueueCount[MAXCOMPONENTS];
} STAT;

////////////////////////////////////////////////
//// Make Functions for Components          ////
//// If succeed, return 1; else return 0.   ////
////////////////////////////////////////////////

//Make Exit component. 
int MakeExit(int ExitID);

//Make Generator Component
int MakeGenerator (int GenID, double IntTime, int DestID);

//Make Queue Station Component
int MakeQStation (int QSID, double service_time, int DestID);

//Make Fork Component
int MakeFork(int ForkID, int K, double probabilities[], int DestID[]);

//////////////////////////////////////////////
//Fuctions to compute and output statistics //
//////////////////////////////////////////////

//Compute statistics
STAT* GetStat(void);

//Print out statistics on screen.
void OutStat(STAT* p);

void runsim_config(double endtime);

////////////////
//Free Memory //
////////////////
void Free();

#endif