

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


extern "C" int CaptureFile(DWORD processid,const char* bmpfile)
{
    HANDLE hProcess= NULL;
    int ret;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,processid);
    if (hProcess == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }


    return 0;
fail:
    if (hProcess != NULL)
    {
        CloseHandle(hProcess);
    }
    hProcess= NULL;
    return -ret;
}
