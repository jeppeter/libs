// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "../rpcdispatch/rpcdisp.h"
#include "../rpcdispatch/kdcomdisp.h"
#include "../rpcdispatch/reporter.h"
#include <bzscmn/file.h>

HINSTANCE g_hThisDll;
StatusReporter *g_pReporter = NULL;

void InitializeRpcDispatcher();
void CleanupRpcDispatcher();


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	wchar_t szDllName[MAX_PATH];
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetModuleFileName(NULL, szDllName, _countof(szDllName));
		if (!BazisLib::FilePath(szDllName).GetFileName().icompare(L"VirtualBox.exe"))
		{
			g_hThisDll = hModule;
			g_pReporter = new StatusReporter();
			InitializeRpcDispatcher();
		}
		break;
	case DLL_PROCESS_DETACH:
		CleanupRpcDispatcher();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

