
#include "routined11.h"
#include "d3d11.h"
#include "..\\detours\\detours.h"
#include "..\\common\\output_debug.h"

#pragma comment(lib,"d3d11.lib")

#define COM_METHOD(TYPE, METHOD) TYPE STDMETHODCALLTYPE METHOD



#if 0

/**********************************************
*  this is the IDXGISwapChain define
**********************************************/

HRESULT(STDMETHODCALLTYPE *QueryInterface)(
    IDXGISwapChain * This,
    /* [in] */ REFIID riid,
    /* [annotation][iid_is][out] */
    __RPC__deref_out  void **ppvObject);

ULONG(STDMETHODCALLTYPE *AddRef)(
    IDXGISwapChain * This);

ULONG(STDMETHODCALLTYPE *Release)(
    IDXGISwapChain * This);

HRESULT(STDMETHODCALLTYPE *SetPrivateData)(
    IDXGISwapChain * This,
    /* [annotation][in] */
    __in  REFGUID Name,
    /* [in] */ UINT DataSize,
    /* [annotation][in] */
    __in_bcount(DataSize)  const void *pData);

HRESULT(STDMETHODCALLTYPE *SetPrivateDataInterface)(
    IDXGISwapChain * This,
    /* [annotation][in] */
    __in  REFGUID Name,
    /* [annotation][in] */
    __in  const IUnknown *pUnknown);

HRESULT(STDMETHODCALLTYPE *GetPrivateData)(
    IDXGISwapChain * This,
    /* [annotation][in] */
    __in  REFGUID Name,
    /* [annotation][out][in] */
    __inout  UINT *pDataSize,
    /* [annotation][out] */
    __out_bcount(*pDataSize)  void *pData);

HRESULT(STDMETHODCALLTYPE *GetParent)(
    IDXGISwapChain * This,
    /* [annotation][in] */
    __in  REFIID riid,
    /* [annotation][retval][out] */
    __out  void **ppParent);

HRESULT(STDMETHODCALLTYPE *GetDevice)(
    IDXGISwapChain * This,
    /* [annotation][in] */
    __in  REFIID riid,
    /* [annotation][retval][out] */
    __out  void **ppDevice);

HRESULT(STDMETHODCALLTYPE *Present)(
    IDXGISwapChain * This,
    /* [in] */ UINT SyncInterval,
    /* [in] */ UINT Flags);

HRESULT(STDMETHODCALLTYPE *GetBuffer)(
    IDXGISwapChain * This,
    /* [in] */ UINT Buffer,
    /* [annotation][in] */
    __in  REFIID riid,
    /* [annotation][out][in] */
    __out  void **ppSurface);

HRESULT(STDMETHODCALLTYPE *SetFullscreenState)(
    IDXGISwapChain * This,
    /* [in] */ BOOL Fullscreen,
    /* [annotation][in] */
    __in_opt  IDXGIOutput *pTarget);

HRESULT(STDMETHODCALLTYPE *GetFullscreenState)(
    IDXGISwapChain * This,
    /* [annotation][out] */
    __out  BOOL *pFullscreen,
    /* [annotation][out] */
    __out  IDXGIOutput **ppTarget);

HRESULT(STDMETHODCALLTYPE *GetDesc)(
    IDXGISwapChain * This,
    /* [annotation][out] */
    __out  DXGI_SWAP_CHAIN_DESC *pDesc);

HRESULT(STDMETHODCALLTYPE *ResizeBuffers)(
    IDXGISwapChain * This,
    /* [in] */ UINT BufferCount,
    /* [in] */ UINT Width,
    /* [in] */ UINT Height,
    /* [in] */ DXGI_FORMAT NewFormat,
    /* [in] */ UINT SwapChainFlags);

HRESULT(STDMETHODCALLTYPE *ResizeTarget)(
    IDXGISwapChain * This,
    /* [annotation][in] */
    __in  const DXGI_MODE_DESC *pNewTargetParameters);

HRESULT(STDMETHODCALLTYPE *GetContainingOutput)(
    IDXGISwapChain * This,
    /* [annotation][out] */
    __out  IDXGIOutput **ppOutput);

HRESULT(STDMETHODCALLTYPE *GetFrameStatistics)(
    IDXGISwapChain * This,
    /* [annotation][out] */
    __out  DXGI_FRAME_STATISTICS *pStats);

HRESULT(STDMETHODCALLTYPE *GetLastPresentCount)(
    IDXGISwapChain * This,
    /* [annotation][out] */
    __out  UINT *pLastPresentCount);


#endif


#define  SWAP_CHAIN_IN()  do{} while(0)

#define  SWAP_CHAIN_OUT()  do{} while(0)


int RegisterSwapChain(IDXGISwapChain *pSwapChain)
{
    return 0;
}

int UnRegisterSwapChain(IDXGISwapChain *pSwapChain)
{
    return 0;
}

class CDXGISwapChainHook : public IDXGISwapChain
{
private:
    IDXGISwapChain* m_ptr;
public:
    CDXGISwapChainHook(IDXGISwapChain* pPtr) : m_ptr(pPtr) {};

public:
    COM_METHOD(HRESULT, QueryInterface)(THIS_ REFIID riid, void** ppvObj)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->QueryInterface(riid,ppvObj);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(ULONG, AddRef)(THIS)
    {
        ULONG uret;
        SWAP_CHAIN_IN();
        uret = m_ptr->AddRef();
        SWAP_CHAIN_OUT();
        return uret;
    }

    COM_METHOD(ULONG, Release)(THIS)
    {
        ULONG uret;
        SWAP_CHAIN_IN();
        uret = m_ptr->Release();
        if(uret == 1)
        {
            uret = UnRegisterSwapChain(m_ptr);
        }
        SWAP_CHAIN_OUT();
        return uret;
    }

    COM_METHOD(HRESULT , SetPrivateData)(THIS_  REFGUID Name,UINT DataSize,const void *pData)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->SetPrivateData(Name,DataSize,pData);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,SetPrivateDataInterface)(THIS_ REFGUID Name,const IUnknown *pUnknown)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->SetPrivateDataInterface(Name,pUnknown);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,GetPrivateData)(THIS_  REFGUID Name,UINT *pDataSize,void *pData)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetPrivateData(Name,pDataSize,pData);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,GetParent)(THIS_ REFIID riid,void **ppParent)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetParent(riid,ppParent);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,GetDevice)(THIS_ REFIID riid,void **ppDevice)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetDevice(riid,ppDevice);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,Present)(THIS_ UINT SyncInterval,UINT Flags)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->Present(SyncInterval,Flags);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,GetBuffer)(THIS_ UINT Buffer,REFIID riid,void **ppSurface)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetBuffer(Buffer,riid,ppSurface);
        SWAP_CHAIN_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,SetFullscreenState)(THIS_ BOOL Fullscreen,IDXGIOutput *pTarget)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->SetFullscreenState(Fullscreen,pTarget);
        SWAP_CHAIN_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,GetFullscreenState)(THIS_ BOOL *pFullscreen,IDXGIOutput **ppTarget)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetFullscreenState(pFullscreen,ppTarget);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDesc)(THIS_ DXGI_SWAP_CHAIN_DESC *pDesc)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetDesc(pDesc);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,ResizeBuffers)(THIS_ UINT BufferCount,UINT Width,UINT Height,DXGI_FORMAT NewFormat,UINT SwapChainFlags)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->ResizeBuffers(BufferCount,Width,Height,NewFormat,SwapChainFlags);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,ResizeTarget)(THIS_ const DXGI_MODE_DESC *pNewTargetParameters)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->ResizeTarget(pNewTargetParameters);
        SWAP_CHAIN_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,GetContainingOutput)(THIS_ IDXGIOutput **ppOutput)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetContainingOutput(ppOutput);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetFrameStatistics)(THIS_ DXGI_FRAME_STATISTICS *pStats)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetFrameStatistics(pStats);
        SWAP_CHAIN_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetLastPresentCount)(THIS_ UINT *pLastPresentCount)
    {
        HRESULT hr;
        SWAP_CHAIN_IN();
        hr = m_ptr->GetLastPresentCount(pLastPresentCount);
        SWAP_CHAIN_OUT();
        return hr;
    }

};



#if 0

/**********************************************
* this is ID3D11DeviceContext for the context
**********************************************/

HRESULT(STDMETHODCALLTYPE *QueryInterface)(
    ID3D11DeviceContext * This,
    /* [in] */ REFIID riid,
    /* [annotation][iid_is][out] */
    __RPC__deref_out  void **ppvObject);

ULONG(STDMETHODCALLTYPE *AddRef)(
    ID3D11DeviceContext * This);

ULONG(STDMETHODCALLTYPE *Release)(
    ID3D11DeviceContext * This);

void (STDMETHODCALLTYPE *GetDevice)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11Device **ppDevice);

HRESULT(STDMETHODCALLTYPE *GetPrivateData)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __inout  UINT *pDataSize,
    /* [annotation] */
    __out_bcount_opt(*pDataSize)	void *pData);

HRESULT(STDMETHODCALLTYPE *SetPrivateData)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in  UINT DataSize,
    /* [annotation] */
    __in_bcount_opt(DataSize)  const void *pData);

HRESULT(STDMETHODCALLTYPE *SetPrivateDataInterface)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in_opt  const IUnknown *pData);

void (STDMETHODCALLTYPE *VSSetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

void (STDMETHODCALLTYPE *PSSetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

void (STDMETHODCALLTYPE *PSSetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11PixelShader *pPixelShader,
    /* [annotation] */
    __in_ecount_opt(NumClassInstances)	ID3D11ClassInstance *const *ppClassInstances,
    UINT NumClassInstances);

void (STDMETHODCALLTYPE *PSSetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

void (STDMETHODCALLTYPE *VSSetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11VertexShader *pVertexShader,
    /* [annotation] */
    __in_ecount_opt(NumClassInstances)	ID3D11ClassInstance *const *ppClassInstances,
    UINT NumClassInstances);

void (STDMETHODCALLTYPE *DrawIndexed)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  UINT IndexCount,
    /* [annotation] */
    __in  UINT StartIndexLocation,
    /* [annotation] */
    __in  INT BaseVertexLocation);

void (STDMETHODCALLTYPE *Draw)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  UINT VertexCount,
    /* [annotation] */
    __in  UINT StartVertexLocation);

HRESULT(STDMETHODCALLTYPE *Map)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Resource *pResource,
    /* [annotation] */
    __in  UINT Subresource,
    /* [annotation] */
    __in  D3D11_MAP MapType,
    /* [annotation] */
    __in  UINT MapFlags,
    /* [annotation] */
    __out  D3D11_MAPPED_SUBRESOURCE *pMappedResource);

void (STDMETHODCALLTYPE *Unmap)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Resource *pResource,
    /* [annotation] */
    __in  UINT Subresource);

void (STDMETHODCALLTYPE *PSSetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

void (STDMETHODCALLTYPE *IASetInputLayout)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11InputLayout *pInputLayout);

void (STDMETHODCALLTYPE *IASetVertexBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __in_ecount(NumBuffers)  ID3D11Buffer *const *ppVertexBuffers,
    /* [annotation] */
    __in_ecount(NumBuffers)  const UINT *pStrides,
    /* [annotation] */
    __in_ecount(NumBuffers)  const UINT *pOffsets);

void (STDMETHODCALLTYPE *IASetIndexBuffer)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11Buffer *pIndexBuffer,
    /* [annotation] */
    __in  DXGI_FORMAT Format,
    /* [annotation] */
    __in  UINT Offset);

void (STDMETHODCALLTYPE *DrawIndexedInstanced)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  UINT IndexCountPerInstance,
    /* [annotation] */
    __in  UINT InstanceCount,
    /* [annotation] */
    __in  UINT StartIndexLocation,
    /* [annotation] */
    __in  INT BaseVertexLocation,
    /* [annotation] */
    __in  UINT StartInstanceLocation);

void (STDMETHODCALLTYPE *DrawInstanced)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  UINT VertexCountPerInstance,
    /* [annotation] */
    __in  UINT InstanceCount,
    /* [annotation] */
    __in  UINT StartVertexLocation,
    /* [annotation] */
    __in  UINT StartInstanceLocation);

void (STDMETHODCALLTYPE *GSSetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

void (STDMETHODCALLTYPE *GSSetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11GeometryShader *pShader,
    /* [annotation] */
    __in_ecount_opt(NumClassInstances)	ID3D11ClassInstance *const *ppClassInstances,
    UINT NumClassInstances);

void (STDMETHODCALLTYPE *IASetPrimitiveTopology)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  D3D11_PRIMITIVE_TOPOLOGY Topology);

void (STDMETHODCALLTYPE *VSSetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

void (STDMETHODCALLTYPE *VSSetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

void (STDMETHODCALLTYPE *Begin)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Asynchronous *pAsync);

void (STDMETHODCALLTYPE *End)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Asynchronous *pAsync);

HRESULT(STDMETHODCALLTYPE *GetData)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Asynchronous *pAsync,
    /* [annotation] */
    __out_bcount_opt(DataSize)  void *pData,
    /* [annotation] */
    __in  UINT DataSize,
    /* [annotation] */
    __in  UINT GetDataFlags);

void (STDMETHODCALLTYPE *SetPredication)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11Predicate *pPredicate,
    /* [annotation] */
    __in  BOOL PredicateValue);

void (STDMETHODCALLTYPE *GSSetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

void (STDMETHODCALLTYPE *GSSetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

void (STDMETHODCALLTYPE *OMSetRenderTargets)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews,
    /* [annotation] */
    __in_ecount_opt(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews,
    /* [annotation] */
    __in_opt  ID3D11DepthStencilView *pDepthStencilView);

void (STDMETHODCALLTYPE *OMSetRenderTargetsAndUnorderedAccessViews)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  UINT NumRTVs,
    /* [annotation] */
    __in_ecount_opt(NumRTVs)  ID3D11RenderTargetView *const *ppRenderTargetViews,
    /* [annotation] */
    __in_opt  ID3D11DepthStencilView *pDepthStencilView,
    /* [annotation] */
    __in_range(0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1)  UINT UAVStartSlot,
    /* [annotation] */
    __in  UINT NumUAVs,
    /* [annotation] */
    __in_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
    /* [annotation] */
    __in_ecount_opt(NumUAVs)  const UINT *pUAVInitialCounts);

void (STDMETHODCALLTYPE *OMSetBlendState)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11BlendState *pBlendState,
    /* [annotation] */
    __in_opt  const FLOAT BlendFactor[ 4 ],
    /* [annotation] */
    __in  UINT SampleMask);

void (STDMETHODCALLTYPE *OMSetDepthStencilState)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11DepthStencilState *pDepthStencilState,
    /* [annotation] */
    __in  UINT StencilRef);

void (STDMETHODCALLTYPE *SOSetTargets)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_SO_BUFFER_SLOT_COUNT)	UINT NumBuffers,
    /* [annotation] */
    __in_ecount_opt(NumBuffers)  ID3D11Buffer *const *ppSOTargets,
    /* [annotation] */
    __in_ecount_opt(NumBuffers)  const UINT *pOffsets);

void (STDMETHODCALLTYPE *DrawAuto)(
    ID3D11DeviceContext * This);

void (STDMETHODCALLTYPE *DrawIndexedInstancedIndirect)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Buffer *pBufferForArgs,
    /* [annotation] */
    __in  UINT AlignedByteOffsetForArgs);

void (STDMETHODCALLTYPE *DrawInstancedIndirect)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Buffer *pBufferForArgs,
    /* [annotation] */
    __in  UINT AlignedByteOffsetForArgs);

void (STDMETHODCALLTYPE *Dispatch)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  UINT ThreadGroupCountX,
    /* [annotation] */
    __in  UINT ThreadGroupCountY,
    /* [annotation] */
    __in  UINT ThreadGroupCountZ);

void (STDMETHODCALLTYPE *DispatchIndirect)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Buffer *pBufferForArgs,
    /* [annotation] */
    __in  UINT AlignedByteOffsetForArgs);

void (STDMETHODCALLTYPE *RSSetState)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11RasterizerState *pRasterizerState);

void (STDMETHODCALLTYPE *RSSetViewports)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports,
    /* [annotation] */
    __in_ecount_opt(NumViewports)  const D3D11_VIEWPORT *pViewports);

void (STDMETHODCALLTYPE *RSSetScissorRects)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects,
    /* [annotation] */
    __in_ecount_opt(NumRects)  const D3D11_RECT *pRects);

void (STDMETHODCALLTYPE *CopySubresourceRegion)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Resource *pDstResource,
    /* [annotation] */
    __in  UINT DstSubresource,
    /* [annotation] */
    __in  UINT DstX,
    /* [annotation] */
    __in  UINT DstY,
    /* [annotation] */
    __in  UINT DstZ,
    /* [annotation] */
    __in  ID3D11Resource *pSrcResource,
    /* [annotation] */
    __in  UINT SrcSubresource,
    /* [annotation] */
    __in_opt  const D3D11_BOX *pSrcBox);

void (STDMETHODCALLTYPE *CopyResource)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Resource *pDstResource,
    /* [annotation] */
    __in  ID3D11Resource *pSrcResource);

void (STDMETHODCALLTYPE *UpdateSubresource)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Resource *pDstResource,
    /* [annotation] */
    __in  UINT DstSubresource,
    /* [annotation] */
    __in_opt  const D3D11_BOX *pDstBox,
    /* [annotation] */
    __in  const void *pSrcData,
    /* [annotation] */
    __in  UINT SrcRowPitch,
    /* [annotation] */
    __in  UINT SrcDepthPitch);

void (STDMETHODCALLTYPE *CopyStructureCount)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Buffer *pDstBuffer,
    /* [annotation] */
    __in  UINT DstAlignedByteOffset,
    /* [annotation] */
    __in  ID3D11UnorderedAccessView *pSrcView);

void (STDMETHODCALLTYPE *ClearRenderTargetView)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11RenderTargetView *pRenderTargetView,
    /* [annotation] */
    __in  const FLOAT ColorRGBA[ 4 ]);

void (STDMETHODCALLTYPE *ClearUnorderedAccessViewUint)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11UnorderedAccessView *pUnorderedAccessView,
    /* [annotation] */
    __in  const UINT Values[ 4 ]);

void (STDMETHODCALLTYPE *ClearUnorderedAccessViewFloat)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11UnorderedAccessView *pUnorderedAccessView,
    /* [annotation] */
    __in  const FLOAT Values[ 4 ]);

void (STDMETHODCALLTYPE *ClearDepthStencilView)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11DepthStencilView *pDepthStencilView,
    /* [annotation] */
    __in  UINT ClearFlags,
    /* [annotation] */
    __in  FLOAT Depth,
    /* [annotation] */
    __in  UINT8 Stencil);

void (STDMETHODCALLTYPE *GenerateMips)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11ShaderResourceView *pShaderResourceView);

void (STDMETHODCALLTYPE *SetResourceMinLOD)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Resource *pResource,
    FLOAT MinLOD);

FLOAT(STDMETHODCALLTYPE *GetResourceMinLOD)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Resource *pResource);

void (STDMETHODCALLTYPE *ResolveSubresource)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11Resource *pDstResource,
    /* [annotation] */
    __in  UINT DstSubresource,
    /* [annotation] */
    __in  ID3D11Resource *pSrcResource,
    /* [annotation] */
    __in  UINT SrcSubresource,
    /* [annotation] */
    __in  DXGI_FORMAT Format);

void (STDMETHODCALLTYPE *ExecuteCommandList)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in  ID3D11CommandList *pCommandList,
    BOOL RestoreContextState);

void (STDMETHODCALLTYPE *HSSetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

void (STDMETHODCALLTYPE *HSSetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11HullShader *pHullShader,
    /* [annotation] */
    __in_ecount_opt(NumClassInstances)	ID3D11ClassInstance *const *ppClassInstances,
    UINT NumClassInstances);

void (STDMETHODCALLTYPE *HSSetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

void (STDMETHODCALLTYPE *HSSetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

void (STDMETHODCALLTYPE *DSSetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

void (STDMETHODCALLTYPE *DSSetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11DomainShader *pDomainShader,
    /* [annotation] */
    __in_ecount_opt(NumClassInstances)	ID3D11ClassInstance *const *ppClassInstances,
    UINT NumClassInstances);

void (STDMETHODCALLTYPE *DSSetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

void (STDMETHODCALLTYPE *DSSetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

void (STDMETHODCALLTYPE *CSSetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews);

void (STDMETHODCALLTYPE *CSSetUnorderedAccessViews)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot)  UINT NumUAVs,
    /* [annotation] */
    __in_ecount(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
    /* [annotation] */
    __in_ecount(NumUAVs)  const UINT *pUAVInitialCounts);

void (STDMETHODCALLTYPE *CSSetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_opt  ID3D11ComputeShader *pComputeShader,
    /* [annotation] */
    __in_ecount_opt(NumClassInstances)	ID3D11ClassInstance *const *ppClassInstances,
    UINT NumClassInstances);

void (STDMETHODCALLTYPE *CSSetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __in_ecount(NumSamplers)  ID3D11SamplerState *const *ppSamplers);

void (STDMETHODCALLTYPE *CSSetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __in_ecount(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers);

void (STDMETHODCALLTYPE *VSGetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

void (STDMETHODCALLTYPE *PSGetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __out_ecount(NumViews)	ID3D11ShaderResourceView **ppShaderResourceViews);

void (STDMETHODCALLTYPE *PSGetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11PixelShader **ppPixelShader,
    /* [annotation] */
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT *pNumClassInstances);

void (STDMETHODCALLTYPE *PSGetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

void (STDMETHODCALLTYPE *VSGetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11VertexShader **ppVertexShader,
    /* [annotation] */
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT *pNumClassInstances);

void (STDMETHODCALLTYPE *PSGetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

void (STDMETHODCALLTYPE *IAGetInputLayout)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11InputLayout **ppInputLayout);

void (STDMETHODCALLTYPE *IAGetVertexBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __out_ecount_opt(NumBuffers)  ID3D11Buffer **ppVertexBuffers,
    /* [annotation] */
    __out_ecount_opt(NumBuffers)  UINT *pStrides,
    /* [annotation] */
    __out_ecount_opt(NumBuffers)  UINT *pOffsets);

void (STDMETHODCALLTYPE *IAGetIndexBuffer)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out_opt  ID3D11Buffer **pIndexBuffer,
    /* [annotation] */
    __out_opt  DXGI_FORMAT *Format,
    /* [annotation] */
    __out_opt  UINT *Offset);

void (STDMETHODCALLTYPE *GSGetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

void (STDMETHODCALLTYPE *GSGetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11GeometryShader **ppGeometryShader,
    /* [annotation] */
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT *pNumClassInstances);

void (STDMETHODCALLTYPE *IAGetPrimitiveTopology)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  D3D11_PRIMITIVE_TOPOLOGY *pTopology);

void (STDMETHODCALLTYPE *VSGetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __out_ecount(NumViews)	ID3D11ShaderResourceView **ppShaderResourceViews);

void (STDMETHODCALLTYPE *VSGetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

void (STDMETHODCALLTYPE *GetPredication)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out_opt  ID3D11Predicate **ppPredicate,
    /* [annotation] */
    __out_opt  BOOL *pPredicateValue);

void (STDMETHODCALLTYPE *GSGetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __out_ecount(NumViews)	ID3D11ShaderResourceView **ppShaderResourceViews);

void (STDMETHODCALLTYPE *GSGetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

void (STDMETHODCALLTYPE *OMGetRenderTargets)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumViews,
    /* [annotation] */
    __out_ecount_opt(NumViews)	ID3D11RenderTargetView **ppRenderTargetViews,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilView **ppDepthStencilView);

void (STDMETHODCALLTYPE *OMGetRenderTargetsAndUnorderedAccessViews)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)  UINT NumRTVs,
    /* [annotation] */
    __out_ecount_opt(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilView **ppDepthStencilView,
    /* [annotation] */
    __in_range(0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1)  UINT UAVStartSlot,
    /* [annotation] */
    __in_range(0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot)	UINT NumUAVs,
    /* [annotation] */
    __out_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

void (STDMETHODCALLTYPE *OMGetBlendState)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out_opt  ID3D11BlendState **ppBlendState,
    /* [annotation] */
    __out_opt  FLOAT BlendFactor[ 4 ],
    /* [annotation] */
    __out_opt  UINT *pSampleMask);

void (STDMETHODCALLTYPE *OMGetDepthStencilState)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out_opt  ID3D11DepthStencilState **ppDepthStencilState,
    /* [annotation] */
    __out_opt  UINT *pStencilRef);

void (STDMETHODCALLTYPE *SOGetTargets)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers,
    /* [annotation] */
    __out_ecount(NumBuffers)  ID3D11Buffer **ppSOTargets);

void (STDMETHODCALLTYPE *RSGetState)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11RasterizerState **ppRasterizerState);

void (STDMETHODCALLTYPE *RSGetViewports)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports,
    /* [annotation] */
    __out_ecount_opt(*pNumViewports)  D3D11_VIEWPORT *pViewports);

void (STDMETHODCALLTYPE *RSGetScissorRects)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects,
    /* [annotation] */
    __out_ecount_opt(*pNumRects)  D3D11_RECT *pRects);

void (STDMETHODCALLTYPE *HSGetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __out_ecount(NumViews)	ID3D11ShaderResourceView **ppShaderResourceViews);

void (STDMETHODCALLTYPE *HSGetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11HullShader **ppHullShader,
    /* [annotation] */
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT *pNumClassInstances);

void (STDMETHODCALLTYPE *HSGetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

void (STDMETHODCALLTYPE *HSGetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

void (STDMETHODCALLTYPE *DSGetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __out_ecount(NumViews)	ID3D11ShaderResourceView **ppShaderResourceViews);

void (STDMETHODCALLTYPE *DSGetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11DomainShader **ppDomainShader,
    /* [annotation] */
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT *pNumClassInstances);

void (STDMETHODCALLTYPE *DSGetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

void (STDMETHODCALLTYPE *DSGetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

void (STDMETHODCALLTYPE *CSGetShaderResources)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot)  UINT NumViews,
    /* [annotation] */
    __out_ecount(NumViews)	ID3D11ShaderResourceView **ppShaderResourceViews);

void (STDMETHODCALLTYPE *CSGetUnorderedAccessViews)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1)  UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot)  UINT NumUAVs,
    /* [annotation] */
    __out_ecount(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews);

void (STDMETHODCALLTYPE *CSGetShader)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __out  ID3D11ComputeShader **ppComputeShader,
    /* [annotation] */
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */
    __inout_opt  UINT *pNumClassInstances);

void (STDMETHODCALLTYPE *CSGetSamplers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot)	UINT NumSamplers,
    /* [annotation] */
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers);

void (STDMETHODCALLTYPE *CSGetConstantBuffers)(
    ID3D11DeviceContext * This,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1)	UINT StartSlot,
    /* [annotation] */
    __in_range(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot)	UINT NumBuffers,
    /* [annotation] */
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers);

void (STDMETHODCALLTYPE *ClearState)(
    ID3D11DeviceContext * This);

void (STDMETHODCALLTYPE *Flush)(
    ID3D11DeviceContext * This);

D3D11_DEVICE_CONTEXT_TYPE(STDMETHODCALLTYPE *GetType)(
    ID3D11DeviceContext * This);

UINT(STDMETHODCALLTYPE *GetContextFlags)(
    ID3D11DeviceContext * This);

HRESULT(STDMETHODCALLTYPE *FinishCommandList)(
    ID3D11DeviceContext * This,
    BOOL RestoreDeferredContextState,
    /* [annotation] */
    __out_opt  ID3D11CommandList **ppCommandList);


#endif


#define  DEVICE_CONTEXT_IN() do{} while(0)

#define  DEVICE_CONTEXT_OUT() do{} while(0)

int UnRegisterDeviceContext(ID3D11DeviceContext *pContext)
{
    return 0;
}

int RegisterDeviceContext(ID3D11DeviceContext *pContext)
{
    return 0;
}

class CD3D11DeviceContextHook : public ID3D11DeviceContext
{
private:
    ID3D11DeviceContext *m_ptr;
public:
    CD3D11DeviceContextHook(ID3D11DeviceContext *pPtr):m_ptr(pPtr) {};

public:
    COM_METHOD(HRESULT, QueryInterface)(THIS_ REFIID riid, void** ppvObj)
    {
        HRESULT hr;
        DEVICE_CONTEXT_IN();
        hr =  m_ptr->QueryInterface(riid, ppvObj);
        DEVICE_CONTEXT_OUT();
        return hr;
    }
    COM_METHOD(ULONG, AddRef)(THIS)
    {
        ULONG ret;
        DEVICE_CONTEXT_IN();
        ret=  m_ptr->AddRef();
        DEVICE_CONTEXT_OUT();
        return ret;
    }
    COM_METHOD(ULONG, Release)(THIS)
    {
        ULONG uret,realret;
        int ret;

        DEVICE_CONTEXT_IN();
        uret = m_ptr->Release();
        realret = uret;
        /*it means that is the just one ,we should return for the job*/
        if(uret == 1)
        {
            ret = UnRegisterDeviceContext(m_ptr);
            /*if 1 it means not release one*/
            realret = ret;
        }
        DEVICE_CONTEXT_OUT();
        return realret;
    }

    COM_METHOD(void,GetDevice)(THIS_ ID3D11Device **ppDevice)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GetDevice(ppDevice);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(HRESULT,GetPrivateData)(THIS_ REFGUID guid,UINT *pDataSize,void *pData)
    {
        HRESULT hr;
        DEVICE_CONTEXT_IN();
        hr =  m_ptr->GetPrivateData(guid,pDataSize,pData);
        DEVICE_CONTEXT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetPrivateData)(THIS_ REFGUID guid,UINT DataSize,const void *pData)
    {
        HRESULT hr;
        DEVICE_CONTEXT_IN();
        hr =  m_ptr->SetPrivateData(guid,DataSize,pData);
        DEVICE_CONTEXT_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetPrivateDataInterface)(THIS_ REFGUID guid,const IUnknown *pData)
    {
        HRESULT hr;
        DEVICE_CONTEXT_IN();
        hr =  m_ptr->SetPrivateDataInterface(guid,pData);
        DEVICE_CONTEXT_OUT();
        return hr;
    }

    COM_METHOD(void,VSSetConstantBuffers)(THIS_ UINT StartSlot,UINT NumBuffers,ID3D11Buffer *const *ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->VSSetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,PSSetShaderResources)(THIS_ UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView *const *ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->PSSetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,PSSetShader)(THIS_ ID3D11PixelShader *pPixelShader,ID3D11ClassInstance *const *ppClassInstances,UINT NumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->PSSetShader(pPixelShader,ppClassInstances,NumClassInstances);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,PSSetSamplers)(THIS_ UINT StartSlot,UINT NumSamplers,ID3D11SamplerState *const *ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->PSSetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,VSSetShader)(THIS_ ID3D11VertexShader *pVertexShader,ID3D11ClassInstance *const *ppClassInstances,	UINT NumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->VSSetShader(pVertexShader,ppClassInstances,NumClassInstances);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,DrawIndexed)(THIS_ UINT IndexCount,UINT StartIndexLocation,INT BaseVertexLocation)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DrawIndexed(IndexCount,StartIndexLocation,BaseVertexLocation);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,Draw)(THIS_ UINT VertexCount,UINT StartVertexLocation)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->Draw(VertexCount,StartVertexLocation);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(HRESULT,Map)(THIS_ ID3D11Resource *pResource,UINT Subresource,D3D11_MAP MapType,UINT MapFlags,D3D11_MAPPED_SUBRESOURCE *pMappedResource)
    {
        HRESULT hr;
        DEVICE_CONTEXT_IN();
        hr =  m_ptr->Map(pResource,Subresource,MapType,MapFlags,pMappedResource);
        DEVICE_CONTEXT_OUT();
        return hr;
    }

    COM_METHOD(void ,Unmap)(THIS_ ID3D11Resource *pResource,UINT Subresource)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->Unmap(pResource,Subresource);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,PSSetConstantBuffers)(THIS_ UINT StartSlot,UINT NumBuffers,ID3D11Buffer *const *ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->PSSetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,IASetInputLayout)(THIS_ ID3D11InputLayout *pInputLayout)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->IASetInputLayout(pInputLayout);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,IASetVertexBuffers)(THIS_ UINT StartSlot,UINT NumBuffers,ID3D11Buffer *const *ppVertexBuffers,const UINT *pStrides,const UINT *pOffsets)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->IASetVertexBuffers(StartSlot,NumBuffers,ppVertexBuffers,pStrides,pOffsets);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,IASetIndexBuffer)(THIS_ ID3D11Buffer *pIndexBuffer,DXGI_FORMAT Format,UINT Offset)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->IASetIndexBuffer(pIndexBuffer,Format,Offset);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,DrawIndexedInstanced)(THIS_ UINT IndexCountPerInstance,UINT InstanceCount,UINT StartIndexLocation,INT BaseVertexLocation,UINT StartInstanceLocation)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DrawIndexedInstanced(IndexCountPerInstance,InstanceCount,StartIndexLocation,BaseVertexLocation,StartInstanceLocation);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,DrawInstanced)(THIS_ UINT VertexCountPerInstance,UINT InstanceCount,UINT StartVertexLocation,UINT StartInstanceLocation)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DrawInstanced(VertexCountPerInstance,InstanceCount,StartVertexLocation,StartInstanceLocation);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,GSSetConstantBuffers)(THIS_ UINT StartSlot,UINT NumBuffers,ID3D11Buffer *const *ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GSSetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,GSSetShader)(THIS_ ID3D11GeometryShader *pShader,ID3D11ClassInstance *const *ppClassInstances,	UINT NumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GSSetShader(pShader,ppClassInstances,NumClassInstances);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,IASetPrimitiveTopology)(THIS_ D3D11_PRIMITIVE_TOPOLOGY Topology)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->IASetPrimitiveTopology(Topology);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,VSSetShaderResources)(THIS_ UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView *const *ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->VSSetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,VSSetSamplers)(THIS_ UINT StartSlot,UINT NumSamplers,ID3D11SamplerState *const *ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->VSSetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,Begin)(THIS_ ID3D11Asynchronous *pAsync)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->Begin(pAsync);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,End)(THIS_ ID3D11Asynchronous *pAsync)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->End(pAsync);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(HRESULT,GetData)(THIS_ ID3D11Asynchronous *pAsync,void *pData,UINT DataSize,UINT GetDataFlags)
    {
        HRESULT hr;
        DEVICE_CONTEXT_IN();
        hr =  m_ptr->GetData(pAsync,pData,DataSize,GetDataFlags);
        DEVICE_CONTEXT_OUT();
        return hr;
    }

    COM_METHOD(void,SetPredication)(THIS_ ID3D11Predicate *pPredicate,BOOL PredicateValue)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->SetPredication(pPredicate,PredicateValue);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,GSSetShaderResources)(THIS_ UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView *const *ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GSSetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,GSSetSamplers)(THIS_ UINT StartSlot,UINT NumSamplers,ID3D11SamplerState *const *ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GSSetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,OMSetRenderTargets)(THIS_ UINT NumViews,ID3D11RenderTargetView *const *ppRenderTargetViews,ID3D11DepthStencilView *pDepthStencilView)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->OMSetRenderTargets(NumViews,ppRenderTargetViews,pDepthStencilView);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,OMSetRenderTargetsAndUnorderedAccessViews)(THIS_ UINT NumRTVs,ID3D11RenderTargetView *const *ppRenderTargetViews,ID3D11DepthStencilView *pDepthStencilView,
            UINT UAVStartSlot,UINT NumUAVs,ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,const UINT *pUAVInitialCounts)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs,ppRenderTargetViews,pDepthStencilView,UAVStartSlot,NumUAVs,ppUnorderedAccessViews,pUAVInitialCounts);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,OMSetBlendState)(THIS_ ID3D11BlendState *pBlendState,const FLOAT BlendFactor[ 4 ],UINT SampleMask)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->OMSetBlendState(pBlendState,BlendFactor,SampleMask);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,OMSetDepthStencilState)(THIS_ ID3D11DepthStencilState *pDepthStencilState,UINT StencilRef)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->OMSetDepthStencilState(pDepthStencilState,StencilRef);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,SOSetTargets)(THIS_ UINT NumBuffers,ID3D11Buffer *const *ppSOTargets,const UINT *pOffsets)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->SOSetTargets(NumBuffers,ppSOTargets,pOffsets);
        DEVICE_CONTEXT_OUT();
        return;
    }

    COM_METHOD(void,DrawAuto)(THIS)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DrawAuto();
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DrawIndexedInstancedIndirect)(THIS_ ID3D11Buffer *pBufferForArgs,UINT AlignedByteOffsetForArgs)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DrawIndexedInstancedIndirect(pBufferForArgs,AlignedByteOffsetForArgs);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void ,DrawInstancedIndirect)(THIS_ ID3D11Buffer *pBufferForArgs,UINT AlignedByteOffsetForArgs)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DrawInstancedIndirect(pBufferForArgs,AlignedByteOffsetForArgs);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,Dispatch)(THIS_  UINT ThreadGroupCountX,UINT ThreadGroupCountY,UINT ThreadGroupCountZ)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->Dispatch(ThreadGroupCountX,ThreadGroupCountY,ThreadGroupCountZ);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DispatchIndirect)(THIS_  ID3D11Buffer *pBufferForArgs,UINT AlignedByteOffsetForArgs)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DispatchIndirect(pBufferForArgs,AlignedByteOffsetForArgs);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,RSSetState)(THIS_  ID3D11RasterizerState *pRasterizerState)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->RSSetState(pRasterizerState);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,RSSetViewports)(THIS_  UINT NumViewports,const D3D11_VIEWPORT *pViewports)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->RSSetViewports(NumViewports,pViewports);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,RSSetScissorRects)(THIS_ UINT NumRects,const D3D11_RECT *pRects)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->RSSetScissorRects(NumRects,pRects);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CopySubresourceRegion)(THIS_ ID3D11Resource *pDstResource,UINT DstSubresource,
                                           UINT DstX,UINT DstY,UINT DstZ,ID3D11Resource *pSrcResource,UINT SrcSubresource,const D3D11_BOX *pSrcBox)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CopySubresourceRegion(pDstResource,DstSubresource,DstX,DstY,DstZ,pSrcResource,SrcSubresource,pSrcBox);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CopyResource)(THIS_ ID3D11Resource *pDstResource,ID3D11Resource *pSrcResource)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CopyResource(pDstResource,pSrcResource);
        DEVICE_CONTEXT_OUT();
        return ;
    }


    COM_METHOD(void,UpdateSubresource)(THIS_ ID3D11Resource *pDstResource,UINT DstSubresource,
                                       const D3D11_BOX *pDstBox,const void *pSrcData,UINT SrcRowPitch,UINT SrcDepthPitch)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->UpdateSubresource(pDstResource,DstSubresource,pDstBox,pSrcData,SrcRowPitch,SrcDepthPitch);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CopyStructureCount)(THIS_ ID3D11Buffer *pDstBuffer,UINT DstAlignedByteOffset,ID3D11UnorderedAccessView *pSrcView)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CopyStructureCount(pDstBuffer,DstAlignedByteOffset,pSrcView);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,ClearRenderTargetView)(THIS_ ID3D11RenderTargetView *pRenderTargetView,const FLOAT ColorRGBA[ 4 ])
    {
        DEVICE_CONTEXT_IN();
        m_ptr->ClearRenderTargetView(pRenderTargetView,ColorRGBA);
        DEVICE_CONTEXT_OUT();
        return ;
    }

};



#if 0

/********************************************
* this is the interface of ID3D11Device
*********************************************/

HRESULT(STDMETHODCALLTYPE *QueryInterface)(
    ID3D11DeviceChild * This,
    /* [in] */ REFIID riid,
    /* [annotation][iid_is][out] */
    __RPC__deref_out  void **ppvObject);

ULONG(STDMETHODCALLTYPE *AddRef)(
    ID3D11DeviceChild * This);

ULONG(STDMETHODCALLTYPE *Release)(
    ID3D11DeviceChild * This);

void (STDMETHODCALLTYPE *GetDevice)(
    ID3D11DeviceChild * This,
    /* [annotation] */
    __out  ID3D11Device **ppDevice);

HRESULT(STDMETHODCALLTYPE *GetPrivateData)(
    ID3D11DeviceChild * This,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __inout  UINT *pDataSize,
    /* [annotation] */
    __out_bcount_opt(*pDataSize)	void *pData);

HRESULT(STDMETHODCALLTYPE *SetPrivateData)(
    ID3D11DeviceChild * This,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in  UINT DataSize,
    /* [annotation] */
    __in_bcount_opt(DataSize)  const void *pData);

HRESULT(STDMETHODCALLTYPE *SetPrivateDataInterface)(
    ID3D11DeviceChild * This,
    /* [annotation] */
    __in  REFGUID guid,
    /* [annotation] */
    __in_opt  const IUnknown *pData);

#endif





/***********************************************************************
* this is the detours handle functions ,and it will handle for the handle ,we should make the
* dummy class and interface for it
***********************************************************************/
static HRESULT(WINAPI* D3D11CreateDeviceAndSwapChainNext)(IDXGIAdapter *pAdapter,
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
    DEBUG_INFO("call D3D11CreateDeviceAndSwapChainNext (0x%08x)\n",hr);
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
    if(ret < 0)
    {
        return ret;
    }

    return 0;

}


void RotineClearD11(void)
{
    return;
}
