
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

void DoProcesses(int max, int allowedAlive, int strtSeq, int increment, char * filename)
{
	int status = 0;	
	key_t key = ftok(".", 'a');
	int shmid = shmget(key, sizeof(struct clocks_pids), IPC_CREAT|0666);
	struct clocks_pids * x = (struct clocks_pids*)shmat(shmid, NULL, 0);
	x->pid = (int**)malloc(sizeof(int*)*max);
	int i = 0;
	for(i = 0; i < max; i++)
	{
		x->pid[i] = (int*)malloc(sizeof(int));
	}
	shmdt(x);
	int n = 0;
	int pida = 0;
	int childCompleted = 0;
	int alive = 0;
	while(childCompleted < max)
	{
		key_t key = ftok(".",'a');
		int shmid = shmget(key, sizeof(struct clocks_pids), IPC_CREAT|0666);
		struct clocks_pids * x = (struct clocks_pids*)shmat(shmid, NULL, 0);
		for(n = childCompleted; n < allowedAlive-alive; n++)
		{
			*x->pid[n] = fork();
			if(x->pid[n] < 0)
			{
				perror("Creationg of child process was unsucessful\n");
				char * st;
				st = (char*)malloc(sizeof(char)* sizeof(strtSeq));
				sprintf(st, "%d", strtSeq);
				execl("./prime.o", st, (char*)NULL);
				free(st);
				exit(0);
			}
			if(x->pid[n] == 0)
			{
				printf("Creationg of child process was successful\n");
			}
			if(x->pid[n] > 0)
			{
				printf("Parent sent off child to check for primality\n");
				alive++;
			}
		}
		for(n = 0; n < max; n++)
		{
			pida = waitpid(*x->pid[n], &status, WNOHANG);
			if(pida == -1)
			{
	
				perror("problem\n");
			}
			else if(pida == 0)
			{
				//printf("Child still running\n");
			}
			else if(pida ==  *x->pid[n])
			{
				printf("child is finished %d \n", *x->pid[n]);
				alive--;
				childCompleted++;
			}
		}
		x->clock_nn = x->clock_nn + 10000;

		if(x->clock_nn > 10000)
			x->clock++;
		shmdt(x);
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
