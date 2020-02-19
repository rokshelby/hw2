#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myGlobal.h"

int var;
int pic(int argc, char * argv[])
{
	printf("count of arguments: %d argument %s\n", argc, argv[0]);
	printf("iota %d\n", var);
	//var = var - 50;
	return 0;
}
