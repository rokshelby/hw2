#include "myGlobal.h"

int main(int argc, char ** argv)
{
	int i;
        int exitForFlag = 0;
	int num = 0;
	int id = 0;
	int buffer = 0;
	int timeStamp = 0;
	//printf("how many arguments %d\n",argc);
	sscanf(argv[0], "%d", &id);
	sscanf(argv[1], "%d", &num);
	sscanf(argv[2], "%d", &buffer);
	//printf("id %d and num %d and shared memory %d \n", id, num, buffer);
	
	key_t key = ftok("./oss.c", 3);
	int * chr;
	int shmid = shmget(key, buffer  ,0666|IPC_CREAT);
	//printf("lu lu %d \n", shmid); 
	chr = (int*)shmat(shmid, NULL, 0);
	//printf("address of chr %p\n",&chr);
	//printf("right here\n");
	
	//printf("fkdjfkldjk\n");
	//printf(" jjjj %d id %d\n", num, id);
	////	timeStamp = chr->clock_c;
	shmdt(chr);
	
 	
        for( i = 2; i < num-1 && exitForFlag == 0; i++)
        { 
	
                if((num % i) == 0)
                {
                        exitForFlag = 1;
                }
		

		chr = (int*) shmat(shmid, NULL, 0);
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

		chr = (int*) shmat(shmid, NULL, 0);
		chr++;
		chr++;
		num = num * -1;
		chr[id] = num;		
		shmdt(chr);
	}
	else
	{	//printf("The number %d is prime at clock %d\n", num, chr[0]);	
		//printf("The number %d is prime at clock%d\n", numNum, chr[0]);
		//printf("save number %d at id %d \n", num, id);	

		chr = (int*) shmat(shmid, NULL, 0);
		chr++;
		chr++;
		chr[id] = num;
		shmdt(chr);
	}
	
	return 0;
}

