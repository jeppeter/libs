#define  DLL1_API _declspec(dllexport)
//定义DLL1_API 为_declspec(dllexport) ，并且解决在不同语言调用下的名字匹配问题
#include <iostream>
#include <windows.h>
//#include "Dll1.h"
using namespace std;

#define  DEBUG_INFO(fmt,...) DebugOutString(__FILE__,__LINE__,fmt,__VA_ARGS__)

void DebugOutString(const char* file,int lineno,const char* fmt,...)
{
	char* pFmt=NULL;
	char* pLine=NULL;
	char* pWhole=NULL;
	va_list ap;

	pFmt = new char[2000];
	pLine = new char[2000];
	pWhole = new char[4000];

	_snprintf_s(pLine,2000,1999,"%s:%d\t",file,lineno);
	va_start(ap,fmt);
	_vsnprintf_s(pFmt,2000,1999,fmt,ap);
	strcpy(pWhole,pLine);
	strcat_s(pWhole,4000,pFmt);
	OutputDebugString(pWhole);
	delete [] pFmt;
	delete [] pLine;
	delete [] pWhole;
	return ;
}

DLL1_API int dllex_add(int a,int b)   //实现两个整数相加
{
    return a+b;
}
DLL1_API int dllex_subtract(int a,int b)   //实现两个整数相减
{
    return a-b;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved
)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DEBUG_INFO("PROCESS ATTACH %p\n",hinstDLL);
		break;
	case DLL_PROCESS_DETACH:
		DEBUG_INFO("PROCESS DETACH %p\n",hinstDLL);
		break;
	case DLL_THREAD_ATTACH:
		DEBUG_INFO("THREAD ATTACH %p\n",hinstDLL);
		break;
	case DLL_THREAD_DETACH:
		DEBUG_INFO("THREAD DETACH %p\n",hinstDLL);
		break;
	default:
		break;
	}

	return TRUE;
}

