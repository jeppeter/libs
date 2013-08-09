
#include <windows.h>
#include "dllcri.h"
#include <stdlib.h>
#include <stdio.h>
#include "output_debug.h"

int SnapWholePicture(const char* pFileName)
{
    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                     )
{
    switch(ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		DEBUG_INFO("process attach\n");
		break;
    case DLL_THREAD_ATTACH:
		DEBUG_INFO("thread attach\n");
		break;
    case DLL_THREAD_DETACH:
		DEBUG_INFO("thread detach\n");
		break;
    case DLL_PROCESS_DETACH:
		DEBUG_INFO("process detach\n");
        break;
    }
    return TRUE;
}


