#include "myGlobal.h"

int prime(int num)
{
	int i;
        int exitForFlag = 0;

	
	int numNum = num;
	//sscanf(num, "%d", &numNum);
	//printf("chips %d\n", numNum);	
        for( i = 2; i < numNum-1 && exitForFlag == 0; i++)
        {
                if((numNum % i) == 0)
                {
                        exitForFlag = 1;
                }
        }


	key_t key = ftok(".", 'a');

        int buffer[1];
        int * chr;
        int shmid = shmget(key, sizeof(buffer[1]), 0666|IPC_CREAT);
	
	        chr = (int*)shmat(shmid, NULL, 0);
	if(exitForFlag ==1)
	{
		printf("The number %d isn't prime at clock %d\n", numNum, chr[0]);
		//printf("The number %d isn't prime at clock %d\n", numNum,chr[0]);
	}
	else
	{	printf("The number %d is prime at clock %d\n", numNum, chr[0]);	
		//printf("The number %d is prime at clock%d\n", numNum, chr[0]);	
	}
	shmdt(chr);
	return 0;
}

