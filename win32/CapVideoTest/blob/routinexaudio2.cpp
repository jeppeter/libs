
#include <xaudio2.h>
#include "routinexaudio2.h"
#include "..\\common\\output_debug.h"
#include <assert.h>
#include <Objbase.h>
#include "..\\detours\\detours.h"


//#pragma comment(lib,"Xaudio2.lib")

#define COM_METHOD(TYPE, METHOD) TYPE STDMETHODCALLTYPE METHOD


#define  XAUDIO2_IN()
#define  XAUDIO2_OUT()

class CXAudio2Hook : public IXAudio2
{
private:
    IXAudio2 *m_ptr;
public:
    CXAudio2Hook(IXAudio2* ptr) : m_ptr(ptr) {};

public:
    COM_METHOD(HRESULT, QueryInterface)(THIS_ REFIID riid, void** ppvObj)
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->QueryInterface(riid,ppvObj);
        XAUDIO2_OUT();
        return hr;
    }

    COM_METHOD(ULONG, AddRef)(THIS)
    {
        ULONG uret;
        XAUDIO2_IN();
        uret = m_ptr->AddRef();
        XAUDIO2_OUT();
        return uret;
    }

    COM_METHOD(ULONG, Release)(THIS)
    {
        ULONG uret;
        XAUDIO2_IN();
        uret = m_ptr->Release();
        XAUDIO2_OUT();
        return uret;
    }

    COM_METHOD(HRESULT,GetDeviceCount)(THIS_ UINT32* pCount)
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->GetDeviceCount(pCount);
        XAUDIO2_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceDetails)(THIS_ UINT32 Index,  XAUDIO2_DEVICE_DETAILS* pDeviceDetails)
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->GetDeviceDetails(Index,pDeviceDetails);
        XAUDIO2_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Initialize)(THIS_ UINT32 Flags X2DEFAULT(0),XAUDIO2_PROCESSOR XAudio2Processor X2DEFAULT(XAUDIO2_DEFAULT_PROCESSOR))
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->Initialize(Flags,XAudio2Processor);
        XAUDIO2_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,RegisterForCallbacks)(THIS_ IXAudio2EngineCallback* pCallback)
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->RegisterForCallbacks(pCallback);
        XAUDIO2_OUT();
        return hr;
    }

    COM_METHOD(void, UnregisterForCallbacks)(THIS_ IXAudio2EngineCallback* pCallback)
    {
        XAUDIO2_IN();
        m_ptr->UnregisterForCallbacks(pCallback);
        XAUDIO2_OUT();
        return;
    }

    COM_METHOD(HRESULT,CreateSourceVoice)(THIS_ IXAudio2SourceVoice** ppSourceVoice,const WAVEFORMATEX* pSourceFormat, UINT32 Flags X2DEFAULT(0),
                                          float MaxFrequencyRatio X2DEFAULT(XAUDIO2_DEFAULT_FREQ_RATIO),
                                          IXAudio2VoiceCallback* pCallback X2DEFAULT(NULL),
                                          const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
                                          const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL))
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->CreateSourceVoice(ppSourceVoice,pSourceFormat,Flags,MaxFrequencyRatio,pCallback,pSendList,pEffectChain);
        XAUDIO2_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,CreateSubmixVoice)(THIS_ IXAudio2SubmixVoice** ppSubmixVoice,
                                          UINT32 InputChannels, UINT32 InputSampleRate,
                                          UINT32 Flags X2DEFAULT(0), UINT32 ProcessingStage X2DEFAULT(0),
                                          const XAUDIO2_VOICE_SENDS* pSendList X2DEFAULT(NULL),
                                          const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL))
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->CreateSubmixVoice(ppSubmixVoice,InputChannels,InputSampleRate,Flags,ProcessingStage,pSendList,pEffectChain);
        XAUDIO2_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateMasteringVoice)(THIS_ IXAudio2MasteringVoice** ppMasteringVoice,
            UINT32 InputChannels X2DEFAULT(XAUDIO2_DEFAULT_CHANNELS),
            UINT32 InputSampleRate X2DEFAULT(XAUDIO2_DEFAULT_SAMPLERATE),
            UINT32 Flags X2DEFAULT(0), UINT32 DeviceIndex X2DEFAULT(0),
            const XAUDIO2_EFFECT_CHAIN* pEffectChain X2DEFAULT(NULL))
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->CreateMasteringVoice(ppMasteringVoice,InputChannels,InputSampleRate,Flags,DeviceIndex,pEffectChain);
        XAUDIO2_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,StartEngine)(THIS)
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->StartEngine();
        XAUDIO2_OUT();
        return hr;
    }


};

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

