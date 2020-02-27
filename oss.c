
// Author Rakeem Shelby
// hw 2 for CS4760
// goal of the hw is to be able to work with shared memory.


#include "myGlobal.h"

void sig_handler(int);
int around;
int max;
int strtSeq;
int increment;
int msec;
int trigger;
int before;

void PrintHelpFile()
{

	printf("-h\tDescribe how the project should be run and then, terminate.\n");
	printf("-n x\tIndicate the maximum total of child processes oss will ever create(Default 4)\n");
	printf("-s x\tIndicate the number of children allowed to exist in the system at the same time.(Default 2)\n");
	printf("-b B\tStart of the sequence of numbers to be tested for primality.\n");
	printf("-i I\tIncrement between numbers that we test.\n");
	printf("-o filename\t Output file.\n");
}

int CheckNextArgument(char * next)
{	
	int temp = atoi(next);
	return temp;
}

char * getString(int size, char * str)
{
	int i = 0;
	char * returnStr = malloc(sizeof(char) * strlen(str));
	//	printf("%s my string \n",str);
	for(i = 0; i < strlen(str); i++)
	{
		returnStr[i] = str[i];
	}
	returnStr[strlen(str)] = 0;
	return returnStr;
}

void printArguments(int max,int num, int strtSeq, int increment,char * filename)
{
	printf("max child processes %d\n",max);
	printf("num child processes %d\n", num);
	printf("start sequence %d\n", strtSeq);
	printf("increment %d\n", increment);
	printf("filename %s\n", filename);

}

void sig_handler(int sig)
{

	exit(0);
}

void write_file(char * filename, char * information)
{
	FILE * fp;
	fp = fopen(filename, "a+");	
	fprintf(fp, information);
	fclose(fp);
}

void clear_file(char * filename)
{

	FILE * fp;
	fp = fopen(filename, "wb");
	fclose(fp);

}


void DoProcesses(int max, int allowedAlive, int strtSeq, int increment, char * filename)
{
	int status = 0;	
	key_t key = ftok("./oss.c", 3);
	int i;
	char ** argToPass = malloc(sizeof(char*) * 4);	
        int * chr;	
	//int * buffer = malloc(sizeof(int) * (max + 2));
	
	clear_file(filename);
	
			
	int shmid = shmget(key, sizeof(int) * (max + 2), 0666|IPC_CREAT);
	chr = (int *)shmat(shmid, NULL,0);
 	int buffer = sizeof(int) * (max + 2);      
	int keepCopy = strtSeq;	
	//clock
	chr[0] = 0;

	chr++;
	
	//clock_nn
	chr[0] = 0;
		
	//initialize array
	for(i = 0; i < max; i++)
	{
		chr[i] = 0;
	}

	pid_t * pids = malloc(sizeof(pid_t) * (max));

	//intialize character array to pass to child, id and number	
	for(i = 0; i < 3; i++)
	{
		argToPass[i] = malloc(sizeof(int)*8);
	}


	char bufWrit[255]; //used to write to file
	int n = 0; //used as index to check for completed processes
	int pida = 0; //used to check the value as a child process has completed
	int childCompleted = 0; //if child is completed it is incremented to make sure it will leave the while loop for max number of processes
	int alive = 0; //takes note of child processes that are alive
	int count = 0; //another index used for child creation process
	//int msec = 0; //milliseconds
//	clock_t before = clock(); //initialization of clock
	//	int trigger = 100; //when the program should stop 
	int local_clock_nn = 0;
	int local_clock = 0;
	signal(SIGINT, sig_handler); //allows for the ctrl-c command
	shmdt(chr);
	struct itimerval time;
	time.it_value.tv_sec = 2;
	time.it_value.tv_usec = 0;
	time.it_interval = time.it_value;

	setitimer(ITIMER_REAL, &time, NULL);
	while(childCompleted < max) //if childCompleted is less than max and the time is not up keep going
	{
		//printf("childCompleted %d max %d msec %d && trigger %d \n", childCompleted, max, msec, trigger);
		//if alive children is less than allowedalive then create a process
		//completed children + currently alive must be less than max allowed children.
		if(alive <= allowedAlive && childCompleted + alive <= max && alive <= 20)
		{	
			//how to access the shared memory

			chr = (int*)shmat(shmid, NULL, 0);
			local_clock = chr[0];
		//			printf("%d the shared clock\n ", chr[0]);
			chr++; //iteration through shared memory
			local_clock_nn = chr[0];
			chr++;	
			int pid;
			pid  = fork(); //creates child process
			if(pid < 0)
			{
				perror("Creation of child process was unsuccessful\n");
			}
			else if(pid == 0)
			{
				sprintf(bufWrit, "Child process %d was started at clock %d seconds and %d nanoseconds\n", getpid(), local_clock, local_clock_nn);
				write_file(filename, bufWrit);
				printf("Creation of child process was successful %d\n",getpid());
				char * st;
				st = (char*)malloc(sizeof(char)*sizeof(strtSeq)); //following code used to pass arguments to prime
				sprintf(st, "%d", count);
				strcpy(argToPass[0], getString(strlen(st), st));
				sprintf(st, "%d", strtSeq);
				strcpy(argToPass[1], getString(strlen(st), st));
				sprintf(st, "%d", buffer);
				strcpy(argToPass[2], getString(strlen(st), st));
				//printf("integer arugments %d %d %d\n",count, strtSeq, sizeof(*buffer));
				//printf("arguments %s %s\n",argToPass[0], argToPass[1]);
				execv("prime", argToPass);
				exit(0);				
			}
			else if(pid > 0)
			{
				pids[count] = pid;
				printf("Parent sent off child to check for primality %d getpid  %d\n", pids[count], getpid());
				alive++;
				count++;	
				strtSeq = strtSeq + increment;
				//printf("count variable %d\n", count);
			
			}	
		}	
		for(n = 0; n < max; n++)
		{
			chr = (int*)shmat(shmid, NULL, 0);	
			chr++;
			chr++;
			pida = waitpid(pids[n], &status, WNOHANG); //any process finished it picks up
			//printf("%d: status %d\n",n, status);
			if(pida == -1)
			{
				//printf("status %s\n", status);
				//printf("pidList %d\n", pidList[n]);	
				//perror("\n");		
			}
			else if(pida == 0)
			{
				//printf("Child still running\n");
			}
			else if(pida > 0)
			{
				printf("child is finished %d\n", pida);
				chr--; //move through shared memory
				//printf("local cloc %d clock %d\n",local_clock_nn, local_clock);
				local_clock_nn = chr[0];
			
				chr--;
				local_clock = chr[0];
				sprintf(bufWrit, "Child Process %d, has completed at %d seconds and %d nanoseconds\n", pida, local_clock, local_clock_nn);
				write_file(filename, bufWrit);				
				alive--;
				childCompleted++;
			}
			shmdt(chr);
			
		}

                chr = (int*)shmat(shmid, NULL, 0);
		chr++;
		chr[0] = chr[0] + 10000; //nano clock increment
	
		if(chr[0] > 1000000000) //check nano
		{
			chr--;	
			chr[0] = chr[0] + 1; //big clock increment;
			chr++;
			chr[0] = 0; //zero out nano clock
		}
		shmdt(chr);

		//clock_t difference = clock() - before;

		//msec = difference * 10 / CLOCKS_PER_SEC;

		//printf("childCompleted %d\n", childCompleted);
	}
	//printf("childCompleted %d max %d msec %d && trigger %d \n", childCompleted, max, msec, trigger);
	write_file(filename, "List of Prime Numbers:\n");
	int r = 0;
	chr = (int*)shmat(shmid, NULL, 0);
	chr++;
	chr++;
	int localStrt = keepCopy;
	for(r = 0; r < max; r++)
	{
		//printf("comparison %d vs %d\n", chr[r], localStrt);
		if(localStrt == chr[r] && localStrt != 0)	
		{
			sprintf(bufWrit, "prime: %d\n", localStrt);
			write_file(filename, bufWrit);
		}
		localStrt = localStrt + increment;
	}

	localStrt = keepCopy;
	write_file(filename, "List of Non-Prime Numbers:\n");
	for(r = 0; r < max; r++)
	{
		if( (localStrt*-1) == chr[r])
		{
			sprintf(bufWrit, "non prime: %d\n", localStrt);
			write_file(filename, bufWrit);
		}
		localStrt = localStrt + increment;
	}
	localStrt = keepCopy;
	write_file(filename, "Unevaluated Numbers:\n");
	for(r = 0; r < max; r++)
	{
		if(chr[r] == -1)
		{
			sprintf(bufWrit, "unevaluated: %d\n", localStrt);
			write_file(filename, bufWrit);
		}		
		localStrt = localStrt + increment;
	}
	shmdt(chr);	
	if ( shmctl(shmid,IPC_RMID,NULL) < 0 )
	    fprintf ( stderr, "Could not deallocate shared memory; remove it manually\n" );
}



int main(int argc, char * argv[])
{
        int cmdLineOption;
	int doneReading = 0;
	int count = 1;
	//Defaults
	int maxChildProcesses = 4;
	int numChildToExists = 2;
	int startSequence;
	char * filename = 0;
	int increment = 5;
	int tempA = 0;
		
	while((cmdLineOption = getopt(argc, argv, "hnsbio")) != 1 && doneReading == 0)
	{
         	switch (cmdLineOption)
                {
                	case 'h':			
				PrintHelpFile();
                        	break;
                 	case 'n':
				//printf("n\n");
				if(count + 1 < argc)
				{
					tempA = CheckNextArgument(argv[count+1]);
					if(tempA != 0)
					{
						maxChildProcesses = tempA;
						count++;		
					}	
				}
				break;
                                     
                 	case 's':
				//printf("s\n");
				if(count + 1 < argc)
				{
				        tempA = CheckNextArgument(argv[count+1]);
                                        if(tempA != 0)
                                        {
                                                numChildToExists = tempA;
                                                count++;
                                        }
				}
				break;

	                case 'b':
				//printf("b\n");
				if(count + 1 < argc)
				{
					tempA = CheckNextArgument(argv[count+1]);
                                        if(tempA != 0)
                                        {
                                                startSequence = tempA;
                                                count++;
                                        }
					else
						startSequence = 101;
				}
				break;
                 	case 'i':
				//printf("i\n");
				if(count + 1 < argc)
				{
					tempA = CheckNextArgument(argv[count+1]);
                                        if(tempA != 0)
                                        {
                                                increment = tempA;
                                                count++;
					}
					else
						increment = 4;
				}									
				break;
			case 'o':
				//printf("o\n");
				if(count + 1 < argc)
				{
					tempA = CheckNextArgument(argv[count+1]);
                                        if(tempA == 0)
                                        {
						filename = malloc(sizeof(char) * (sizeof(argv[count+1])));
						filename[0] = 0;
						strcpy(filename, argv[count+1]);
						if(strlen(filename) > 4)
						{	

							char bf[4];
							snprintf(bf, 4, "%s", filename);
							if(!strcmp(bf,".log"))
							{
	
	                                                	strcat(filename, ".log");
							}
						}
						else
						{
								strcat(filename, ".log");
						}
                                        }
				}
				break;
			default:
				doneReading = 1;
				break;		
			
		}
		
		count++;	
	}
	if(filename == NULL)
	{
		filename = malloc(sizeof(char) * (sizeof("output")));
		strcpy(filename,"output.log");

	}
	
	//printArguments(maxChildProcesses, numChildToExists, startSequence, increment, filename);	
	DoProcesses(maxChildProcesses, numChildToExists, startSequence, increment, filename);
	return 0;
}
