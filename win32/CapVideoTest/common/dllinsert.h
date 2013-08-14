#ifndef __DLL_INSERT_H__
#define __DLL_INSERT_H__

#include <windows.h>

extern "C" int LoadInsert(const char* pExec,const char* pDllFullName,const char* pDllName);
extern "C" int CaptureFile(DWORD processid,const char* pDllName,const char* pFuncName,const char* bmpfile);

#endif  /*__DLL_INSERT_H__*/