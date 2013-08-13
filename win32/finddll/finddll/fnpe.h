

#ifndef __FN_PE_H__
#define __FN_PE_H__


extern "C" int __GetRemoteProcAddress(unsigned int processid,const char* pDllName,const char* pProcName,void** ppFnAddr);
extern "C" int __CallRemoteFunc(unsigned int processid,void* pFnAddr,const char* pParam,int timeout,void** ppRetVal);


#endif /*__FN_PE_H__*/

