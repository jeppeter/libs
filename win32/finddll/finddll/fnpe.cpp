

#include "fnpe.h"

#include <stdio.h>
#include <stdlib.h>
#include <TlHelp32.h>
#include <windows.h>
#include <assert.h>

#define  DEBUG_INFO(fmt,...) \
do\
{\
	printf("%s:%d\t",__FILE__,__LINE__);\
    printf(fmt,__VA_ARGS__);\
} while(0)


PVOID __GetModuleBaseAddr(unsigned int processid,const char* pDllName)
{
    HANDLE hsnap=INVALID_HANDLE_VALUE;
    int ret;
    BOOL bret;
    PVOID pBaseAddr = NULL;
    LPMODULEENTRY32 pMEntry=NULL;
#ifdef _UNICODE
    LPWSTR pDllWide=NULL;
    int len;
    len = strlen(pDllName);
    pDllWide = new wchar_t[len + 1];
    assert(pDllWide);
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
	pBaseAddr = NULL;
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



BOOL __ReallocateSize(PVOID* ppBuffer,int buflen,int*pBufsize)
{
    PVOID pBuffer = NULL;
    int bufsize=*pBufsize;
    BOOL bret;

    if(buflen > 0)
    {
        pBuffer = VirtualAllocEx(GetCurrentProcess(),NULL,buflen,MEM_COMMIT,PAGE_READWRITE);
        if(pBuffer == NULL)
        {
            return FALSE;
        }
    }

    if(*ppBuffer)
    {
        bret = VirtualFreeEx(GetCurrentProcess(),*ppBuffer,bufsize,MEM_RELEASE);
        if(!bret)
        {
            DEBUG_INFO("Free[0x%p] %d error(%d)\n",*ppBuffer,bufsize,GetLastError());
        }
    }
    *ppBuffer = pBuffer;
    *pBufsize = buflen;
    return TRUE;
}


DWORD __GetExportTableAddr(HANDLE hProcess,PVOID pModBase)
{
    PIMAGE_DOS_HEADER pDosHeader=NULL;
    PIMAGE_FILE_HEADER pFileHeader=NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptional32=NULL;
    PIMAGE_NT_HEADER pNtHeader=NULL;
    PIMAGE_DATA_DIRECTORY pDataDir=NULL;
    PVOID pBuffer=NULL;
    DWORD pTableAddr=NULL;
    int bufsize=0;
    int buflen=0;
    int ret;
    BOOL bret;
    PVOID pRPtr=NULL;
    SIZE_t rsize;
	bufsize = 0;
    buflen = sizeof(*pDosHeader);
    if(buflen > bufsize)
    {
        bufsize = buflen;
    }
    buflen = sizeof(*pNtHeader);
    if(buflen > bufsize)
    {
        bufsize = buflen;
    }

	bret = __ReallocateSize(&pBuffer,buflen,&bufsize);
	if (!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
		DEBUG_INFO("\n");
        goto fail;
    }

    /*now we should read the memory of DOS header*/
    pRPtr = pModBase;
    pDosHeader = pBuffer;
    bret = ReadProcessMemory(hProcess,pRPtr,pDosHeader,sizeof(*pDosHeader),&rsize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }
    if(rsize != sizeof(*pDosHeader))
    {
        ret = ERROR_INVALID_BLOCK;
        goto fail;
    }

    /*header of MZ*/
    if(pDosHeader->e_magic != 0x5a4d)
    {
        ret = ERROR_INVALID_BLOCK;
        goto fail;
    }

    pRPtr += pDosHeader->e_lfanew;
    pNtHeader = pBuffer;
    bret = ReadProcessMemory(hProcess,pRPtr,pNtHeader,sizeof(*pNtHeader),&rsize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    if(rsize != sizeof(*pNtHeader))
    {
        ret = ERROR_INVALID_BLOCK;
        goto fail;
    }

    /*header of PE\0\0*/
    if(pNtHeader->Signature != 0x00004550)
    {
        ret = ERROR_INVALID_BLOCK;
        goto fail;
    }

    /*now we get the PE header so we should get the export table*/
    pOptional32 = &(pNtHeader->OptionalHeader);
    if(pOptional32->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        ret = ERROR_INVALID_BLOCK;
        goto fail;
    }

    pDataDir = &(pOptional32->DataDirectory[0]);

    pTableAddr = pDataDir->VirtualAddress;
	__ReallocateSize(&pBuffer,0,&bufsize);

    return pTableAddr;


fail:
	__ReallocateSize(&pBuffer,0,&bufsize);
    SetLastError(ret);
    return NULL;
}

int __ReadName(HANDLE hProcess,PVOID pModBase,DWORD rva,PVOID *ppBuffer,int skipbyte,int* pBufSize)
{
    int done;
    int readlen ;
    SIZE_T rsize;
    PVOID pCurPtr=NULL;
    PVOID pTmpBuf=NULL;
    PVOID pCurAddr;
    int tmpsize=0,tmplen=0;
    BOOL bret;
    int ret;
    int i;
    unsigned char* pChar=NULL;

    done = 0;
    readlen = 0;
    /*now first to read for the readnum*/

    pCurPtr = *ppBuffer;
    pCurAddr = pModBase + rva+skipbyte;

    for(;;)
    {
        if(readlen >= *pBufSize)
        {
            /*we will expand the buffer size*/
            tmplen = (*pBufSize) << 1 ? (*pBufSize) << 1 : 0x1000;
            assert(tmpsize == 0);
            assert(pTmpBuf == NULL);
            bret = __ReallocateSize(&pTmpBuf,tmplen,&tmpsize);
            if(!bret)
            {
                ret = GetLastError() ? GetLastError() : 1;
                goto fail;
            }

            /*now to copy memory*/
            if(readlen > 0)
            {
                memcpy(pTmpBuf,*ppBuffer,readlen);
            }
            /*free buffer*/
            __ReallocateSize(ppBuffer,0,pBufSize);
            /*to replace the buffer*/
            *ppBuffer = pTmpBuf;
            *pBufSize = tmpsize;
            pTmpBuf = NULL;
            tmpsize = 0;
            pCurPtr = (*ppBuffer + readlen);
        }

        bret = ReadProcessMemory(hProcess,pCurAddr,pCurPtr,1,&rsize);
        if(!bret)
        {
            ret = GetLastError() ? GetLastError() : 1;
            goto fail;
        }
        if(rsize != 1)
        {
            ret = ERROR_INVALID_BLOCK;
            goto fail;
        }
        if(*pCurPtr == '\0')
        {
            done =1;
            break;
        }
        pCurPtr += 1;
        readlen += 1;
        pCurAddr += 1;
    }

    assert(pTmpBuf == NULL);
    return readlen;

fail:
    __ReallocateSize(&pTmpBuf,0,&tmpsize);
    SetLastError(ret);
    return -ret;
}


PVOID __GetProcAddr(HANDLE hProcess,PVOID pModBase,DWORD tablerva,const char* pDllName,const char* pProcName)
{
    int buflen=0,bufsize=0;
    PVOID pBuffer=NULL;
    int namelen = 0,namesize=0;
    PVOID pNameBuf=NULL;

    int ret;
    SIZE_T rsize;
    BOOL bret;
    int fntablesize=0,fntablelen=0;
    PVOID pFnAddr = NULL;
    DOWRD *pFnTable=NULL;
    int nametablesize=0,nametablelen=0;
    DWORD *pNameTable=NULL;
    PIMAGE_EXPORT_DIRECTORY pExportDir=NULL;
    int i,j,readlen,findidx;
    DWORD rva;
    PVOID pCurPtr;

    buflen = 0x1000;
    bret = __ReallocateSize(&pBuffer,buflen,&bufsize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }
    namelen = 0x1000;
    bret = __ReallocateSize(&pNameBuf,namelen,&namesize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }
    buflen = sizeof(*pExportDir);
    if(buflen > bufsize)
    {
        bret = __ReallocateSize(&pBuffer,buflen,&bufsize);
        if(!bret)
        {
            ret = GetLastError() ? GetLastError() : 1;
            goto fail;
        }
    }

    /*now we should get the table address */
    pExportDir = pBuffer;
    bret = ReadProcessMemory(hProcess,pModBase + tablerva,pExportDir,sizeof(*pExportDir),&rsize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }
    if(rsize != sizeof(*pExportDir))
    {
        ret = ERROR_INVALID_BLOCK;
        goto fail;
    }

    /*now we should compare module name ,this will give it check*/
    rva = pExportDir->Name;
    readlen = __ReadName(hProcess,pModBase,rva,&pNameBuf,0,&namesize);
    if(readlen < 0)
    {
        ret = -readlen;
        goto fail;
    }

    /*now to compare the name*/
    if(strcmp(pNameBuf,pDllName)!= 0)
    {
        ret = ERROR_INVALID_NAME;
        goto fail;
    }

    /*now we should get for the functions scanning*/
    fntablelen = sizeof(DWORD)*pExportDir->NumberOfFunctions;
    bret = __ReallocateSize(&pFnTable,fntablelen,&fntablesize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    rva = pExportDir->AddressOfFunctions;
    bret = ReadProcessMemory(hProcess,pModBase+rva ,pFnTable,fntablelen,&rsize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    if(rsize != fntablelen)
    {
        ret = ERROR_INVALID_BLOCK;
        goto fail;
    }

	/*name table is less than the functions ,if it has no name ,the value is 0 ,so we should set NumberOfFunctions not NumberOfNames*/
    nametablelen = sizeof(DWORD)*pExportDir->NumberOfFunctions;
    bret = __ReallocateSize(&pNameTable,nametablelen,&nametablesize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    rva = pExportDir->AddressOfNames;
    bret = ReadProcessMemory(hProcess,pModBase+rva ,pNameTable,nametablelen,&rsize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    if(rsize != nametablelen)
    {
        ret = ERROR_INVALID_BLOCK;
        goto fail;
    }

    findidx = -1;
    /*now to get the name and */
    for(i = 0; i<pExportDir->NumberOfFunctions; i++)
    {
        /*now to test if */
        if(pNameTable[i] == 0)
        {
            continue;
        }

        /*now it is not has name ,so we should search for it*/
        rva = pNameTable[i];
        ret = __ReadName(hProcess,pModBase,rva,&pNameBuf,2,&namesize);
        if(ret < 0)
        {
            ret = GetLastError() ? GetLastError() : 1;
            goto fail;
        }

        /*now to compare the jobs*/
        if(strcmp(pNameBuf,pProcName)==0)
        {
            findidx = i;
            break;
        }
    }

    if(findidx < 0)
    {
        ret = ERROR_INVALID_NAME;
        goto fail;
    }

    /*ok we should set the function pointer*/
    pFnAddr = pModBase + pFnTable[findidx];

    __ReallocateSize(&pNameTable,0,nametablesize);
    __ReallocateSize(&pFnTable,0,fntablesize);
    __ReallocateSize(&pNameBuf,0,&namesize);
    __ReallocateSize(&pBuffer,0,&bufsize);

    return pFnAddr;
fail:
    __ReallocateSize(&pNameTable,0,nametablesize);
    __ReallocateSize(&pFnTable,0,fntablesize);
    __ReallocateSize(&pNameBuf,0,&namesize);
    __ReallocateSize(&pBuffer,0,&bufsize);
    SetLastError(ret);
    return NULL;
}

extern "C" int __GetRemoteProcAddress(unsigned int processid,const char* pDllName,const char* pProcName,PVOID* ppFnAddr)
{
    HANDLE hProcess=NULL;
    PVOID pBaseAddr=NULL,pFuncAddr=NULL;
    DWORD exporttablerva = 0;
    int ret;

    pBaseAddr = __GetModuleBaseAddr(processid,pDllName);
    if(pBaseAddr == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    hProcess = OpenProcess(PROCESS_VM_OPERATION |
                           PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE,processid);
    if(hProcess == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    pBaseAddr= __GetModuleBaseAddr(unsigned int processid,const char * pDllName);
    if(pBaseAddr == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    exporttablerva = __GetExportTableAddr(hProcess,pBaseAddr);
    if(exporttablerva == 0)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }

    pFuncAddr = __GetProcAddr(hProcess,pBaseAddr,exporttablerva,pDllName,pProcName);
    if(pFuncAddr == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }
    *ppFnAddr = pFuncAddr;

    assert(hProcess != NULL);
    CloseHandle(hProcess);


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


