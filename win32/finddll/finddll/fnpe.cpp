

#include "fnpe.h"

PVOID __GetModuleBaseAddr(unsigned int processid,const char* pDllName)
{
    HANDLE hsnap=INVALID_HANDLE_VALUE;
    int ret;
    BOOL bret;
    PVOID pBaseAddr = NULL;
    LPMODULEENTRY32 pMEntry=NULL;
#ifdef _UNICODE
    PWCSTR pDllWide=NULL;
    int len;
    pDllWide = new wchar_t[MAX_MODULE_NAME32 + 1];
    assert(pDllWide);

    len = strlen(pDllName);
    bret = MultiByteToWideChar(CP_ACP,NULL,pDllName,-1,pDllWide,len*2);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }
#endif

    hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,processid);
    if(hsnap == INVALID_HANDLE_VALUE)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }


    pMEntry = new MODULEENTRY32;
    assert(pMEntry);
    SetLastError(0);
    for(bret = Module32First(hsnap,pMEntry); bret; bret = Module32Next(hsnap,pMEntry))
    {
#ifdef _UNICODE
        if(wcscmp(pMEntry->szModule,pDllWide)==0)
#else
        if(strcmp(pMEntry->szModule,pDllName)==0)
#endif
        {
            pBaseAddr = pMEntry->modBaseAddr;
            break;
        }
    }


    if(pBaseAddr == NULL)
    {
        ret = ERROR_MOD_NOT_FOUND;
        goto fail;
    }

#ifdef _UNICODE
    if(pDllWide)
    {
        delete [] pDllWide;
    }
    pDllWide = NULL;
#endif

    /*ok ,we find ,so we should close handle*/
    if(pMEntry)
    {
        delete pMEntry;
    }
    pMEntry = NULL;

    if(hsnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hsnap);
    }
    hsnap = INVALID_HANDLE_VALUE;


    return pBaseAddr;
fail:
#ifdef _UNICODE
    if(pDllWide)
    {
        delete [] pDllWide;
    }
    pDllWide = NULL;
#endif
    if(pMEntry)
    {
        delete pMEntry;
    }
    pMEntry = NULL;

    if(hsnap != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hsnap);
    }
    hsnap = INVALID_HANDLE_VALUE;
    SetLastError(ret);
    return NULL;
}

PVOID __Get()
{
}

extern "C" int __GetRemoteProcAddress(unsigned int processid,const char* pDllName,const char* pProcName,PVOID* ppFnAddr)
{
    HANDLE hProcess=NULL;
    PVOID pBaseAddr=NULL,pFuncAddr=NULL;
    int ret;

    pBaseAddr = __GetModuleBaseAddr(processid,pDllName);
    if(pBaseAddr == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

	hProcess = OpenProcess(PROCESS_VM_OPERATION | 
		PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE,processid);
	if (hProcess == NULL)
	{
		ret = GetLastError() ? GetLastError() : 1;
		goto fail;
	}

	/*now we should */

    return 0;
fail:
    if(hProcess)
    {
        CloseHandle(hProcess);
    }
    hProcess = NULL;
    SetLastError(ret);
    return -ret;
}


