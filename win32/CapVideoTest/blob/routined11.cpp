
#include "routined11.h"
#include "d3d11.h"


static HRESULT (WINAPI* D3D11CreateDeviceAndSwapChainNext)(IDXGIAdapter *pAdapter,
     D3D_DRIVER_TYPE DriverType,
     HMODULE Software,
     UINT Flags,
     const D3D_FEATURE_LEVEL *pFeatureLevels,
     UINT FeatureLevels,
     UINT SDKVersion,
     const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    IDXGISwapChain **ppSwapChain,
    ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel,
    ID3D11DeviceContext **ppImmediateContext)
    = D3D11CreateDeviceAndSwapChain;

HRESULT D3D11CreateDeviceAndSwapChainCallBack(
     IDXGIAdapter *pAdapter,
     D3D_DRIVER_TYPE DriverType,
     HMODULE Software,
     UINT Flags,
     const D3D_FEATURE_LEVEL *pFeatureLevels,
     UINT FeatureLevels,
     UINT SDKVersion,
     const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
    IDXGISwapChain **ppSwapChain,
    ID3D11Device **ppDevice,
    D3D_FEATURE_LEVEL *pFeatureLevel,
    ID3D11DeviceContext **ppImmediateContext
)
{
	HRESULT hr;

	hr = D3D11CreateDeviceAndSwapChainNext(pAdapter,DriverType,Software,Flags,pFeatureLevels,
		FeatureLevels,SDKVersion,pSwapChainDesc,ppSwapChain,ppDevice,pFeatureLevel,ppImmediateContext);
	return hr;
}


static int InitializeD11Hook(void)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)&D3D11CreateDeviceAndSwapChainNext, D3D11CreateDeviceAndSwapChainCallBack);
    DetourTransactionCommit();


    return 0;
}

int RoutineDetourD11(void)
{
	int ret;

	ret = InitializeD11Hook();
	if (ret < 0)
	{
		return ret;
	}

	return 0;
	
}

