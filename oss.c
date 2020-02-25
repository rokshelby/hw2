
#include "myGlobal.h"

void sig_handler(int);
int around;
int max;
int strtSeq;
int increment;
int msec;
int trigger;
int before;
int local_clock;
int local_clock_nn;
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
	char * returnStr = malloc(sizeof(char) * size);
	for(i = 0; i < size; i++)
	{
		returnStr[i] = str[i];
	}
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


void DoProcesses(int max, int allowedAlive, int strtSeq, int increment, char * filename)
{
	int status = 0;	
	key_t key = ftok(".", 'c');
	int i;
	char ** argToPass = malloc(sizeof(char*) * 2);	
	int * chr;	
	int * buffer = malloc(sizeof(int) * (max + 2));
	int shmid = shmget(key, sizeof(*buffer),IPC_CREAT|0666);
	chr = (int*)shmat(shmid, NULL, 0);
        int keepCopy = strtSeq;	
	//clock
	chr[0] = 0;
	chr++;
	//clock_nn
	chr[0] = 0;
	chr++;	
	//initialize array
	for(i = 0; i < max; i++)
	{
		chr[i] = 0;
	}
	
	for(i = 0; i < 2; i++)
	{
		argToPass[i] = malloc(sizeof(int));
	}


	char bufWrit[255];
	int n = 0;
	int pida = 0;
	int childCompleted = 0;
	int alive = 0;
	int count = 0;
	int msec = 0;
	clock_t before = clock();
	int trigger = 2;
	printf("allowed to be alive %d\n",allowedAlive);
	signal(SIGINT, sig_handler);
	shmdt(chr);
	while(childCompleted < max && msec < trigger)
	{

		//printf("alive %d and allowedAlive %d\n", alive, allowedAlive);	
		if(alive < allowedAlive && childCompleted + alive < max)
		{	
			int shmid = shmget(key, sizeof(*buffer), 0666|IPC_CREAT);
			chr = (int*)shmat(shmid, NULL, 0);
			local_clock = chr[0];
			chr++;
			local_clock_nn = chr[0];
			chr++;
			chr[count] = fork();
			if(chr[count] < 0)
			{
				perror("Creation of child process was unsuccessful\n");
			}
			if(chr[count] == 0)
			{
				sprintf(bufWrit, "Child process %d was started at clock %d seconds and %d nanoseconds\n", getpid(), local_clock, local_clock_nn);
				write_file(filename, bufWrit);
				printf("Creation of child process was successful %d\n",getpid());
				char * st;
				st = (char*)malloc(sizeof(char)*sizeof(strtSeq));
				sprintf(st, "%d", count);
				strcpy(argToPass[0], getString(sizeof(st), st));
				sprintf(st, "%d", strtSeq);
				strcpy(argToPass[1], getString(sizeof(st), st));
				//printf("arguments %s %s\n",argToPass[0], argToPass[1]);
				execv("prime", argToPass);
				exit(0);				
			}
			if(chr[count] > 0)
			{
				printf("Parent sent off child to check for primality %d getpid  %d\n", chr[count], getpid());
				alive++;
				count++;	
				strtSeq = strtSeq + increment;
			}
			shmdt(chr);	
		}	
		for(n = 0; n < max; n++)
		{
			int shmid = shmget(key, sizeof(*buffer), 0666|IPC_CREAT);
			chr = (int*)shmat(shmid, NULL, 0);	
			chr++;
			chr++;
			pida = waitpid(chr[n], &status, WNOHANG);
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
				chr--;
				local_clock_nn = chr[0];
				chr--;
				local_clock = chr[0];
				sprintf(bufWrit, "Child Process %d, has completed at %d seconds and %d nanoseconds\n", getpid(), local_clock, local_clock_nn);
				write_file(filename, bufWrit);				
				alive--;
				childCompleted++;
			}
			shmdt(chr);
			
		}

		int shmid = shmget(key, sizeof(*buffer), 0666|IPC_CREAT);
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

		clock_t difference = clock() - before;
		msec = difference * 1000 / CLOCKS_PER_SEC;

		//printf("childCompleted %d\n", childCompleted);
	}
	write_file(filename, "List of Prime Numbers:\n");
	int r = 0;
	shmid = shmget(key, sizeof(*buffer), 0666|IPC_CREAT);
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
	//shmctl(shmid,IPC_RMID,NULL);
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
				printf("n\n");
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
				printf("s\n");
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
				printf("b\n");
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
				printf("i\n");
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
				printf("o\n");
				if(count + 1 < argc)
				{
					tempA = CheckNextArgument(argv[count+1]);
                                        if(tempA == 0)
                                        {
						filename = malloc(sizeof(char) * (sizeof(argv[count+1])));
						filename[0] = 0;	
                                                strcpy(filename, argv[count+1]);
                                                strcpy(filename, ".log");
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
	
	printArguments(maxChildProcesses, numChildToExists, startSequence, increment, filename);




		
	DoProcesses(maxChildProcesses, numChildToExists, startSequence, increment, filename);
	return 0;
}
