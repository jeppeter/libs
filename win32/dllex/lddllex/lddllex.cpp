// lddllex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

#define  LIB_NAME "dllex.dll"

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE hmod=NULL;

	hmod = LoadLibrary(LIB_NAME);
	if (hmod == NULL)
	{
		fprintf(stderr,"could not load %s\n",LIB_NAME);
		return -3;
	}

	FreeLibrary(hmod);
	return 0;
}

