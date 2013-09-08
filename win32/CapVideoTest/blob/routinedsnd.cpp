

#include <dsound.h>
#include "routinedsnd.h"
#include "..\\common\\output_debug.h"
#include <assert.h>
#include "..\\detours\\detours.h"


#pragma comment(lib,"dsound.lib")

static HRESULT(WINAPI *DirectSoundCreate8Next)(
    LPCGUID lpcGuidDevice,
    LPDIRECTSOUND8 * ppDS8,
    LPUNKNOWN pUnkOuter
) = DirectSoundCreate8;

HRESULT WINAPI DirectSoundCreate8CallBack(LPCGUID lpcGuidDevice,
        LPDIRECTSOUND8 * ppDS8,
        LPUNKNOWN pUnkOuter)
{
    HRESULT hr;

    hr = DirectSoundCreate8Next(lpcGuidDevice,ppDS8,pUnkOuter);
    DEBUG_INFO("call DirectSoundCreate8CallBack return hr 0x%08x\n",hr);
    return hr;
}

int RoutineDetourDSound(void)
{
    assert(DirectSoundCreate8Next);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)&DirectSoundCreate8Next,DirectSoundCreate8CallBack);
    DetourTransactionCommit();
	DEBUG_INFO("dsound\n");
    return 0;
}

void RoutineClearDSound(void)
{
    return;
}

