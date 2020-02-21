#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myGlobal.h"

int var;
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
                  printf("child %d clock %d\n",getpid(), var);
                  var = var + 1;
          }
         
          return exitForFlag;

}

