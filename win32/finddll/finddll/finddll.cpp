// finddll.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <varargs.h>
#include "fnpe.h"

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

void Usage(int ec,const char* fmt,...)
{
    va_list ap;
    FILE* fp=stderr;
    if(ec == 0)
    {
        fp = stdout;
    }
    if(fmt)
    {
        va_start(ap,fmt);
        vfprintf(fp,fmt,ap);
        fprintf(fp,"\n");
    }

    fprintf(fp,"finddll [OPTIONS]\n");
    fprintf(fp,"\t-h|--help                   : to display help message\n");
    fprintf(fp,"\t-p|--pid   pid            : to specify process id\n");
    fprintf(fp,"\t-d|--dll   dllname     : to specify dll name\n");
    fprintf(fp,"\t-f|--func  func          :  to specify function name\n");
    fprintf(fp,"\t-P|--param param    : to set for parameter call function\n");

    exit(ec);
}

static int st_ProcessPid = 0;
static char* st_DllName=NULL;
static char* st_FuncName=NULL;
static char* st_Param=NULL;

void ParseParam(int argc,char* argv[])
{
    int i;

    for(i=1; i<argc; i++)
    {
        if(strcmp(argv[i],"-h")==0||
                strcmp(argv[i],"--help") == 0)
        {
            Usage(0,NULL);
        }
        else if(strcmp(argv[i],"-p") == 0 ||
                strcmp(argv[i],"--pid") == 0)
        {
            if((i+1)>=argc)
            {
                Usage(3,"%s need one param",argv[i]);
            }
            st_ProcessPid = atoi(argv[i+1]);
            i++;
        }
        else if(strcmp(argv[i],"-f") == 0 ||
                strcmp(argv[i],"--func") == 0)
        {
            if((i+1)>=argc)
            {
                Usage(3,"%s need one param",argv[i]);
            }
            st_FuncName = argv[i+1];
            i ++;
        }
        else if(strcmp(argv[i],"-d") == 0 ||
                strcmp(argv[i],"--dll") == 0)
        {
            if((i+1)>=argc)
            {
                Usage(3,"%s need one param",argv[i]);
            }
            st_DllName = argv[i+1];
            i ++;
        }
        else if(strcmp(argv[i],"-P") == 0 ||
                strcmp(argv[i],"--param") == 0)
        {
            if((i+1)>=argc)
            {
                Usage(3,"%s need one param",argv[i]);
            }
            st_Param= argv[i+1];
            i ++;
        }
        else
        {
            Usage(3,"not recognize param %s",argv[i]);
        }
    }

    if(st_ProcessPid == 0)
    {
        Usage(3,"please specify -p|--pid");
    }

    if(st_FuncName == NULL)
    {
        Usage(3,"please specify -f|--func");
    }

    if(st_DllName == NULL)
    {
        Usage(3,"please specify -d|--dll");
    }

    if(st_Param == NULL)
    {
        Usage(3,"please specify -P|--param");
    }
    return;
}

int AnsiMain(int argc,char *argv[])
{
    PVOID pFuncAddr;
    int ret;
    PVOID pRetVal;
    ParseParam(argc,argv);

    ret =  __GetRemoteProcAddress(st_ProcessPid,st_DllName,st_FuncName,&pFuncAddr);
    if(ret < 0)
    {
        return ret;
    }

    ret = __CallRemoteFunc(st_ProcessPid,pFuncAddr,st_Param,2,&pRetVal);
    if(ret < 0)
    {
        return ret;
    }
    return ret;
}

int _tmain(int argc, _TCHAR* argv[])
{
    int ret;
#ifdef _UNICODE
    char** ppArgv=NULL;
    int i;
    ppArgv = AnsiArgs(argc,argv);
    if(ppArgv == NULL)
    {
        return -ERROR_NOT_ENOUGH_MEMORY;
    }
    ret = AnsiMain(argc,ppArgv);
#else
    ret = AnsiMain(argc,argv);
#endif

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

