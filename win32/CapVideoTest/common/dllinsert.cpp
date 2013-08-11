

#include "dllinsert.h"
#include "..\\detours\\detours.h"


int __LoadInsert(const char* pExec,const char* pDllFullName,const char* pDllName)
{
    BOOL bret;
    PROCESS_INFORMATION pi = {0};
    STARTUPINFO si = {0};
    int ret;
    si.cb = sizeof(si);
#ifdef _UNICODE
    LPWSTR pExecWide=NULL;
    int execnamelen=0;
    if (pExec == NULL )
    {
        ret = ERROR_INVALID_DATA;
        return -ret;
    }
    execnamelen = strlen(pExec);
    pExecWide = new wchar_t[execnamelen+1];

    bret = MultiByteToWideChar(CP_ACP,NULL,pFmtStr,-1,pWide,(execnamelen)*2);
    if (!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        delete [] pExecWide;
        pExecWide =NULL;
        return -ret;
    }

    bret = DetourCreateProcessWithDllW(pExecWide,NULL,NULL,NULL,TRUE,CREATE_DEFAULT_ERROR_MODE
                                       NULL,NULL,
                                       &si,&pi,pDllFullName,pDllName,NULL);


    delete [] pExecWide ;
    pExecWide = NULL;

#else
    bret = DetourCreateProcessWithDllA(pExec,NULL,NULL,NULL,TRUE,CREATE_DEFAULT_ERROR_MODE
                                       NULL,NULL,
                                       &si,&pi,pDllFullName,pDllName,NULL);
#endif

    ret = 0;
    if (!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
    }
    return -ret;
}

extern "C" int LoadInsert(const char* pExec,const char* pDllFullName,const char* pDllName)
{

    return __LoadInsert(pExec,pDllFullName,pDllName);
}


int __IsModuleIn(DWORD processid,const char* pDllName,const char* pDllFullName)
{
    HANDLE hsnap=INVALID_HANDLE_VALUE;
    PMODULEENTRY32 *pModule32=NULL;
    HANDLE hProcess=NULL;
    int ret=0;
    BOOL bret;
    TCHAR *pFullName=NULL;
#ifdef _UNICODE
    char* pFullAnsi=NULL;
    pFullAnsi = new char[MAX_MODULE_NAME32 + 1+MAX_PATH];
#endif
    pModule32 = new MODULEENTRY32;
    pFullName = new TCHAR[MAX_MODULE_NAME32 + 1+MAX_PATH];
    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,processid);
    if (hProcess == NULL)
    {
        ret = 0;
        goto out;
    }

    hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,processid);
    if (hsnap == INVALID_HANDLE_VALUE)
    {
        ret = 0;
        goto out;
    }

    for (bret = Module32First(hsnap,pModule32); bret ; bret = Module32Next(hsnap,pModule32))
    {
#ifdef _UNICODE
	/*now we should copy the full name*/
	bret = 
#else
#endif
    }



out:
    if (hsnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hsnap);
    }
    hsnap = INVALID_HANDLE_VALUE;
    if (hProcess != NULL)
    {
        CloseHandle(hProcess);
    }
    hProcess = NULL;
    delete pModule32;
    delete [] pFullName ;
#ifdef _UNICODE
    delete [] pFullAnsi;
#endif
    return 0;
}

extern "C" int CaptureFile(DWORD processid,const char* bmpfile,const char* pDllName,const char* pDllFullName)
{
    HANDLE hProcess= NULL;
    int ret;
    PVOID pParam=NULL;
    DWORD paramsize=0;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,processid);
    if (hProcess == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }


    return 0;
fail:
    if (pParam)
    {
        assert(hProcess != NULL);
        VirtualFreeEx(hProcess,pParam,paramsize,MEM_RELEASE);
    }
    pParam = NULL;
    paramsize = 0;
    if (hProcess != NULL)
    {
        CloseHandle(hProcess);
    }
    hProcess= NULL;
    return -ret;
}
