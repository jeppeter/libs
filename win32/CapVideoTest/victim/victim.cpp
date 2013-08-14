// victim.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

#ifdef _UNICODE
#include <stdio.h>
#include <malloc.h>

char** AnsiArgs(int argc,wchar_t* argv[])
{
    char** ppArgv=NULL;
    int i,ret;
    int len;

    ppArgv =(char**) calloc(sizeof(*ppArgv),(argc+1));
    if(ppArgv == NULL)
    {
        goto fail;
    }

    for(i=0; i<argc; i++)
    {
        if(argv[i])
        {
            len = wcslen(argv[i]);
            ppArgv[i] = (char*)calloc(sizeof(char),(len+1)*2);
            if(ppArgv[i] == NULL)
            {
                goto fail;
            }
            SetLastError(0);
            ret = WideCharToMultiByte(CP_ACP,0,argv[i],len,ppArgv[i],(len+1)*2,NULL,NULL);
            if(ret == 0 && GetLastError())
            {
                ret = GetLastError();
                goto fail;
            }
        }
    }

    /*all is ok*/

    return ppArgv;
fail:
    if(ppArgv)
    {
        for(i=0; i<argc; i++)
        {
            if(ppArgv[i])
            {
                free(ppArgv[i]);
            }
            ppArgv[i]=NULL;
        }
        free(ppArgv);
    }
    ppArgv =NULL;

    return NULL;
}
#endif




int _tmain(int argc, _TCHAR* argv[])
{
    int ret;
	int i;
#ifdef _UNICODE
    char** ppArgv=NULL;
    ppArgv = AnsiArgs(argc,argv);
    if(ppArgv == NULL)
    {
        return -ERROR_NOT_ENOUGH_MEMORY;
    }
	for (i=0;i<argc;i++)
	{
		printf("[%d] param %s\n",i,ppArgv[i]);
	}
#else
	for(i=0;i<argc;i++)
	{
		printf("[%d] param %s\n",i,argv[i]);
	}
#endif
	while(1)
	{
		Sleep(1000);
	}

#ifdef _UNICODE
    if(ppArgv)
    {
        for(i=0; i<argc; i++)
        {
            if(ppArgv[i])
            {
                free(ppArgv[i]);
            }
            ppArgv[i]=NULL;
        }
        free(ppArgv);
    }
    ppArgv=NULL;
#endif
    return ret;
}

