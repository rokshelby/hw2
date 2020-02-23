#include "myGlobal.h"

int * var;
int prime(char * num)
{
	int i;
        int exitForFlag = 0;

	
	int numNum;
	sscanf(num, "%d", &numNum);
	
        for( i = 2; i < numNum-1 && exitForFlag == 0; i++)
        {
                if((numNum % i) == 0)
                {
                        exitForFlag = 1;
                }
        }

	key_t key = ftok(".", 'a');
	int shmid = shmget(key, sizeof(int), IPC_CREAT|0666);
	struct clocks_pids * x = (struct clocks_pids*) shmat(shmid, NULL, 0);

	if(exitForFlag == 1)
	{

		printf("The number %d isn't prime at clock %d\n", numNum, x->clock);
	}
	else
	{	
		printf("The number %d is prime at clock%d\n", numNum, x->clock);	
	}
	shmdt(x);


	return 0;
}

