
#include "myGlobal.h"

void sig_handler(int);
int around;
int max;
int strtSeq;
int increment;
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

void DoProcesses(int max, int allowedAlive, int strtSeq, int increment, char * filename)
{
	int status = 0;	
	key_t key = ftok(".", 'a');

	int buffer[1];

	int * pidList = malloc(sizeof(int)*max);
	int i;
	for(i = 0; i < max; i++)
	{
		pidList[i] = 0;
	}


	int * chr;	

	int shmid = shmget(key, sizeof(buffer[1]), 0666|IPC_CREAT);
	chr = (int*)shmat(shmid, NULL, 0);
	chr[0] = 0;
	chr[1] = 0;	
	printf("clock %d nano %d",chr[0], chr[1]);
	int n = 0;
	int pida = 0;
	int childCompleted = 0;
	int alive = 0;
	int count = 0;
	printf("allowed to be alive %d\n",allowedAlive);
	signal(SIGINT, sig_handler);
	while(childCompleted < max)
	{

		//printf("alive %d and allowedAlive %d\n", alive, allowedAlive);	
		if(alive < allowedAlive)
		{	
			
			pidList[count] = fork();
			if(pidList[count] < 0)
			{
				perror("Creation of child process was unsuccessful\n");
			}
			if(pidList[count] == 0)
			{
				printf("Creation of child process was successful %d\n",getpid());
				char * st;
				st = (char*)malloc(sizeof(char)*sizeof(strtSeq));
				sprintf(st, "%d", strtSeq);		
				int sock = prime(strtSeq);
				//execl("./prime.o", 5, (char*)NULL);
				exit(0);				

			}
			if(pidList[count] > 0)
			{
				printf("Parent sent off child to check for primality %d getpid  %d\n", pidList[count], getpid());
				alive++;
				count++;
				strtSeq = strtSeq + increment;
			}
		}
	
		for(n = 0; n < max; n++)
		{
			pida = waitpid(pidList[n], &status, WNOHANG);
			if(pida == -1)
			{
				//printf("status %s\n", status);
				//printf("pidList %d\n", pidList[n]);	
				//perror("\n");
				childCompleted++;
			}
			else if(pida == 0)
			{
				//printf("Child still running\n");
			}
			else if(pida ==  pidList[n])
			{
				printf("child is finished %d\n", pidList[n]);
				alive--;
				childCompleted++;
			}
		}
		chr[1] = chr[1] + 1000000000;
		
		if(chr[1] > 1000000000)
		{
			chr[0] = chr[0] + 1;
			chr[1] = 0;
		}
	
		//i		printf("childCompleted %d\n", childCompleted);
	}
	shmdt(chr);
	
	
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
		strcpy(filename,"output");

	}
	
	printArguments(maxChildProcesses, numChildToExists, startSequence, increment, filename);

	DoProcesses(maxChildProcesses, numChildToExists, startSequence, increment, filename);
	return 0;
}
