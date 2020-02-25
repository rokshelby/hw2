#include "myGlobal.h"

int main(int argc, char ** argv)
{
	int i;
        int exitForFlag = 0;
	int num = 0;
	int id = 0;
	int timeStamp = 0;
	sscanf(argv[0], "%d", &id);
	sscanf(argv[1], "%d", &num);
	
	//printf("id %d and num %d\n", id, num);

	key_t key = ftok(".", 'c');
	int * chr;
	int * buffer = malloc(sizeof(int) * (num + 2));
	int shmid = shmget(key, sizeof(*buffer), 0444|IPC_CREAT);
	chr = (int*)shmat(shmid, NULL, 0);
	//printf("right here\n");
	
	timeStamp = chr[0];
	shmdt(chr);
        for( i = 2; i < num-1 && exitForFlag == 0; i++)
        {
                if((num % i) == 0)
                {
                        exitForFlag = 1;
                }
		
		int shmid = shmget(key, sizeof(*buffer), 0666|IPC_CREAT);
		chr = (int*) shmat(shmid, NULL, 0);
		chr++;
		chr++;
		if(timeStamp > 1000000)
			chr[id] = -1;
		shmdt(chr);
	        //printf("%d number \n",num);
		timeStamp = timeStamp + 10000;
        }

	if(exitForFlag ==1)
	{
		//printf("The number %d isn't prime at clock %d\n", num, chr[0]);
		//printf("The number %d isn't prime at clock %d\n", numNum,chr[0]);
		int shmid = shmget(key, sizeof(*buffer), 0666|IPC_CREAT);
		chr = (int*) shmat(shmid, NULL, 0);
		chr++;
		chr++;
		num = num * -1;
		chr[id] = (int*)num;		
		shmdt(chr);
	}
	else
	{	//printf("The number %d is prime at clock %d\n", num, chr[0]);	
		//printf("The number %d is prime at clock%d\n", numNum, chr[0]);
		//printf("save number %d at id %d \n", num, id);	
		int shmid = shmget(key, sizeof(*buffer), 0666|IPC_CREAT);
		chr = (int*) shmat(shmid, NULL, 0);
		chr++;
		chr++;
		chr[id] = (int*)num;
		shmdt(chr);
	}

	return 0;
}

