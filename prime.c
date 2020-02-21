#include "myGlobal.h"

int * var;
int prime(int num)
{
	int i;
        int exitForFlag = 0;
	
        for( i = 2; i < num-1 && exitForFlag == 0; i++)
        {
                if((num % i) == 0)
                {
                        exitForFlag = 1;
                }

		
		key_t key = ftok(".", 'a');
	        int shmid = shmget(key, sizeof(int), IPC_CREAT|0666);
       		var = (int*) shmat(shmid, NULL,0);
		
		
		printf("child %d clock %d\n", getpid(), *var);
		*var = *var + 1;
		
        	shmdt(var);
        }
	return exitForFlag;
}

