// finddll.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


static int st_AnsiArgc;
static char *st_AnsiArgv[]=NULL;

char*** AnsiArgs(int argc,wchar_t* argv[])
{
	int retargv=argc;
	char** ppArgv=NULL;
	int i;

	ppArgv = calloc(sizeof(*ppArgv),(argc+1));
	if (ppArgv == NULL)
	{
		goto fail;
	}

	for (i=0;i<argc;i++)
	{
	}

	return ppArgv;
fail:
	if (ppArgv)
	{
		for(i=0;i<argc;i++)
		{
			if (ppArgv[i])
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



int AnsiMain(int argc,char *argv[])
{

}

int _tmain(int argc, _TCHAR* argv[])
{
	
	return 0;
}

