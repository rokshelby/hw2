
#include "myGlobal.h"

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



void DoProcesses(int max, int num, int strtSeq, int increment, char * filename)
{
	
	int status = 0;	
	int pidIndex = 0;
	int around  = 0;
	int clock = 0;
	char *  myargs = malloc( sizeof(int));
	char * myenv  = {NULL};
	int ret;
	pid_t pid;
	pid_t * pids = malloc(sizeof(pid_t) * max);
	int n = 0;
	pidIndex = 0;	
	pid_t pida;	
	while(max > 0)
	{
		
		if(around < num)
		{	
			pid = fork();
			if(pid < 0)
			{
				perror("creation of child process was unsucessful\n");
				
			}

			if(pid == 0)
			{
				printf("creation of child process was successful\n");
				//call prime
				//
				n = prime(strtSeq);
				if(n == 1)
				{
					printf("the number %d is not prime\n",strtSeq);

				}
				else
					printf("The number %d is prime\n", strtSeq);
				exit(0);
			}
			
			if(pid > 0)
			{
	
	
				printf("child finished\n");
				max--;
				around++;
				strtSeq = strtSeq + increment;
				
			}
			
		}
		pida = wait(&status);
		printf("child with pid %ld exited with timestep %d \n",(long)pida, clock);
	around--;

	}	
	
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
	key_t key = ftok(".", 'a');
	int shmid = shmget(key, sizeof(int), IPC_CREAT|0666);
	int * var = (int*) shmat(shmid, (void*)0,0);
	*var = 5;
	
	shmdt(var);
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
