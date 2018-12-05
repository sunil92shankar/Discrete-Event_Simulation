/*
 * configuration_program.c
 *
 *  Created on: Oct 4, 2018
 *      Author: SUNIL
 */

//Standard libraries
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<ctype.h>

//User defined header
#include"interface.h"//interface file has functions necessary to create components in the application, run simulation and to get statistics
#include"config_functions.h"//functions necessary for successful reading of the configuration input

int main(int argc, char *argv[])
{
	if(argc!=4)
	{
			printf("Incorrect number of program arguments\n");
			return -1;
	}

	float simTime = atof(argv[1]);

	char *filename = argv[2];

	//code to add .txt to filename
	int str_len = strlen(filename) + strlen(".txt");//get the sum lengths of strings to be concatenated
	char *new1 = (char*)malloc(str_len*sizeof(char) + 1);//allocate memory for new string including space for end of string character \0
	*new1 = '\0';//store no value in string (empty string) to prepare for concatenation
	filename = strcat(new1,filename);
	filename = strcat(filename ,".txt");//concatenate both strings

	char *outfilename = argv[3];

	//code to add .txt to filename
	str_len = strlen(outfilename) + strlen(".txt");//get the sum lengths of strings to be concatenated
	char *new2 = (char*)malloc(str_len*sizeof(char) + 1);//allocate memory for new string including space for end of string character \0
	*new2 = '\0';//store no value in string (empty string) to prepare for concatenation
	outfilename = strcat(new2,outfilename);
	outfilename = strcat(outfilename ,".txt");//concatenate both strings


	struct config_master_list configComponents[MAXCOMPONENTS];//a local array of components with all related data. This will be passed to functions to create components for the engine/application

	srand(time(NULL));//seeding rand() with time
	int No_of_Stations = -10;//variable to hold number of components as read from the text file
	//char* filename="D:/Google Drive/CSE 6010 Assignment 3/Configuration_test.txt";
	FILE *fp;//declare file pointer
	fp = fopen(filename, "r");//open file with given wile name using declared file pointer

	if(fp == NULL)//check that file pointer is valid
	{
		printf("Could not open file %s", filename);
		return -1;
	}

	int count_lines=0;//variable that counts the number of lines read (analogous to number of components)
	char* component_type = (char *)malloc(sizeof(char));//character pointer to hold information on component type. Obtained from text read in from the configuration file.
	char value_read[1000];//Assuming the longest line read is less than 1000 characters

	if(fgets(value_read, 1000, fp) != NULL)//read a line, store values in 'value_read'
	{
		value_read[strlen(value_read)-2]='\0';//value_read[strlen(value_read)];// replacing '\n' with '\o'
		No_of_Stations= toInt(value_read);//Store number of components read from file//https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_71/rtref/itoi.htm

		if(No_of_Stations!=-1)//check that we get a valid value of number of components from the text file
			printf("No. of stations is read as: %d\n",No_of_Stations);
		else
			{
				printf("Invalid Input of component count\n");
				return -1;
			}

		if(No_of_Stations>=MAXCOMPONENTS)//check that the number of components read is within the upper limit of number of components
		{
			printf("Invalid Input\n");
			return -1;
		}
	}


	while(fgets(value_read, 1000, fp) != NULL)//keep reading from text line by line
		{
			printf("\n");
			count_lines++;//increment the number of line read (analogous to number of components read)
			if(value_read[strlen(value_read)-1]=='\n')//if last value is '\n', replace with '\o'
				value_read[strlen(value_read)-1]=value_read[strlen(value_read)];
			printf("Check Value read:%s\n",value_read);//print the line that was read
			int fork_count=-1;//variable to check value of number of components a fork connects to
			int componentID=-1;//component ID of the component currently being read (will be initialized with actual value later)
			double time_parameter=-1;//variable to hold either mean interarrival time, or service time
			double forkprob[1000]={-1};//Probabilities data for a fork, to be aptly updated later
			int forkdest[1000]={-1};//destination data for a fork, will be aplty updated later in the code

			int fork_compcount=0;//variable to count number of destination components for forks
			int fork_probcount=0;//variable to count probabilities for fork destinations
			int destID=-1;//variable to hold destination ID for QS

			if(count_lines==0)
			{
				//NULL (code previously here was moved before the while loop to read the first line)
			}
			else
			{
					//Break line into strings  http://pubs.opengroup.org/onlinepubs/7908799/xsh/strtok_r.html TODO copy
					// Returns first token
				    char* portion = strtok(value_read, " ");//portion holds a part of a line separated from the others by whitespace
				    int portion_count=0;//variable counting the portion of line being read (each portion being separated from the other with a space)
				    while (portion != NULL) //looping through all portions/words in a line
				    {
				    	if(portion_count==0)//reading the first word/portion of a line, analogous to reading the component ID of a component
				    	{
				    		componentID = toInt(portion);//convert value read into integer value for component ID
				    		if(componentID!=-1)//check validity of component ID
				    			{
				    				printf("Component ID of component %d : %d\n", count_lines, componentID);
				    			}
				    		else
				    			{
				    				printf("Invalid Input\n");
				    				return -1;
				    			}
				    	}

				    	//checking second value of line, the component type
				    	if(portion_count==1)
				    	{
			    			printf("Component type of component %d : %s\n", count_lines,portion);
			    			if(charCheck(portion)==-1)//checking if the 'portion' value is valid
			    			{
			    				printf("Invalid Input\n");
			    				return -1;
			    			}
			    			component_type=portion;//initializing commponent type with value stored in 'portion'

			    			if(*component_type=='X')//if component type is Exit (and portion count is 1, then there is no more data needed, and we may declare a local Exit component object)
			    			{
			    				//initialize next component array with ID, type and void *data values
			    				configComponents[count_lines-1].id=componentID;
			    				configComponents[count_lines-1].ComponentType ='X';
			    				configComponents[count_lines-1].Comp = MakeLocalExit(componentID);//returns pointer to the local (as in configuration file and not the application file) Exit component
			    				//note that the application uses a different kind or components array
			    			}

			    		}


				    	if(*component_type!='F')//if component type is not FORK (fork has slightly complex implementations taken care of later)
				    	{
				    		//checking third value in a line, valid here for Qstations and generators
							if(portion_count==2)
							{
								if(*component_type=='G')//reading the third data in a line for a generator
								{
									time_parameter = toFloat_time(portion);//reading the interarrival time
									if(time_parameter==-1)//checking validity of interarrival time value
									{
										printf("Invalid time input\n");
										return -1;
									}
									printf("Interarrival time for generator (component id %d) is: %lf\n",count_lines,time_parameter);
								}
								if(*component_type=='X')//reading third data value in a line for exit, this would make no sense as exit needs only 2 data values
								{
									printf("Erroneous input! Exit component cannot have more than 2 input arguments");
									return -1;
								}
								if(*component_type=='Q')//reading third data value in a line for Queue Station
								{
									printf("Average service time for Station (component number in list (not id) %d) is: %lf\n",count_lines, toFloat_time(portion));
									time_parameter = toFloat_time(portion);//store, after conversion the mean service time
									if(time_parameter==-1)//check validity of the mean service time reading
									{
										printf("Invalid time input\n");
										return -1;
									}
								}

							}

							//checking fourth value in a line, valid here for Qstations and generators
							if(portion_count==3)
							{
								if(*component_type=='G')//reading fourth data in a line for a generator
								{
                                                                        portion[strlen(portion)-1]='\0';
									printf("Destination for generator (component number as listed (not id) %d) is: %d\n",count_lines, toInt(portion));
									destID = toInt(portion);//data read in 'portion' should be destination ID. This is stored in destID

									if(destID==-1)//check validity of destination ID of generator
									{
										printf("Invalid Destination ID\n");
										return -1;
									}

									//initialize components array with value of the newly generated generator component
									configComponents[count_lines-1].id=componentID;
									configComponents[count_lines-1].ComponentType ='G';
									configComponents[count_lines-1].Comp = MakeLocalGen(componentID, time_parameter, destID);//this function returns a pointer to the local generator object created
								}
								if(*component_type=='X')//fourth value of line read for exit component
								{
									//Invalid, check performed at portion_count=2
								}
								if(*component_type=='Q')//fourth value in line read for Queue Station
								{
									portion[strlen(portion)-1]='\0';
									destID = toInt(portion);//convert value read into valid integer and stored in destID

									if(destID==-1)//check validity of destination ID
									{
										printf("Invalid Destination ID\n");
										return -1;
									}

									printf("Destination for Station (component number as listed (not id) %d) is: %d\n",count_lines, toInt(portion));
									//Initialize next set of values in the component array with the data for the QS created in configuration file. Note that QS is not yet created in application. That comes later
									configComponents[count_lines-1].id=componentID;
									configComponents[count_lines-1].ComponentType ='Q';
									configComponents[count_lines-1].Comp = MakeLocalQS(componentID, time_parameter, destID);
								}
								if(*component_type=='F')//fourth value in line read for Fork
								{
									//Operations for Fork station performed separately
								}

							}
							if(portion_count==4)//check for amount of data read for QS/generator
							{
								printf("Erroneous input. QS/Generator cannot have more than 4 inputs per line\n");
								return -1;
							}
				    	}
				    	else if (portion_count>=2)//now checking for forks as part of the else conditions
				    	{
				    		if(portion_count==2)//reading thrid data value in line for fork, this is the number of components, the fork directs customers to
				    		{
				    			fork_count = toInt(portion);//convert value obtained into valid integer value
				    			if(fork_count==-1)//check for validity of fork_count
				    			{
				    				printf("Invalid fork count\n");
				    				return -1;
				    			}
				    			printf("No of components connecting to fork (component ID %d) is %d\n",count_lines,fork_count);
				    		}
				    		else if ((portion_count>2) && (portion_count<=(2+fork_count)))//now checking for the probability values of the fork directing to destination components
				    		{
				    			double prob=toFloat_prob(portion);//convert value read to valid double value
				    			if(prob==-1)//check probability value validity
				    			{
				    				printf("Invalid probability input\n");
				    				return -1;
				    			}
				    			printf("Probabilities related to fork (component ID %d) is %lf\n",count_lines,prob);
				    			if(fork_probcount<fork_count)//check that the variable counting the number of probability values is less than the number of components
				    			{
				    				forkprob[fork_probcount]=prob;//intilialize array storing fork probabilities
				    			}
				    			fork_probcount++;//update count of probability values stored
				    		}
				    		else if ((portion_count>(2+fork_count)) && (portion_count<(2+2*fork_count)))//check destination ID value read for a fork
				    		{
				    			int destID=toInt(portion);//convert destination ID value to integer
				    			if(destID==-1)//check validity of destination ID for fork
				    			{
				    				printf("Invalid destination ID input\n");
				    				return -1;
				    			}
				    			printf("IDs related to fork (component ID %d) is %d\n",count_lines,destID);
				    			if(fork_compcount<fork_count)
				    			{
				    				forkdest[fork_compcount]=destID;//intilialize array storing fork destination IDs
				    			}
				    			fork_compcount++;//update count of destination values stored
				    		}
						else if(portion_count==(2+2*fork_count))
						{
							portion[strlen(portion)-1]='\0';
							int destID=toInt(portion);//convert destination ID value to integer
				    			if(destID==-1)//check validity of destination ID for fork
				    			{
				    				printf("Invalid destination ID input\n");
				    				return -1;
				    			}
				    			printf("IDs related to fork (component ID %d) is %d\n",count_lines,destID);
				    			if(fork_compcount<fork_count)
				    			{
				    				forkdest[fork_compcount]=destID;//intilialize array storing fork destination IDs
				    			}
				    			fork_compcount++;//update count of destination values stored
					
						}
				    		else if(portion_count>(2+2*fork_count))//check if there is more data presented than required/makes sense
				    		{
				    			printf("Erroneous input in fork input line. More data than required\n");
				    			return -1;
				    		}

				    		if(fork_probcount==fork_count && fork_compcount==fork_count)//ensure that count of probability value and destination ID values matches the number of components the fork is supposed to connect to
				    		{
				    			//initialize next value for the component array
				    			configComponents[count_lines-1].id=componentID;
				    			configComponents[count_lines-1].ComponentType ='F';
				    			configComponents[count_lines-1].Comp = MakeLocalfork(componentID,fork_compcount, forkdest, forkprob);
				    		}

				    	}

				        portion = strtok(NULL, " ");
				        portion_count++;//update the portions/words read
				    }
				    if(portion_count<2)//checking for insufficient data read in general
				    {
				    	printf("Insufficient data\n");
				    	return -1;
				    }
				    if(*component_type=='G' && portion_count!=4)//checking for insufficient data read for generator
				    {
				    	printf("Insufficient data for generator\n");
				    	return  -1;
				    }
				    if(*component_type=='Q' && portion_count!=4)//checking for insufficient data read for QS
				    {
				       	printf("Insufficient data for QS\n");
				       	return  -1;
				    }
				    if(*component_type=='X' && portion_count!=2)//checking for insufficent deata read for exit
				    {
				       	printf("Insufficient data for QS\n");
				       	return  -1;
				    }
				    if(*component_type=='F' && portion_count<(3+2*fork_count))//checking for insufficient data read for Fork
				    {
				       	printf("Insufficient data for Fork\n");
				       	return  -1;
				    }
			}

			
		}

	if(No_of_Stations!=count_lines)//check that number of stations mentioned in input file matches the number of data lines in the text file
	{
		printf("No of stations mentioned does not match data input\n");
		return -1;
	}

	if(fp!=NULL)
		fclose(fp);//close file pointer

	printf("\n");
	for(int i=0;i<No_of_Stations;i++)//printing all components information
	{
		printf("Comp ID: %d, Comp Type: %c\n",configComponents[i].id,configComponents[i].ComponentType);
	}

	//
	//check Generators for validity of destination IDs
	//
	int gen_check=0;
	for(int i=0;i<No_of_Stations;i++)
	{
		if(configComponents[i].ComponentType=='G')//check if component is generator
		{
			gen_check = Generator_check(*(loc_gen *)configComponents[i].Comp, configComponents, No_of_Stations);
		}
		if(gen_check==-1)//check for validity
		{
			printf("Generator connected to invalid component ID\n");
			return -1;
		}
	}


	//
	//check Queue Stations for validity of destination IDs
	//
	int QS_checkval=0;
	for(int i=0;i<No_of_Stations;i++)
	{
		if(configComponents[i].ComponentType=='Q')//check if component is QS
		{
			QS_checkval = QS_check(*(loc_QS *)configComponents[i].Comp, configComponents, No_of_Stations);
		}

		if(QS_checkval==-1)//check for validity
		{
			printf("QS connected to invalid component ID\n");
			return -1;
		}
	}


	//
	//check Forks for  valid destination IDs and probabilities
	//
	int fork_checkval=0;
	for(int i=0;i<No_of_Stations;i++)
	{
		if(configComponents[i].ComponentType=='F')//check if component is fork
		{
			fork_checkval = fork_check(*(loc_Fork *)configComponents[i].Comp, configComponents, No_of_Stations);
		}
		if(fork_checkval==-1)//validity check
		{
			printf("Fork error\n");
			return -1;
		}
	}

	//
	//Arrange components in order of ID
	//note that the order in which components appear in the input file is not necessarily in order of their IDs
	//Here, we rearrange the components array in order such that the data when passed to applications, is in order of components IDs
	//
	for(int i=0;i<No_of_Stations;i++)
	{
		for(int j=i+1;j<No_of_Stations;j++)
		        {
		            if(configComponents[i].id>configComponents[j].id)
		            {
		                struct config_master_list temp = configComponents[i];
		                configComponents[i]  = configComponents[j];
		                configComponents[j]  = temp;
		            }
		        }
	}

	int numQS=0;//variable to hold information on number of queue stations
	//
	//Create components in application
	//
	for(int i=0;i<No_of_Stations;i++)
	{
		printf("ID: %d\n", configComponents[i].id);
		if(configComponents[i].ComponentType=='G')
		{
			//create Generator
			int check_func = MakeGenerator (configComponents[i].id, ((loc_gen*)(configComponents[i].Comp))->mean_interarrTime, ((loc_gen*)(configComponents[i].Comp))->destId);
			if(check_func==0)//failure check
				return -1;
		}
		if(configComponents[i].ComponentType=='Q')
		{
			numQS=configComponents[i].id;
			int check_func = MakeQStation (configComponents[i].id, ((loc_QS*)(configComponents[i].Comp))->mean_ServTime, ((loc_QS*)(configComponents[i].Comp))->destId) ;
			if(check_func==0)//failure check
				return -1;
			//create QS
		}
		if(configComponents[i].ComponentType=='F')
		{
			printf("Num Comp fork %d", ((loc_Fork*)(configComponents[i].Comp))->numComponents);
			int check_func = MakeFork(configComponents[i].id, ((loc_Fork*)(configComponents[i].Comp))->numComponents, ((loc_Fork*)(configComponents[i].Comp))->prob, ((loc_Fork*)(configComponents[i].Comp))->destID);
			if (check_func==0)//failure check
				return -1;
			//create Fork
		}
		if(configComponents[i].ComponentType=='X')
		{
			int check_func = MakeExit(configComponents[i].id);
			if(check_func==0)//failure check
				return -1;
			//create Exit
		}
	}

	//double simTime=10.0;//TODO remove
	//char *outfilename="D:/Google Drive/CSE 6010 Assignment 3/output.txt";//TODO remove
	runsim_config(simTime);//run simulation
	STAT* p = GetStat();//get simulation statistics
	int writecheck = print_stat(outfilename, p, numQS);//execute write operation to print statistics to file
	if(writecheck==-1)//check for success of write operation
	{
		printf("Unsuccessful Write Operations\n");
		return -1;
	}
	freeComp(configComponents, No_of_Stations);//free memory allocated to local (as in the configuration program) components instances
	Free();
	//free(component_type);
	free(new2);
	free(new1);
	return 0;
}
