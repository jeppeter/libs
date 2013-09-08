
#include <xaudio2.h>
#include "routinexaudio2.h"
#include "..\\common\\output_debug.h"
#include <assert.h>
#include <Objbase.h>
#include "..\\detours\\detours.h"


//#pragma comment(lib,"Xaudio2.lib")

static HRESULT(*XAudio2CreateNext)(
    IXAudio2 **ppXAudio2,
    UINT32 Flags,
    XAUDIO2_PROCESSOR XAudio2Processor
) = XAudio2Create;

static HRESULT(__stdcall *CoCreateInstanceNex)(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv
) = CoCreateInstance;


HRESULT __stdcall CoCreateInstanceCallBack(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv
)
{
	HRESULT hr;
    hr = CoCreateInstanceNex(rclsid,
                             pUnkOuter,dwClsContext,riid,ppv);
    if(SUCCEEDED(hr) && (rclsid == __uuidof(XAudio2_Debug) ||
                         rclsid == __uuidof(XAudio2)))
    {
        DEBUG_INFO("get xaudio2\n");
    }
    return hr;
}

HRESULT WINAPI XAudio2CreateCallBack(
    IXAudio2 **ppXAudio2,
    UINT32 Flags,
    XAUDIO2_PROCESSOR XAudio2Processor
)
{
    HRESULT hr;

    hr = XAudio2CreateNext(ppXAudio2,Flags,XAudio2Processor);
    DEBUG_INFO("call XAudio2CreateCallBack return hr 0x%08x\n",hr);
    return hr;
}

int RoutineDetourXAudio2(void)
{
    assert(DirectSoundCreate8Next);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)&XAudio2CreateNext,XAudio2CreateCallBack);
    DetourAttach((PVOID*)&CoCreateInstanceNex,CoCreateInstanceCallBack);
    DetourTransactionCommit();

    DEBUG_INFO("xaudio2\n");
    return 0;
}

void RoutineClearXAudio2(void)
{
    return;
}

