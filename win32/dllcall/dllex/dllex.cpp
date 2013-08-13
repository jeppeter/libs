
#include "dllex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int PrintFunc(const char* pName)
{
	int ret = 0;
	if (pName)
	{
		ret = strlen(pName);
		printf("PrintFunc: %s\n",pName);
	}
	return ret;
}

int RepeatFunc(const char* pName)
{
	int ret;
	if (pName)
	{
		ret = strlen(pName);
		printf("RepeatFunc: %s\n",pName);
	}
	return ret;
}