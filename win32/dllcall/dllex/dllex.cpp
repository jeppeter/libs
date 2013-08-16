
#include "dllex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 
#include <excpt.h>
int PrintFunc(const char* pName)
{
    int ret = 0;
    if(pName)
    {
        ret = strlen(pName);
        printf("PrintFunc: %s\n",pName);
    }
    return ret;
}

int RepeatFunc(const char* pName)
{
    int ret;
    if(pName)
    {
        ret = strlen(pName);
        printf("RepeatFunc: %s\n",pName);
    }
    return ret;
}

int CrashFunc(const char* pName)
{
    int ret=0;
    if(pName)
    {
		int *p=0x0;
        ret = strlen(pName);
        printf("CrashFunc: %s\n",pName);
        __try
        {

            puts("in try");

            __try
            {

                puts("in try");

                *p = 13;    // causes an access violation exception;

            }
            __finally
            {

                puts("in finally. termination: ");

                puts(AbnormalTermination() ? "\tabnormal" : "\tnormal");

            }

        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {

            puts("in except");

        }
    }

    return ret;
}