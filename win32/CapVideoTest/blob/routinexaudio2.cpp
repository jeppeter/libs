
#include "routinexaudio2.h"
#include "..\\common\\output_debug.h"
#include <assert.h>
#include <Objbase.h>
#include "..\\detours\\detours.h"
#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <XAudio2.h>
#include <vector>


//#pragma comment(lib,"Xaudio2.lib")
#define COM_METHOD(TYPE, METHOD) TYPE STDMETHODCALLTYPE METHOD

class CIMMDeviceCollectionHook;
static ULONG UnRegisterMMDevCollection(IMMDeviceCollection* pCollection);
static CIMMDeviceCollectionHook* RegisterMMDevCollection(IMMDeviceCollection* pCollection);


class CIMMDeviceHook;
static ULONG UnRegisterMMEnumerator(IMMDeviceEnumerator *pEnumerator);
static ULONG UnRegisterMMDevice(IMMDevice *pDevice);
static CIMMDeviceHook* RegisterMMDevice(IMMDevice* pDevice);



static CRITICAL_SECTION st_MMDevCollectionCS;
static std::vector<IMMDeviceCollection*> st_MMDevCollectionVecs;
static std::vector<CIMMDeviceCollectionHook*> st_MMColHookVecs;

#define  MMDEV_COLLECTION_IN() do{DEBUG_INFO("MMDEV_COLLECTION_IN\n");}while(0)
#define  MMDEV_COLLECTION_OUT() 

class CIMMDeviceCollectionHook : public IMMDeviceCollection
{
private:
    IMMDeviceCollection *m_ptr;
public:
    CIMMDeviceCollectionHook(IMMDeviceCollection *ptr) : m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        MMDEV_COLLECTION_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        MMDEV_COLLECTION_OUT();
        return hr;
    }

    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        MMDEV_COLLECTION_IN();
        uret = m_ptr->AddRef();
        MMDEV_COLLECTION_OUT();
        return uret;
    }

    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        MMDEV_COLLECTION_IN();
        uret = m_ptr->Release();
        if(uret == 1)
        {
            uret = UnRegisterMMDevCollection(m_ptr);
            if(uret == 0)
            {
                this->m_ptr = NULL;
            }
        }
        MMDEV_COLLECTION_OUT();
        if(uret == 0)
        {
            delete this;
        }
        return uret;
    }

    COM_METHOD(HRESULT,GetCount)(THIS_ UINT *pcDevices)
    {
        HRESULT hr;
        MMDEV_COLLECTION_IN();
        hr = m_ptr->GetCount(pcDevices);
        MMDEV_COLLECTION_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Item)(THIS_ UINT nDevice,IMMDevice **ppDevice)
    {
        HRESULT hr;
        MMDEV_COLLECTION_IN();
        hr = m_ptr->Item(nDevice,ppDevice);
        if(SUCCEEDED(hr))
        {
            CIMMDeviceHook* pHook=NULL;
            IMMDevice   *pDevice= *ppDevice;
            pHook = RegisterMMDevice(pDevice);
            *ppDevice = (IMMDevice*)pHook;
            DEBUG_INFO("item %d device 0x%p hook 0x%p\n",nDevice,pDevice,pHook);
        }
        MMDEV_COLLECTION_OUT();
        return hr;
    }
};

static CIMMDeviceCollectionHook* RegisterMMDevCollection(IMMDeviceCollection * pCollection)
{
    int findidx = -1;
    unsigned int i;
    CIMMDeviceCollectionHook* pCollectHook=NULL;
    EnterCriticalSection(&st_MMDevCollectionCS);

    assert(st_MMDevCollectionVecs.size() == st_MMColHookVecs.size());
    for(i=0; i<st_MMDevCollectionVecs.size(); i++)
    {
        if(st_MMDevCollectionVecs[i] == pCollection)
        {
            findidx = i;
            break;
        }
    }

    if(findidx < 0)
    {
        pCollectHook = new CIMMDeviceCollectionHook(pCollection);
        st_MMDevCollectionVecs.push_back(pCollection);
        st_MMColHookVecs.push_back(pCollectHook);
        pCollection->AddRef();
    }
    else
    {
        pCollectHook = st_MMColHookVecs[findidx];
    }
    LeaveCriticalSection(&st_MMDevCollectionCS);
    return pCollectHook;
}


static ULONG UnRegisterMMDevCollection(IMMDeviceCollection * pCollection)
{
    ULONG uret;
    CIMMDeviceCollectionHook *pColHook=NULL;
    int findidx=-1;
    unsigned int i;

    EnterCriticalSection(&st_MMDevCollectionCS);
    for(i=0; i<st_MMDevCollectionVecs.size(); i++)
    {
        if(st_MMDevCollectionVecs[i] == pCollection)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_MMDevCollectionVecs.erase(st_MMDevCollectionVecs.begin() + findidx);
        st_MMColHookVecs.erase(st_MMColHookVecs.begin() + findidx);
    }

    LeaveCriticalSection(&st_MMDevCollectionCS);
    uret = 1;
    if(findidx >= 0)
    {
        uret = pCollection->Release();
    }
    return uret;
}



#define  MMDEVICE_ENUMERRATOR_IN() do{DEBUG_INFO("MMDEVICE_ENUMERATOR_IN\n");}while(0)
#define  MMDEVICE_ENUMERRATOR_OUT()




class CIMMDeviceEnumeratorHook : public IMMDeviceEnumerator
{
private:
    IMMDeviceEnumerator *m_ptr;
public:
    CIMMDeviceEnumeratorHook(IMMDeviceEnumerator *ptr):m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_  REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        MMDEVICE_ENUMERRATOR_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        MMDEVICE_ENUMERRATOR_OUT();
        return hr;
    }

    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        MMDEVICE_ENUMERRATOR_IN();
        uret = m_ptr->AddRef();
        MMDEVICE_ENUMERRATOR_OUT();
        return uret;
    }

    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        MMDEVICE_ENUMERRATOR_IN();
        uret = m_ptr->Release();

        if(uret == 1)
        {
            uret = UnRegisterMMEnumerator(m_ptr);
            if(uret == 0)
            {
                this->m_ptr = NULL;
            }
        }
        MMDEVICE_ENUMERRATOR_OUT();

        if(uret == 0)
        {
            delete this;
        }
        return uret;
    }

    COM_METHOD(HRESULT,EnumAudioEndpoints)(THIS_  EDataFlow dataFlow,DWORD dwStateMask,IMMDeviceCollection **ppDevices)
    {
        HRESULT hr;
        MMDEVICE_ENUMERRATOR_IN();
        hr = m_ptr->EnumAudioEndpoints(dataFlow,dwStateMask,ppDevices);
        if(SUCCEEDED(hr))
        {
            CIMMDeviceCollectionHook *pColHook=NULL;
            IMMDeviceCollection* pCollection=*ppDevices;
            pColHook = RegisterMMDevCollection(pCollection);
            *ppDevices =(IMMDeviceCollection*) pColHook;
            DEBUG_INFO("dataflow %d statemask %d pointer 0x%p Hook 0x%p\n",dataFlow,dwStateMask,pCollection,pColHook);
        }
        MMDEVICE_ENUMERRATOR_OUT();

        return hr;
    }

    COM_METHOD(HRESULT,GetDefaultAudioEndpoint)(THIS_ EDataFlow dataFlow,ERole role,IMMDevice **ppEndpoint)
    {
        HRESULT hr;
        MMDEVICE_ENUMERRATOR_IN();
        hr = m_ptr->GetDefaultAudioEndpoint(dataFlow,role,ppEndpoint);
        if(SUCCEEDED(hr))
        {
            CIMMDeviceHook* pDevHook=NULL;
            IMMDevice* pDevice= (IMMDevice*)*ppEndpoint;
            pDevHook = RegisterMMDevice(pDevice);
            *ppEndpoint = (IMMDevice*)pDevice;
            DEBUG_INFO("put dataflow %d role %d pointer 0x%p Hook 0x%p\n",dataFlow,role,pDevice,pDevHook);

        }
        MMDEVICE_ENUMERRATOR_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDevice)(THIS_ LPCWSTR pwstrId,IMMDevice **ppDevice)
    {
        HRESULT hr;
        MMDEVICE_ENUMERRATOR_IN();
        hr = m_ptr->GetDevice(pwstrId,ppDevice);
        if(SUCCEEDED(hr))
        {
            CIMMDeviceHook* pDevHook=NULL;
            IMMDevice* pDevice=*ppDevice;
            pDevHook = RegisterMMDevice(pDevice);
            *ppDevice = (IMMDevice*)pDevHook;
            DEBUG_INFO("Get Device %S 0x%p hook 0x%p\n",pwstrId,*ppDevice,pDevHook);
        }
        MMDEVICE_ENUMERRATOR_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,RegisterEndpointNotificationCallback)(THIS_ IMMNotificationClient *pClient)
    {
        HRESULT hr;
        MMDEVICE_ENUMERRATOR_IN();
        hr = m_ptr->RegisterEndpointNotificationCallback(pClient);
        MMDEVICE_ENUMERRATOR_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,UnregisterEndpointNotificationCallback)(THIS_ IMMNotificationClient *pClient)
    {
        HRESULT hr;
        MMDEVICE_ENUMERRATOR_IN();
        hr = m_ptr->UnregisterEndpointNotificationCallback(pClient);
        MMDEVICE_ENUMERRATOR_OUT();
        return hr;
    }

};


class CIAudioRenderClientHook;
static std::vector<IAudioRenderClient*> st_AudioRenderClientVecs;
static std::vector<CIAudioRenderClientHook*> st_AudioRenderClientHookVecs;
static CRITICAL_SECTION st_AudioRenderClientCS;

static CIAudioRenderClientHook* RegisterAudioRenderClient(IAudioRenderClient* pRender);
static ULONG UnRegisterAudioRenderClient(IAudioRenderClient* pRender);

#define AUDIO_RENDER_CLIENT_IN() do{DEBUG_INFO("AUDIO_RENDER_CLIENT_IN\n");}while(0)
#define AUDIO_RENDER_CLIENT_OUT()

class CIAudioRenderClientHook : IAudioRenderClient
{
private:
    IAudioRenderClient* m_ptr;
public:
    CIAudioRenderClientHook(IAudioRenderClient *ptr) : m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_  REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        AUDIO_RENDER_CLIENT_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        AUDIO_RENDER_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        AUDIO_RENDER_CLIENT_IN();
        uret = m_ptr->AddRef();
        AUDIO_RENDER_CLIENT_OUT();
        return uret;
    }

    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        AUDIO_RENDER_CLIENT_IN();
        uret = m_ptr->Release();
        if(uret == 1)
        {
            uret = UnRegisterAudioRenderClient(m_ptr);
            if(uret == 0)
            {
                this->m_ptr = NULL;
            }
        }
        AUDIO_RENDER_CLIENT_OUT();
        if(uret == 0)
        {
            delete this;
        }
        return uret;
    }

    COM_METHOD(HRESULT,GetBuffer)(THIS_ UINT32 NumFramesRequested,BYTE **ppData)
    {
        HRESULT hr;
        AUDIO_RENDER_CLIENT_IN();
        hr = m_ptr->GetBuffer(NumFramesRequested,ppData);
        if(SUCCEEDED(hr))
        {
            DEBUG_INFO("request %d data 0x%p\n",NumFramesRequested,*ppData);
        }
        AUDIO_RENDER_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,ReleaseBuffer)(THIS_ UINT32 NumFramesWritten,DWORD dwFlags)
    {
        HRESULT hr;
        AUDIO_RENDER_CLIENT_IN();
        hr = m_ptr->ReleaseBuffer(NumFramesWritten,dwFlags);
        if(SUCCEEDED(hr))
        {
            DEBUG_INFO("written %d flags 0x%08x\n",NumFramesWritten,dwFlags);
        }
        AUDIO_RENDER_CLIENT_OUT();
        return hr;
    }


};

static CIAudioRenderClientHook* RegisterAudioRenderClient(IAudioRenderClient * pRender)
{
    CIAudioRenderClientHook* pHook=NULL;
    int findidx=-1;
    unsigned int i;

    EnterCriticalSection(&st_AudioRenderClientCS);
    assert(st_AudioRenderClientVecs.size() == st_AudioRenderClientHookVecs.size());
    for(i=0; i<st_AudioRenderClientVecs.size(); i++)
    {
        if(st_AudioRenderClientVecs[i] == pRender)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pHook = st_AudioRenderClientHookVecs[findidx];
    }
    else
    {
        pHook = new CIAudioRenderClientHook(pRender);
        st_AudioRenderClientVecs.push_back(pRender);
        st_AudioRenderClientHookVecs.push_back(pHook);
        pRender->AddRef();
    }
    LeaveCriticalSection(&st_AudioRenderClientCS);
    return pHook;
}

static ULONG UnRegisterAudioRenderClient(IAudioRenderClient * pRender)
{
    int findidx=-1;
    unsigned int i;
    ULONG uret;

    EnterCriticalSection(&st_AudioRenderClientCS);
    assert(st_AudioRenderClientVecs.size() == st_AudioRenderClientHookVecs.size());
    for(i=0; i<st_AudioRenderClientVecs.size(); i++)
    {
        if(st_AudioRenderClientVecs[i] == pRender)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_AudioRenderClientVecs.erase(st_AudioRenderClientVecs.begin()+findidx);
        st_AudioRenderClientHookVecs.erase(st_AudioRenderClientHookVecs.begin() + findidx);
    }
    LeaveCriticalSection(&st_AudioRenderClientCS);
    uret = 1;
    if(findidx >= 0)
    {
        uret = pRender->Release();
    }
    return uret;
}

class CIAudioClientHook;

#define  AUDIO_CLIENT_IN() do{DEBUG_INFO("AUDIO CLIENT IN\n");}while(0)
#define  AUDIO_CLIENT_OUT() 

static std::vector<IAudioClient*> st_AudioClientVecs;
static std::vector<CIAudioClientHook*> st_AudioClientHookVecs;
static CRITICAL_SECTION st_AudioClientCS;

static CIAudioClientHook* RegisterAudioClient(IAudioClient* pAudio);
static ULONG UnRegisterAudioClient(IAudioClient* pAudio);


class CIAudioClientHook : public IAudioClient
{
private:
    IAudioClient *m_ptr;
public:
    CIAudioClientHook(IAudioClient *ptr) : m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        AUDIO_CLIENT_IN();
        uret = m_ptr->AddRef();
        AUDIO_CLIENT_OUT();
        return uret;
    }

    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        AUDIO_CLIENT_IN();
        uret = m_ptr->Release();
        if(uret == 1)
        {
            uret = UnRegisterAudioClient(m_ptr);
            if(uret == 0)
            {
                this->m_ptr = NULL;
            }
        }
        AUDIO_CLIENT_OUT();
        if(uret == 0)
        {
            delete this;
        }
        return uret;
    }


    COM_METHOD(HRESULT,Initialize)(THIS_  AUDCLNT_SHAREMODE ShareMode,DWORD StreamFlags,REFERENCE_TIME hnsBufferDuration,
                                   REFERENCE_TIME hnsPeriodicity,const WAVEFORMATEX *pFormat,LPCGUID AudioSessionGuid)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->Initialize(ShareMode,StreamFlags,hnsBufferDuration,hnsPeriodicity,pFormat,AudioSessionGuid);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetBufferSize)(THIS_ UINT32 *pNumBufferFrames)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetBufferSize(pNumBufferFrames);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetStreamLatency)(THIS_  REFERENCE_TIME *phnsLatency)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetStreamLatency(phnsLatency);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetCurrentPadding)(THIS_  UINT32 *pNumPaddingFrames)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetCurrentPadding(pNumPaddingFrames);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,IsFormatSupported)(THIS_  AUDCLNT_SHAREMODE ShareMode,const WAVEFORMATEX *pFormat,WAVEFORMATEX **ppClosestMatch)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->IsFormatSupported(ShareMode,pFormat,ppClosestMatch);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetMixFormat)(THIS_  WAVEFORMATEX **ppDeviceFormat)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetMixFormat(ppDeviceFormat);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDevicePeriod)(THIS_  REFERENCE_TIME *phnsDefaultDevicePeriod,REFERENCE_TIME *phnsMinimumDevicePeriod)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetDevicePeriod(phnsDefaultDevicePeriod,phnsMinimumDevicePeriod);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Start)(THIS)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->Start();
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Stop)(THIS)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->Stop();
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Reset)(THIS)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->Reset();
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetEventHandle)(THIS_ HANDLE eventHandle)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->SetEventHandle(eventHandle);
        AUDIO_CLIENT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetService)(THIS_  REFIID riid,void **ppv)
    {
        HRESULT hr;
        AUDIO_CLIENT_IN();
        hr = m_ptr->GetService(riid,ppv);
        if(SUCCEEDED(hr))
        {
            if(riid == __uuidof(IAudioRenderClient))
            {
                IAudioRenderClient* pRender=(IAudioRenderClient*)*ppv;
                CIAudioRenderClientHook* pHook=NULL;
                pHook = RegisterAudioRenderClient(pRender);
                *ppv = (IAudioRenderClient*) pHook;
				DEBUG_INFO("Render 0x%p hook 0x%p\n",pRender,pHook);
            }
        }
        AUDIO_CLIENT_OUT();
        return hr;
    }


};


static CIAudioClientHook* RegisterAudioClient(IAudioClient * pAudio)
{
    CIAudioClientHook* pAudioHook=NULL;
    int findidx = -1;
    unsigned int i;

    EnterCriticalSection(&st_AudioClientCS);

    assert(st_AudioClientHookVecs.size() ==  st_AudioClientVecs.size());
    for(i=0; i<st_AudioClientVecs.size() ; i++)
    {
        if(st_AudioClientVecs[i] == pAudio)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pAudioHook = st_AudioClientHookVecs[findidx];
    }
    else
    {
        pAudioHook = new CIAudioClientHook(pAudio);
        st_AudioClientHookVecs.push_back(pAudioHook);
        st_AudioClientVecs.push_back(pAudio);
        pAudio->AddRef();
    }

    LeaveCriticalSection(&st_AudioClientCS);
    return pAudioHook;
}

static ULONG UnRegisterAudioClient(IAudioClient * pAudio)
{
    int findidx = -1;
    unsigned int i;
    ULONG uret;

    EnterCriticalSection(&st_AudioClientCS);

    assert(st_AudioClientHookVecs.size() ==  st_AudioClientVecs.size());
    for(i=0; i<st_AudioClientVecs.size() ; i++)
    {
        if(st_AudioClientVecs[i] == pAudio)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_AudioClientHookVecs.erase(st_AudioClientHookVecs.begin() + findidx);
        st_AudioClientVecs.erase(st_AudioClientVecs.begin() + findidx);
    }

    LeaveCriticalSection(&st_AudioClientCS);

    uret = 1;
    if(findidx >= 0)
    {
        uret = pAudio->Release();
    }
    return uret;
}



#define MMDEVICE_IN() do{DEBUG_INFO("MMDEVICE_IN\n");}while(0)
#define MMDEVICE_OUT()

class CIMMDeviceHook : public IMMDevice
{
private:
    IMMDevice *m_ptr;
public:
    CIMMDeviceHook(IMMDevice* ptr) : m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_  REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        MMDEVICE_OUT();
        return hr;
    }

    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        MMDEVICE_IN();
        uret = m_ptr->AddRef();
        MMDEVICE_OUT();
        return uret;
    }

    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        MMDEVICE_IN();
        uret = m_ptr->Release();
        if(uret == 1)
        {
            uret = UnRegisterMMDevice(m_ptr);
            if(uret == 0)
            {
                this->m_ptr=NULL;
            }
        }
        MMDEVICE_OUT();
        if(uret == 0)
        {
            delete this;
        }
        return uret;
    }

    COM_METHOD(HRESULT,Activate)(THIS_ REFIID iid,DWORD dwClsCtx,PROPVARIANT *pActivationParams,void **ppInterface)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->Activate(iid,dwClsCtx,pActivationParams,ppInterface);
        if(SUCCEEDED(hr))
        {
            if(iid == __uuidof(IAudioClient))
            {
                CIAudioClientHook* pAudioHook=NULL;
                IAudioClient* pAudio=(IAudioClient*)*ppInterface;
                pAudioHook = new CIAudioClientHook(pAudio);
                *ppInterface = (IAudioClient*) pAudioHook;
                DEBUG_INFO("dwClsCtx 0x%x audioclient 0x%p hook 0x%p\n",dwClsCtx,pAudio,pAudioHook);
            }
        }
        MMDEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,OpenPropertyStore)(THIS_ DWORD stgmAccess,IPropertyStore **ppProperties)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->OpenPropertyStore(stgmAccess,ppProperties);
        MMDEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetId)(THIS_ LPWSTR *ppstrId)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->GetId(ppstrId);
        MMDEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetState)(THIS_ DWORD *pdwState)
    {
        HRESULT hr;
        MMDEVICE_IN();
        hr = m_ptr->GetState(pdwState);
        MMDEVICE_OUT();
        return hr;
    }


};



static CRITICAL_SECTION st_MMDevEnumCS;
static CRITICAL_SECTION st_MMDevCS;
static int st_InitializeXAudio2=0;


static std::vector<IMMDeviceEnumerator*> st_EnumeratorVecs;
static std::vector<CIMMDeviceEnumeratorHook*> st_EnumHookVecs;


static CIMMDeviceEnumeratorHook* RegisterEnumerator(IMMDeviceEnumerator* pEnumerator)
{
    int ret=1;
    unsigned int i;
    int findidx = -1;
    CIMMDeviceEnumeratorHook* pEnumHook=NULL;
    EnterCriticalSection(&st_MMDevEnumCS);
    ULONG uret;

    findidx = -1;
    for(i=0; i<st_EnumeratorVecs.size() ; i++)
    {
        if(st_EnumeratorVecs[i] == pEnumerator)
        {
            findidx = i;
            break;
        }
    }

    if(findidx < 0)
    {
        pEnumHook = new CIMMDeviceEnumeratorHook(pEnumerator);
        st_EnumeratorVecs.push_back(pEnumerator);
        st_EnumHookVecs.push_back(pEnumHook);
        uret = pEnumerator->AddRef();
    }
    else
    {
        pEnumHook = st_EnumHookVecs[findidx];
    }

    LeaveCriticalSection(&st_MMDevEnumCS);

    if(findidx < 0)
    {
        DEBUG_INFO("0x%p uret %d\n",pEnumHook,uret);
    }

    return pEnumHook;
}

static ULONG UnRegisterMMEnumerator(IMMDeviceEnumerator *pEnumerator)
{
    ULONG uret=1;
    unsigned int i;
    int findidx =-1;

    DEBUG_INFO("Unregister Enumerator 0x%p\n",pEnumerator);
    EnterCriticalSection(&st_MMDevEnumCS);

    for(i=0; i<st_EnumeratorVecs.size() ; i++)
    {
        if(st_EnumeratorVecs[i] == pEnumerator)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_EnumeratorVecs.erase(st_EnumeratorVecs.begin() + findidx);
        st_EnumHookVecs.erase(st_EnumHookVecs.begin() + findidx);
    }

    LeaveCriticalSection(&st_MMDevEnumCS);

    uret = 1;
    if(findidx >= 0)
    {
        uret = pEnumerator->Release();
    }

    return uret;
}


static std::vector<IMMDevice*> st_MMDeviceVecs;
static std::vector<CIMMDeviceHook*> st_MMDeviceHookVecs;


static void __DebugMMDevice()
{
    unsigned int i;
    DEBUG_INFO("MMDeviceVecs %d\n",st_MMDeviceVecs.size());
    for(i=0; i<st_MMDeviceVecs.size(); i++)
    {
        DEBUG_INFO("[%d] device 0x%p\n",i,st_MMDeviceVecs[i]);
    }
    return;
}

static CIMMDeviceHook* RegisterMMDevice(IMMDevice* pDevice)
{
    CIMMDeviceHook* pDevHook=NULL;
    int findidx = -1;
    unsigned int i;
    EnterCriticalSection(&st_MMDevCS);
    assert(st_MMDeviceVecs.size() == st_MMDeviceHookVecs.size());
    for(i=0; i<st_MMDeviceVecs.size(); i++)
    {
        if(st_MMDeviceVecs[i] == pDevice)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pDevHook = st_MMDeviceHookVecs[findidx];
    }
    else
    {
        pDevHook = new CIMMDeviceHook(pDevice);
        st_MMDeviceVecs.push_back(pDevice);
        st_MMDeviceHookVecs.push_back(pDevHook);
        assert(pDevHook);
        pDevice->AddRef();
    }

    LeaveCriticalSection(&st_MMDevCS);

    return pDevHook;

}

static ULONG UnRegisterMMDevice(IMMDevice* pDevice)
{
    int findidx=-1;
    ULONG uret;
    unsigned int i;
    CIMMDeviceHook* pDevHook=NULL;
    EnterCriticalSection(&st_MMDevCS);
    for(i=0; i<st_MMDeviceVecs.size(); i++)
    {
        if(st_MMDeviceVecs[i] == pDevice)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        pDevHook = st_MMDeviceHookVecs[findidx];
        st_MMDeviceHookVecs.erase(st_MMDeviceHookVecs.begin()+findidx);
        st_MMDeviceVecs.erase(st_MMDeviceVecs.begin() + findidx);
    }

    LeaveCriticalSection(&st_MMDevCS);
    uret = 1;
    if(findidx >= 0)
    {
        uret = pDevice->Release();
    }

    return uret;
}





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

    COM_METHOD(void, StopEngine)(THIS)
    {
        XAUDIO2_IN();
        m_ptr->StopEngine();
        XAUDIO2_OUT();
        return ;
    }

    COM_METHOD(HRESULT,CommitChanges)(THIS_ UINT32 OperationSet)
    {
        HRESULT hr;
        XAUDIO2_IN();
        hr = m_ptr->CommitChanges(OperationSet);
        XAUDIO2_OUT();
        return hr;
    }

    COM_METHOD(void, GetPerformanceData)(THIS_ XAUDIO2_PERFORMANCE_DATA* pPerfData)
    {
        XAUDIO2_IN();
        m_ptr->GetPerformanceData(pPerfData);
        XAUDIO2_OUT();
        return ;
    }

    COM_METHOD(void, SetDebugConfiguration)(THIS_ const XAUDIO2_DEBUG_CONFIGURATION* pDebugConfiguration,
                                            void* pReserved X2DEFAULT(NULL))
    {
        XAUDIO2_IN();
        m_ptr->SetDebugConfiguration(pDebugConfiguration,pReserved);
        XAUDIO2_OUT();
        return;
    }

};




#define XAUDIO2SOURCEVOICE_IN()
#define XAUDIO2SOURCEVOICE_OUT()

class CXAudio2SourceVoiceHook : public IXAudio2SourceVoice
{
private:
    IXAudio2SourceVoice *m_ptr;
public:
    CXAudio2SourceVoiceHook(IXAudio2SourceVoice* ptr) : m_ptr(ptr) {};
public:
    COM_METHOD(void, GetVoiceDetails)(THIS_ XAUDIO2_VOICE_DETAILS* pVoiceDetails)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetVoiceDetails(pVoiceDetails);
        XAUDIO2SOURCEVOICE_OUT();
        return;
    }

    COM_METHOD(HRESULT,SetOutputVoices)(THIS_ const XAUDIO2_VOICE_SENDS* pSendList)
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetOutputVoices(pSendList);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetEffectChain)(THIS_ const XAUDIO2_EFFECT_CHAIN* pEffectChain)
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetEffectChain(pEffectChain);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnableEffect)(THIS_ UINT32 EffectIndex,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->EnableEffect(EffectIndex,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,DisableEffect)(THIS_ UINT32 EffectIndex,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->DisableEffect(EffectIndex,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetEffectState)(THIS_ UINT32 EffectIndex, __out BOOL* pEnabled)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetEffectState(EffectIndex,pEnabled);
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetEffectParameters)(THIS_ UINT32 EffectIndex, const void* pParameters,UINT32 ParametersByteSize,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetEffectParameters(EffectIndex,pParameters,ParametersByteSize,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetEffectParameters)(THIS_ UINT32 EffectIndex,void* pParameters,UINT32 ParametersByteSize)
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->GetEffectParameters(EffectIndex,pParameters,ParametersByteSize);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetFilterParameters)(THIS_ const XAUDIO2_FILTER_PARAMETERS* pParameters,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetFilterParameters(pParameters,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetFilterParameters)(THIS_ XAUDIO2_FILTER_PARAMETERS* pParameters)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetFilterParameters(pParameters);
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetOutputFilterParameters)(THIS_ IXAudio2Voice* pDestinationVoice,const XAUDIO2_FILTER_PARAMETERS* pParameters,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetOutputFilterParameters(pDestinationVoice,pParameters,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetOutputFilterParameters)(THIS_ IXAudio2Voice* pDestinationVoice,XAUDIO2_FILTER_PARAMETERS* pParameters)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetOutputFilterParameters(pDestinationVoice,pParameters);
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetVolume)(THIS_ float Volume,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetVolume(Volume,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetVolume)(THIS_ float* pVolume)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetVolume(pVolume);
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetChannelVolumes)(THIS_ UINT32 Channels, const float* pVolumes,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetChannelVolumes(Channels,pVolumes,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetChannelVolumes)(THIS_ UINT32 Channels,float* pVolumes)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetChannelVolumes(Channels,pVolumes);
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetOutputMatrix)(THIS_ IXAudio2Voice* pDestinationVoice,UINT32 SourceChannels, UINT32 DestinationChannels,const float* pLevelMatrix,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetOutputMatrix(pDestinationVoice,SourceChannels,DestinationChannels,pLevelMatrix,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetOutputMatrix)(THIS_ IXAudio2Voice* pDestinationVoice,UINT32 SourceChannels, UINT32 DestinationChannels,float* pLevelMatrix)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetOutputMatrix(pDestinationVoice,SourceChannels,DestinationChannels,pLevelMatrix);
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(void, DestroyVoice)(THIS)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->DestroyVoice();
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,Start)(THIS_ UINT32 Flags X2DEFAULT(0), UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->Start(Flags,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Stop)(THIS_ UINT32 Flags X2DEFAULT(0), UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->Stop(Flags,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SubmitSourceBuffer)(THIS_ const XAUDIO2_BUFFER* pBuffer,const XAUDIO2_BUFFER_WMA* pBufferWMA X2DEFAULT(NULL))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SubmitSourceBuffer(pBuffer,pBufferWMA);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,FlushSourceBuffers)(THIS)
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->FlushSourceBuffers();
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Discontinuity)(THIS)
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->Discontinuity();
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,ExitLoop)(THIS_ UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->ExitLoop(OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetState)(THIS_ XAUDIO2_VOICE_STATE* pVoiceState)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetState(pVoiceState);
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetFrequencyRatio)(THIS_ float Ratio,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetFrequencyRatio(Ratio,OperationSet);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetFrequencyRatio)(THIS_ float* pRatio)
    {
        XAUDIO2SOURCEVOICE_IN();
        m_ptr->GetFrequencyRatio(pRatio);
        XAUDIO2SOURCEVOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetSourceSampleRate)(THIS_ UINT32 NewSourceSampleRate)
    {
        HRESULT hr;
        XAUDIO2SOURCEVOICE_IN();
        hr = m_ptr->SetSourceSampleRate(NewSourceSampleRate);
        XAUDIO2SOURCEVOICE_OUT();
        return hr;
    }

};



#define  XAUDIO2_MASTER_VOICE_IN()
#define  XAUDIO2_MASTER_VOICE_OUT()

class CXAudio2MasteringVoiceHook : public IXAudio2MasteringVoice
{
private:
    IXAudio2MasteringVoice *m_ptr;
public:
    CXAudio2MasteringVoiceHook(IXAudio2MasteringVoice* ptr) : m_ptr(ptr) {};
public:
    COM_METHOD(void, GetVoiceDetails)(THIS_ XAUDIO2_VOICE_DETAILS* pVoiceDetails)
    {
        XAUDIO2_MASTER_VOICE_IN();
        m_ptr->GetVoiceDetails(pVoiceDetails);
        XAUDIO2_MASTER_VOICE_OUT();
        return;
    }

    COM_METHOD(HRESULT,SetOutputVoices)(THIS_ const XAUDIO2_VOICE_SENDS* pSendList)
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->SetOutputVoices(pSendList);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetEffectChain)(THIS_ const XAUDIO2_EFFECT_CHAIN* pEffectChain)
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->SetEffectChain(pEffectChain);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnableEffect)(THIS_ UINT32 EffectIndex,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->EnableEffect(EffectIndex,OperationSet);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,DisableEffect)(THIS_ UINT32 EffectIndex,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->DisableEffect(EffectIndex,OperationSet);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetEffectState)(THIS_ UINT32 EffectIndex, __out BOOL* pEnabled)
    {
        XAUDIO2_MASTER_VOICE_IN();
        m_ptr->GetEffectState(EffectIndex,pEnabled);
        XAUDIO2_MASTER_VOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetEffectParameters)(THIS_ UINT32 EffectIndex, const void* pParameters,UINT32 ParametersByteSize,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->SetEffectParameters(EffectIndex,pParameters,ParametersByteSize,OperationSet);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetEffectParameters)(THIS_ UINT32 EffectIndex,void* pParameters,UINT32 ParametersByteSize)
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->GetEffectParameters(EffectIndex,pParameters,ParametersByteSize);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetFilterParameters)(THIS_ const XAUDIO2_FILTER_PARAMETERS* pParameters,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->SetFilterParameters(pParameters,OperationSet);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetFilterParameters)(THIS_ XAUDIO2_FILTER_PARAMETERS* pParameters)
    {
        XAUDIO2_MASTER_VOICE_IN();
        m_ptr->GetFilterParameters(pParameters);
        XAUDIO2_MASTER_VOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetOutputFilterParameters)(THIS_ IXAudio2Voice* pDestinationVoice,const XAUDIO2_FILTER_PARAMETERS* pParameters,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->SetOutputFilterParameters(pDestinationVoice,pParameters,OperationSet);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetOutputFilterParameters)(THIS_ IXAudio2Voice* pDestinationVoice,XAUDIO2_FILTER_PARAMETERS* pParameters)
    {
        XAUDIO2_MASTER_VOICE_IN();
        m_ptr->GetOutputFilterParameters(pDestinationVoice,pParameters);
        XAUDIO2_MASTER_VOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetVolume)(THIS_ float Volume,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->SetVolume(Volume,OperationSet);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetVolume)(THIS_ float* pVolume)
    {
        XAUDIO2_MASTER_VOICE_IN();
        m_ptr->GetVolume(pVolume);
        XAUDIO2_MASTER_VOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetChannelVolumes)(THIS_ UINT32 Channels, const float* pVolumes,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->SetChannelVolumes(Channels,pVolumes,OperationSet);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetChannelVolumes)(THIS_ UINT32 Channels,float* pVolumes)
    {
        XAUDIO2_MASTER_VOICE_IN();
        m_ptr->GetChannelVolumes(Channels,pVolumes);
        XAUDIO2_MASTER_VOICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetOutputMatrix)(THIS_ IXAudio2Voice* pDestinationVoice,UINT32 SourceChannels, UINT32 DestinationChannels,const float* pLevelMatrix,UINT32 OperationSet X2DEFAULT(XAUDIO2_COMMIT_NOW))
    {
        HRESULT hr;
        XAUDIO2_MASTER_VOICE_IN();
        hr = m_ptr->SetOutputMatrix(pDestinationVoice,SourceChannels,DestinationChannels,pLevelMatrix,OperationSet);
        XAUDIO2_MASTER_VOICE_OUT();
        return hr;
    }

    COM_METHOD(void, GetOutputMatrix)(THIS_ IXAudio2Voice* pDestinationVoice,UINT32 SourceChannels, UINT32 DestinationChannels,float* pLevelMatrix)
    {
        XAUDIO2_MASTER_VOICE_IN();
        m_ptr->GetOutputMatrix(pDestinationVoice,SourceChannels,DestinationChannels,pLevelMatrix);
        XAUDIO2_MASTER_VOICE_OUT();
        return ;
    }

    COM_METHOD(void, DestroyVoice)(THIS)
    {
        XAUDIO2_MASTER_VOICE_IN();
        m_ptr->DestroyVoice();
        XAUDIO2_MASTER_VOICE_OUT();
        return ;
    }

};




static  HRESULT(WINAPI *CoCreateInstanceNext)(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv
) = CoCreateInstance;


HRESULT WINAPI  CoCreateInstanceCallBack(
    REFCLSID rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext,
    REFIID riid,
    LPVOID *ppv
)
{
    HRESULT hr;
    hr = CoCreateInstanceNext(rclsid,
                              pUnkOuter,dwClsContext,riid,ppv);
    if(SUCCEEDED(hr) && (rclsid == __uuidof(XAudio2_Debug) ||
                         rclsid == __uuidof(XAudio2)))
    {

        DEBUG_INFO("get xaudio2\n");
    }
    else if(SUCCEEDED(hr) && (rclsid == __uuidof(MMDeviceEnumerator)))
    {
        IMMDeviceEnumerator* pEnumerator = (IMMDeviceEnumerator*)(*ppv);
        CIMMDeviceEnumeratorHook* pEnumHook=NULL;
        pEnumHook = RegisterEnumerator(pEnumerator);

        DEBUG_INFO("find enumerator 0x%p hook 0x%p\n",pEnumerator,pEnumHook);
        assert(pEnumHook);
        *ppv = (LPVOID)pEnumHook;

    }
    return hr;
}

static int InitEnvironMentXAudio2(void)
{
    InitializeCriticalSection(&st_MMDevEnumCS);
    InitializeCriticalSection(&st_MMDevCS);
    InitializeCriticalSection(&st_MMDevCollectionCS);
    InitializeCriticalSection(&st_AudioClientCS);
    InitializeCriticalSection(&st_AudioRenderClientCS);
    st_InitializeXAudio2 = 1;
    return 0;
}

static void ClearEnvironmentXAudio2(void)
{
    if(st_InitializeXAudio2)
    {
    }
    return ;
}


int RoutineDetourXAudio2(void)
{
    int ret;
    ret = InitEnvironMentXAudio2();
    if(ret < 0)
    {
        return 0;
    }
    assert(DirectSoundCreate8Next);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)&CoCreateInstanceNext,CoCreateInstanceCallBack);
    DetourTransactionCommit();


    DEBUG_INFO("xaudio2\n");
    return 0;
}

void RoutineClearXAudio2(void)
{
    ClearEnvironmentXAudio2();
    return;
}

