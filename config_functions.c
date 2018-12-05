/*
 * config_functions.c
 *
 *  Created on: Oct 17, 2018
 *      Author: SUNIL
 */
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<ctype.h>
#include"interface.h"
#include"config_functions.h"

/*
 * Function to check that a value read from the configuration file is a valid int, convert it to int and return it to the calling function
 */
int toInt(char * value_read)
{
	if(strlen(value_read)>0)//check that the value read  froom file is not empty
	{
		int check=0;//variable to check for validity

		//check if all characters are digits
		for(int i=0;i<strlen(value_read);i++)//https://www.gnu.org/software/libc/manual/html_node/String-Length.html
		{
				if(!isdigit(value_read[i]))
				{
					check=-1;
				}
		}

		//return integer if characters read previously are all digits, -1 otherwise
		if(check==0)
			return atoi(value_read);
		else
			{
				printf("Value read not positive integer\n");
				return -1;//Assuming Component IDs cannot be negative
			}
	}
	else
	{
		printf("No value read\n");
		return -1;
	}
}

/*
 * Function to check that a value read from file is a valid float vaue representing time
 */
double toFloat_time(char * value_read)
{
	if(strlen(value_read)>0)//ensure some value was read
	{
		int check=0;//variable to record invalidity
		int decCount=0;//variable to count number of decimal points in text

		//check if a character in text is neither a decimal point, nor digit
		for(int i=0;i<strlen(value_read);i++)
		{
			if(value_read[i]!='.' && !isdigit(value_read[i]))
			{
				check=-1;
			}
		}

		//Count number of decimal points
		for(int i=0;i<sizeof(value_read);i++)
		{
			if(*(value_read+i)=='.')
			{
				decCount++;
			}
		}

		if(atof(value_read)<0)//check for negative time value
		{
			printf("Invalid Time input\n");
			return -1;
		}

		if(check==0 && (decCount==0 || decCount==1))//We may at most have one decimal point and all other characters should be digits
			return atof(value_read);
		else//Case where conditions for real number were not met
			{
				printf("Invalid floating point value\n");
				return -1;//Assuming Component IDs cannot be negative
			}
		}
	else//case where no value was read as text
	{
		printf("No value read\n");
		return -1;
	}
}

/*
 * Function to check that a value read from file is a valid float vaue representing probability
 */
double toFloat_prob(char * value_read)
{
	if(strlen(value_read)>0)
	{
		int check=0;//variable to use as flag for validity
		int decCount=0;

		//check if a character in text is neither a decimal point, nor digit
		for(int i=0;i<(strlen(value_read));i++)
		{
			if(!isdigit(value_read[i]) && (value_read[i]!='.'))
			{
				check=-1;
			}
		}

		//Count number of decimal points
		for(int i=0;i<strlen(value_read);i++)
		{
			if(value_read[i]=='.')
			{
				decCount++;
			}
		}

		if(atof(value_read)<0 ||  atof(value_read)>1)//Probability between 0 and 1
		{
			printf("Invalid probability input\n");
			return -1;
		}

		if(check==0 && (decCount==0 || decCount==1))//We may at most one decimal point and all other characters should be digits
			return atof(value_read);
		else
			{
				printf("Invalid floating point value\n");
				return -1;//Assuming Component IDs cannot be negative
			}
	}
	else
	{
		printf("No value read\n");
		return -1;
	}
}

/*
 * Function to check if a value read to represent component type holds valid values
 */
int charCheck(char *value_read)
{
	//check if the entry is a single character
	if(strlen(value_read)>2)
	{
		printf("Invalid component type input length\n");
		return -1;
	}

	//Check for valid component input type
	if(value_read[0]!='G' && value_read[0]!='Q' && value_read[0]!='F' && value_read[0]!='X')
	{
		printf("Invalid component type input\n");
		return -1;
	}

	return 0;

}

/*
 * Function to create a local (as in the configuration program, not the application) generator object
 */
loc_gen* MakeLocalGen (int GenID, double IntTime, int DestID)
{
	loc_gen* p = (loc_gen*)malloc(sizeof(loc_gen));//create local generator object
   // Allocate space for component, fill in parameters
	printf("Creating local generator, Component ID: %d, Interarrival time: %lf, Destination ID %d\n",GenID, IntTime, DestID);
   if (p == NULL) //Check for success
   {
   	printf("malloc error\n");
   	return NULL;
   }

   //Initialize values
   p->id = GenID;
   p->type = 'G';
   p->mean_interarrTime = IntTime;
   p->destId = DestID;
   return p;//return object pointer to the local generator component object
}

/*
 * Function to create a local (as in the configuration program, not the application) QS object
 */
loc_QS* MakeLocalQS (int QSid, double ServTime, int DestID)
{
	loc_QS* p;//create local QS object
	printf("Creating local QS, Component ID: %d, Service time: %lf, Destination ID %d\n",QSid, ServTime, DestID);
   // Allocate space for component, fill in parameters
   if ((p=(loc_QS* )malloc (sizeof(loc_QS))) == NULL) //Check for malloc failure
   {
   	printf("malloc error\n");
   	return NULL;
   }
   //Initialize object values
   p->id = QSid;
   p->type = 'Q';
   p->mean_ServTime = ServTime;
   p->destId = DestID;
   return p;
}


/*
 * Function to create a local (as in the configuration program, not the application) Exit object
 */
loc_exit* MakeLocalExit (int ExitID)
{
	loc_exit *p;//create local exit component object
	// Allocate space for component, fill in parameters
	printf("Creating local exit component, Component ID: %d\n",ExitID);
	if ((p=(loc_exit* )malloc (sizeof(loc_exit))) == NULL) //check for malloc error
	{
		printf("malloc error\n");
	  	return NULL;
	}
	//initialize value
	p->id=ExitID;
	p->type='X';
	return p;
}

/*
 * Function to create a local (as in the configuration program, not the application) fork object
 */
loc_Fork* MakeLocalfork (int Fid, int numComponents, int* destID, double* probabilities)
{
	loc_Fork* p;//declar a pointer to a local fork object
   // Allocate space for component, fill in parameters
	printf("Creating local fork, Component ID: %d, Number of components: %d\n",Fid, numComponents);
   if ((p=(loc_Fork* )malloc (sizeof(loc_Fork))) == NULL) //check for malloc error
   {
   	printf("malloc error\n");
   	return NULL;
   }

   //initialize values
   p->id = Fid;
   p->numComponents = numComponents;
   p->type = 'F';
   for(int i=0;i<numComponents;i++)
   {
   	p->destID[i]=destID[i];//initialize destination IDs
   	p->prob[i]=probabilities[i];//initialize destination probabilities
   }
   for(int i=numComponents;i<1000;i++)
   {
   	p->destID[i]=-1;//initialize all array values of index greater than tha needed for destination forks with -1
   	p->prob[i]=-1.0;
   }

   return p;
}

/*
 * function to check destination validity for generators
 */
int Generator_check(loc_gen Gen, struct config_master_list* comp_list, int numofComponents)//Check that generator is connected to a valid component
{
	int check=0;//variable to act as flag for validity
	for(int i=0;i<numofComponents;i++)//loop through all coponents
	{
		if(Gen.destId==comp_list[i].id)//check if generator destination id matches the id of a component in the component array
			check++;
	}
	if(check==1)//Generator does not direct to a valid destination
	{
		return 0;
	}

	return -1;
}

/*
 * function to check destination validity for QS
 */
int QS_check(loc_QS QS, struct config_master_list* comp_list, int numofComponents)//Check that generator is connected to a valid component
{
	int check=0;//validity flag for destination ID of QS
	for(int i=0;i<numofComponents;i++)
	{
		if(QS.destId==comp_list[i].id)//QS destination is valid or not
			check++;//if QS component  destination is valid
		if(QS.destId==comp_list[i].id && comp_list[i].ComponentType=='G')//QS destination should not be a Generator
		{
			printf("QS connected to generator\n");
			return -1;
		}
	}
	if(check==1)//validity flag holds true
	{
		return 0;
	}

	return -1;//invalid destination of QS component
}

/*
 * function to check destination validity for forks
 */
int fork_check(loc_Fork fork, struct config_master_list* comp_list, int numofComponents)//Check that generator is connected to a valid component
{
	int check=0;//count for number of destinations of fork
	//Probability Check, number of components connected check
	double prob_sum=0;int numComp=0;
	for(int i=0;i<fork.numComponents;i++)
	{
		for(int j=0;j<numofComponents;j++)
		{
			if(fork.destID[i]==comp_list[j].id)//that destination IDs for the fork are valid
				{
					check++;
				}
			if(fork.destID[i]==comp_list[j].id && comp_list[j].ComponentType=='G')//check that the fork does not direct to a generator
				{
					printf("Fork connects to generator\n");
					return -1;
				}
		}
		prob_sum+=fork.prob[i];//sum up probabilities of forwarding to destination IDs
	}
	if(check!=fork.numComponents)//check that fork only directs to valid IDs
	{
		printf("Fork Connection error\n");
		return -1;
	}
	if(prob_sum!=1)//probability must sum to 1
	{
		printf("Invalid Probabilities for fork\n");
		return -1;
	}

	int c=0;
	//checking if number of components connected to matches the number given (K) in file
	while(fork.destID[c]!=-1 && fork.prob[c]!=-1)//note that probabilities and destination IDs beyond K (number of components the fork directs to) are initiaised as -1
	{
		numComp++;
		c++;
	}
	if(numComp!=fork.numComponents)
	{
		printf("Invalid number of connections to fork\n");
		return -1;
	}

	return 0;
}

/*
 * function to free memory allocated to all local (in configuration file) component objects
 */
int freeComp(struct config_master_list configComponents[], int numofComponents)//function to free memory created for local copies of components (local_components)
{
	printf("freeing components\n");
	for(int i=0;i<numofComponents;i++)
	{
		if(configComponents[i].Comp!=NULL)
			free(configComponents[i].Comp);
		else
		{
			printf("Error");
		}
	}
	return 0;
}

/*
 * function to print statistics to file
 */
int print_stat(char *outfilename, STAT *p, int numQS)
{
	FILE* fp;//create file pointer
	fp = fopen(outfilename, "w");//open file for writing, and initialize file pointer

	if(fp == NULL)//check for success of file open operation
	{
		printf("Write error\n");
		return -1;
	}

	//Print statistics to file
	fprintf(fp, "\tStatistics Output\n");
	fprintf(fp, "No. of customers entering system is %d\n",p->EnterCustomer);
	fprintf(fp, "No. of customers exiting the system is %d\n",p->ExitCustomer);
	fprintf(fp, "Minimum time a customer remains in the system is %lf\n", p->Min_inSystem);
	fprintf(fp, "Maximum time a customer remains in system is %lf\n", p->Max_inSystem);
	fprintf(fp, "Average time a customer remains in the system is %lf\n", p->Avg_inSystem);
	fprintf(fp, "Minimum total time a customer waits in queues is %lf\n",p->Min_inQueue);
	fprintf(fp, "Maximum total time a customer waits in queues is %lf\n", p->Max_inQueue);
	fprintf(fp, "Average total time a customer waits in queues is %lf\n", p->Avg_inQueue);

    for (int i = 0; i <=numQS; i++) {
        if (p->QueueCount[i] == 0) continue;
        fprintf(fp ,"Avg time in QS %d: %f\n", i, p -> Avg_each_Queue[i]);
    }

	if(fp!=NULL)
		fclose(fp);//close file pointer

    return 0;

}
