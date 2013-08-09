
#include <windows.h>
#include "dllcri.h"
#include "output_debug.h"
#include <winerror.h>

#define  POINTER_STATE_GRAB        1
#define  POINTER_STATE_FREE        0
#define  POINTER_STATE_RELEASE     -1
static int st_PointerState;
static CRITICAL_SECTION st_CriticalSection;
static void* st_pPtr=NULL;

int InitializeEnviron()
{
    InitializeCriticalSection(&st_CriticalSection);
    st_PointerState = POINTER_STATE_FREE;
    st_pPtr=NULL;

    return 0;
}


void* GrabPointer()
{
    void* ptr=NULL;
    EnterCriticalSection(&st_CriticalSection);
    if(st_PointerState == POINTER_STATE_FREE)
    {
        ptr = st_pPtr;
        if(ptr)
        {
            st_PointerState = POINTER_STATE_GRAB;
        }
    }
    LeaveCriticalSection(&st_CriticalSection);
    return ptr;
}


int ReleasePointer(void* ptr)
{
    int wait=0;
    BOOL bret;
    int count =0;

    count =0 ;
    do
    {
        wait = 0;
        count =0;
        EnterCriticalSection(&st_CriticalSection);
        if(st_pPtr == ptr)
        {
            if(st_PointerState == POINTER_STATE_FREE)
            {
                count = 1;
                st_pPtr = NULL;
            }
            else if(st_PointerState == POINTER_STATE_GRAB)
            {
                wait = 1;
                st_PointerState = POINTER_STATE_RELEASE;
            }
            else if(st_PointerState == POINTER_STATE_RELEASE)
            {
                wait = 1;
            }
        }
        LeaveCriticalSection(&st_CriticalSection);
        if(wait)
        {
            bret = SwitchToThread();
            if(!bret)
            {
                /*sleep for a while*/
                Sleep(10);
            }
        }
    }
    while(wait);

    return count;
}

void FinalizeEnviron()
{
    void * ptr=NULL;

    ptr = GrabPointer();
    if(ptr)
    {
        ReleasePointer(ptr);
    }
    DeleteCriticalSection(&st_CriticalSection);
}

int SetPointer(void* ptr)
{
    int wait;
    void* pFreePtr=NULL;
	BOOL bret;

    do
    {
		wait = 0;
        EnterCriticalSection(&st_CriticalSection);
        if(st_pPtr != NULL && st_PointerState != POINTER_STATE_FREE)
        {
            wait = 1;
        }
        else if(st_pPtr)
        {
            pFreePtr = st_pPtr;
            st_pPtr = ptr;
            st_PointerState = POINTER_STATE_FREE;
        }
        else
        {
			st_pPtr = ptr;
			st_PointerState = POINTER_STATE_FREE;
        }
        LeaveCriticalSection(&st_CriticalSection);
        if(wait)
        {
            bret = SwitchToThread();
            if(!bret)
            {
                /*sleep for a while*/
                Sleep(10);
            }
        }
    }
    while(wait);

	if (pFreePtr)
	{
		/*ok free*/
	}

	return 0;
}

void FreePointer(void* ptr)
{
    EnterCriticalSection(&st_CriticalSection);
    if(ptr == st_pPtr)
    {
        if(st_PointerState == POINTER_STATE_GRAB ||
                st_PointerState == POINTER_STATE_RELEASE)
        {
            st_PointerState = POINTER_STATE_FREE;
        }
    }
    LeaveCriticalSection(&st_CriticalSection);
    return;
}


int SnapWholePicture(const char* pFileName)
{
    void* pPointer=NULL;
    int ret;
    pPointer = GrabPointer();
    if(pPointer == NULL)
    {
        ret = ERROR_SCOPE_NOT_FOUND;
        goto fail;
    }

    FreePointer(pPointer);
    return 0;
fail:
    if(pPointer)
    {
        FreePointer(pPointer);
    }
    pPointer = NULL;
    SetLastError(ret);
    return -ret;
}


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                     )
{
    int ret;
    switch(ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DEBUG_INFO("process attach\n");
        ret = InitializeEnviron();
        if(ret < 0)
        {
            return FALSE;
        }
        break;
    case DLL_THREAD_ATTACH:
        DEBUG_INFO("thread attach\n");
        break;
    case DLL_THREAD_DETACH:
        DEBUG_INFO("thread detach\n");
        break;
    case DLL_PROCESS_DETACH:
        DEBUG_INFO("process detach\n");
        FinalizeEnviron();
        break;
    }
    return TRUE;
}


