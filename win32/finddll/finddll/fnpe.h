

#ifndef __FN_PE_H__
#define __FN_PE_H__

#include <windows.h>

extern "C" int __GetRemoteProcAddress(unsigned int processid,const char* pDllName,const char* pProcName,PVOID* ppFnAddr);


#endif /*__FN_PE_H__*/

