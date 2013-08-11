

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
    int fulllen=0,dlllen=0;
#ifdef _UNICODE
    wchar_t *pFullWide=NULL;
    wchar_t *pDllWide=NULL;
    wchar_t *pFullDllWide = NULL;
    pFullWide = new wchar_t[MAX_MODULE_NAME32 + 2+MAX_PATH];
    pDllWide = new wchar_t[MAX_MODULE_NAME32 + 2];
    fulllen = strlen(pDllFullName);
    dlllen = strlen(pDllName);
    pFullDllWide = new wchar_t[fulllen + dlllen + 3];
    bret =MultiByteToWideChar(CP_ACP,NULL,pDllFullName,-1,pFullWide,fulllen*2);
    if (!bret)
    {
        ret = 0;
        goto out;
    }
    bret =MultiByteToWideChar(CP_ACP,NULL,pDllFullName,-1,pDllWide,fulllen*2);
    if (!bret)
    {
        ret = 0;
        goto out;
    }
#else
    char* pFullDllAnsi=NULL;

    fulllen = strlen(pDllFullName);
    dlllen = strlen(pDllName);
    pFullDllAnsi = new char[MAX_MODULE_NAME32 + 2+MAX_PATH];
#endif
    pModule32 = new MODULEENTRY32;
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
        /*now we should copy the full name copy name*/
        wcscpy(pFullDllWide,pModule32->szExePath);
        wcscat(pFullDllWide,L"\\");
        wcscat(pFullDllWide,pModule32->szModule);
        if (wcscmp(pFullDllWide ,pFullWide )== 0
                && wcscmp(pDllWide,pModule32->szModule) == 0)
        {
            ret = 1;
            break;
        }
#else
        strcpy(pFullDllAnsi,pModule32->szExePath);
        strcat(pFullDllAnsi,"\\");
        strcat(pFullDllAnsi,pModule32->szModule);
        if (strcmp(pFullDllAnsi,pDllFullName) == 0 &&
                strcmp(pDllName,pModule32->szModule) == 0)
        {
            ret = 1;
            break;
        }
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
    if (pModule32)
    {
        delete pModule32;
    }
#ifdef _UNICODE
    if (pDllWide)
    {
        delete [] pDllWide;
    }
    pDllWide = NULL;
    if (pFullWide)
    {
        delete [] pFullWide;
    }
    pFullWide = NULL;
    if (pFullDllWide)
    {
        delete [] pFullDllWide;
    }
    pFullDllWide = NULL;
#else
    if (pFullDllAnsi)
    {
        delete [] pFullDllAnsi;
    }
    pFullDllAnsi = NULL;
#endif
    return ret;
}

extern "C" int CaptureFile(DWORD processid,const char* bmpfile,const char* pDllName,const char* pDllFullName)
{
    HANDLE hProcess= NULL;
    int ret;
    PVOID pParam=NULL;
    DWORD paramsize=0;
    int loadlib= 0;
    PVOID pLoadLibraryFn = NULL,pFreeLibraryFn=NULL,pCaptureFn=NULL;

    /*not */
    ret = __IsModuleIn(processid,pDllName,pDllFullName);
    if (ret <= 0)
    {
        SetLastError(ERROR_MOD_NOT_FOUND);
        return -ERROR_MOD_NOT_FOUND;
    }

    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,processid);
    if (hProcess == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }


    return 0;
fail:
    if (loadlib)
    {
        assert(pLoadLibraryFn);
    }
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
