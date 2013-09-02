#ifndef __DLL_INSERT_H__
#define __DLL_INSERT_H__

#include <windows.h>

extern "C" int LoadInsert(const char* pExec,const char* pCommandLine,const char* pDllFullName,const char* pDllName);
extern "C" int CaptureFile(DWORD processid,const char* pDllName,const char* pFuncName,const char* bmpfile);
extern "C" int __GetRemoteProcAddress(unsigned int processid,const char* pDllName,const char* pProcName,void** ppFnAddr);
extern "C" int __CallRemoteFunc(unsigned int processid,void* pFnAddr,const char* pParam,int timeout,void** ppRetVal);
extern "C" int D3DHook_HookProcess(HANDLE hProc, char * strDllName);
extern "C" int D3DHook_CaptureImageBuffer(HANDLE hProc,char* strDllName,char * data, int len, int * format, int * width, int * height);

#endif  /*__DLL_INSERT_H__*/