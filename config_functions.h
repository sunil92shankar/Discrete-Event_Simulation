/*
 * config_functions.h
 *
 *  Created on: Oct 16, 2018
 *      Author: SUNIL
 */

#ifndef CONFIG_FUNCTIONS_H_
#define CONFIG_FUNCTIONS_H_

#define MAXCOMPONENTS 100//Upper limit on the number of components

// Data structure to hold data of components in the configuration program
struct config_master_list{
	int id;
    char ComponentType;  // GENERATOR, QUEUE_STATION, FORK, EXIT
    void *Comp;         // Pointer to information on component (Generator, Exit struct, Fork, Queue Station)
};


//Data structure to hold data of a generator component in configuration program
typedef struct local_generator loc_gen;
struct local_generator
{
	int id;//component id
	char type;//component type
	double mean_interarrTime;//interarrival time
	int destId;//destination ID
};

//data structure for a queue station component in the configuration program
typedef struct local_QS loc_QS;
struct local_QS
{
	int id;//ID
	char type;//component type
	double mean_ServTime;//mean service time
	int destId;//destination ID
};


//data structure to hold information on fork in the configuration program
typedef struct local_Fork loc_Fork;
struct local_Fork
{
	int id;//fork if
	char type;//component type
	int numComponents;//number of components the fork connects to
	double prob[1000];//probabilities of routing to a component, assuming max components forwarding from fork is 1000
	int destID[1000];//destinations of forward routing components
};

//data structure to hold information on exit component type within the configuration program
typedef struct local_Exit loc_exit;
struct local_Exit
{
	int id;//component id
	char type;//component type
};

/*
 * Function that takes in character read from a file and checks if it is a valid positive integer number
 * @param value_read, a char pointer to the string read in by the configuration program
 * returns int, representing the converted positive integer if successful, -1  otherwise
 */
int toInt(char * value_read);

/*
 * Function that takes in character pointer of a string read from a file and checks if it is a valid positive float number representing time
 * @param value_read, a char pointer to the string read in by the configuration program
 * returns a double value, representing the converted positive float if successful, -1  otherwise
 */
double toFloat_time(char * value_read);

/*
 * Function that takes in character pointer of a string read from a file and checks if it is a valid positive float number representing a probability between 0 and 1
 * @param value_read, a char pointer to the string read in by the configuration program
 * returns a double value, representing the converted positive float if successful, -1  otherwise
 */
double toFloat_prob(char * value_read);

/*
 * Function that takes in character pointer of a string read from a file and checks if it is a valid component type indicator 'G','F','X' or 'Q'
 * @param value_read, a char pointer to the string read in by the configuration program
 * returns an int value, representing that the value is a valid character representing a component, -1  otherwise
 */
int charCheck(char *value_read);

/*
 * Function to make a generator component within the configuration file
 * @param GenId is the ID if the generator to be created
 * @param IntTime is the mean interarrival time for the generator
 * @param DestID is the destination ID of the generator
 * return loc_gen*, a pointer to the generator object created, NULL otherwise
 */
loc_gen* MakeLocalGen (int GenID, double IntTime, int DestID);

/*
 * Function to make a QS component within the configuration file
 * @param QSid is the ID of the QS to be created
 * @param ServTime is the mean service time for the QS
 * @param DestID is the destination ID of the generator
 * return loc_QS*, a pointer to the QS object created, NULL otherwise
 */
loc_QS* MakeLocalQS (int QSid, double ServTime, int DestID);

/*
 * Function to make an Exit component within the configuration file
 * @param ExitId is the ID of the generator to be created
 * return loc_exit, a pointer to the Exit object created, NULL otherwise
 */
loc_exit* MakeLocalExit (int ExitID);


/*
 * Function to make a fork component within the configuration file
 * @param FId is the ID of the generator to be created
 * @param numComponents is the number of forwarding components to which fork directs customers
 * @param destID is an int pointer to an array with informtion on destination IDs
 * @param probabilities is a double pointer to an array with information on probability with which a customer is routed to a particular destination
 * return loc_fork, a pointer to the fork object created, NULL otherwise
 */
loc_Fork* MakeLocalfork (int Fid, int numComponents, int* destID, double* probabilities);

/*
 * Function to check if the validity of generator destination IDs
 * @param Gen is the generator on which checks are done
 * @param comp_list is the overall list of all components created with all relevant data
 * @param numofComponents is the number of components created overall
 * return int, 0 indicating validity and -1 otherwise
 */
int Generator_check(loc_gen Gen, struct config_master_list* comp_list, int numofComponents);

/*
 * Function to check if the validity of QS destination IDs
 * @param QS is the queueu station on which checks are done
 * @param comp_list is the overall list of all components created with all relevant data
 * @param numofComponents is the number of components created overall
 * return int, 0 indicating validity and -1 otherwise
 */
int QS_check(loc_QS QS, struct config_master_list* comp_list, int numofComponents);

/*
 * Function to check if the validity of fork destination IDs and destination probabilities
 * @param fork is the fork object on which checks are done
 * @param comp_list is the overall list of all components created with all relevant data
 * @param numofComponents is the number of components created overall
 * return int, 0 indicating validity and -1 otherwise
 */
int fork_check(loc_Fork fork, struct config_master_list* comp_list, int numofComponents);

/*
 * Function to free memory allocated to all components
 * @param configComponents[] is the array of all components generated in the the configuration program locally
 * @param numofComponents is the number of components created overall
 * return int, 0 indicating success and -1 otherwise
 */
int freeComp(struct config_master_list configComponents[], int numofComponents);

/*
 * Function to write statistics calculated to a file
 * @param outfilename is a char pointer to the file to write to
 * @param p is the pointer to the statistics object that has the statistics details
 * return int, 0 if writing is success, -1 otherwise
 */
int print_stat(char *outfilename, STAT *p, int numQS);

#endif /* CONFIG_FUNCTIONS_H_ */
