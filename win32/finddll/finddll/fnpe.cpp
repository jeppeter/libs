

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

PVOID __GetExportTableAddr(HANDLE hProcess,PVOID pModBase)
{
    PIMAGE_DOS_HEADER pDosHeader=NULL;
    PIMAGE_FILE_HEADER pFileHeader=NULL;
    PIMAGE_OPTIONAL_HEADER32 pOptional32=NULL;
    PIMAGE_NT_HEADER pNtHeader=NULL;
    PIMAGE_DATA_DIRECTORY pDataDir=NULL;
    PVOID pBuffer=NULL,pTableAddr=NULL;
    int bufsize=0;
    int buflen=0;
    int ret;
    BOOL bret;
    PVOID pRPtr=NULL;
    SIZE_t rsize;
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

    pBuffer = VirtualAllocEx(GetCurrentProcess(),NULL,bufsize,MEM_COMMIT,PAGE_READWRITE);
    if(pBuffer == NULL)
    {
        ret = GetLastError() ? GetLastError() : 1;
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

    if(pBuffer)
    {
        bret = VirtualFreeEx(GetCurrentProcess(),pBuffer,bufsize,MEM_RELEASE);
        if(!bret)
        {
            DEBUG_INFO("free 0x%p [0x%08x] size error %d\n",pBuffer,bufsize,GetLastError());
        }
    }
    pBuffer = NULL;

    return pTableAddr;


fail:
    if(pBuffer)
    {
        bret = VirtualFreeEx(GetCurrentProcess(),pBuffer,bufsize,MEM_RELEASE);
        if(!bret)
        {
            DEBUG_INFO("free 0x%p [0x%08x] size error %d\n",pBuffer,bufsize,GetLastError());
        }
    }
    pBuffer = NULL;
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

int __ReadName(HANDLE hProcess,PVOID pModBase,DWORD rva,PVOID *ppBuffer,int readnum,int* pBufSize)
{
    int done;
    int readlen ;
    SIZE_T rsize;
    PVOID pCurPtr=NULL;
    PVOID pTmpBuf=NULL;
    PVOID pCurAddr;
    int tmpnamesize=*pBufSize,tmpnamelen=0;
    int tmpsize=0,tmplen=0;
    BOOL bret;
    int ret;
    int i;
    unsigned char* pChar=NULL;

    done = 0;
    readlen = 0;
    /*now first to read for the readnum*/

    if(tmpnamesize < readnum)
    {
        bret = __ReallocateSize(ppBuffer,readnum,pBufSize);
        if(!bret)
        {
            ret = GetLastError() ? GetLastError() : 1;
            goto fail;
        }
    }
    pCurPtr = *ppBuffer;
    pCurAddr = pModBase + rva;
    tmpnamesize = *pBufSize;

    if(readnum > 0)
    {
        bret = ReadProcessMemory(hProcess,pCurAddr,pCurPtr,readnum,&rsize);
        if(!bret)
        {
            ret = GetLastError() ? GetLastError() : 1;
            goto fail;
        }

        /*now we test if we have the data read ok*/
        pChar = pCurPtr;
        for(i=0; i<rsize; i++)
        {
            if(pChar[i] == '\0')
            {
                /*we got the end of char ,so return the length of chars*/
                return (i+1);
            }
        }

        readlen += rsize;
        pCurPtr += rsize;
        pCurAddr += rsize;
    }

    for(;;)
    {
        if(readlen >= *pBufSize)
        {
            /*we will expand the buffer size*/
            tmpnamelen = (*pBufSize) << 1 ? (*pBufSize) << 1 : 0x1000;
            assert(tmpnamesize == 0);
            assert(pTmpName == NULL);
            bret = __ReallocateSize(&pTmpBuf,tmpnamelen,&tmpsize);
            if(!bret)
            {
                ret = GetLastError() ? GetLastError() : 1;
                goto fail;
            }

            /*now to copy memory*/
            if(readlen > 0)
            {
                memcpy(pTmpName,*ppBuffer,readlen);
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
    int namelen = 0,namesize=0,tmpnamelen=0,tmpnamesize=0;
    PVOID pNameBuf=NULL,pTmpName=NULL;

    int ret;
    SIZE_T rsize;
    BOOL bret;
    int fntablesize=0,fntablelen=0;
    PVOID pFnAddr = NULL,pFnTable=NULL;
    int nametablesize=0,nametablelen=0;
    PVOID pNameTable=NULL;
    PIMAGE_EXPORT_DIRECTORY pExportDir=NULL;
    int i,j,done,readlen;
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
    bret = ReadProcessMemory(hProcess,pModBase + tablerva,pExportDir,buflen,&rsize);
    if(!bret)
    {
        ret = GetLastError() ? GetLastError() : 1;
        goto fail;
    }
    if(rsize != buflen)
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

    nametablelen = sizeof(DWORD)*pExportDir->NumberOfNames;
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

    /*now to get the name and */
    for(i = 0; i<pExportDir->NumberOfNames; i++)
    {
		
    }







    __ReallocateSize(&pNameTable,0,nametablesize);

    __ReallocateSize(&pFnTable,0,fntablesize);
    __ReallocateSize(&pTmpName,0,&tmpnamesize);
    __ReallocateSize(&pNameBuf,0,&namesize);
    __ReallocateSize(&pBuffer,0,&bufsize);

    return pFnAddr;
fail:
    __ReallocateSize(&pNameTable,0,nametablesize);
    __ReallocateSize(&pFnTable,0,fntablesize);
    __ReallocateSize(&pTmpName,0,&tmpnamesize);
    __ReallocateSize(&pNameBuf,0,&namesize);
    __ReallocateSize(&pBuffer,0,&bufsize);
    SetLastError(ret);
    return NULL;
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
    if(hProcess == NULL)
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


