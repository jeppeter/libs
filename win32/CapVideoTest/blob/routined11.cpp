
#include "routined11.h"
#include "d3d11.h"
#include "..\\detours\\detours.h"
#include "..\\common\\output_debug.h"

#pragma comment(lib,"d3d11.lib")

#define COM_METHOD(TYPE, METHOD) TYPE STDMETHODCALLTYPE METHOD




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

    COM_METHOD(void,ClearUnorderedAccessViewUint)(THIS_  ID3D11UnorderedAccessView *pUnorderedAccessView,const UINT Values[ 4 ])
    {
        DEVICE_CONTEXT_IN();
        m_ptr->ClearUnorderedAccessViewUint(pUnorderedAccessView,Values);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,ClearUnorderedAccessViewFloat)(THIS_  ID3D11UnorderedAccessView *pUnorderedAccessView,const FLOAT Values[ 4 ])
    {
        DEVICE_CONTEXT_IN();
        m_ptr->ClearUnorderedAccessViewFloat(pUnorderedAccessView,Values);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,ClearDepthStencilView)(THIS_  ID3D11DepthStencilView *pDepthStencilView,UINT ClearFlags,
                                           FLOAT Depth,UINT8 Stencil)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->ClearDepthStencilView(pDepthStencilView,ClearFlags,Depth,Stencil);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,GenerateMips)(THIS_  ID3D11ShaderResourceView *pShaderResourceView)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GenerateMips(pShaderResourceView);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,SetResourceMinLOD)(THIS_  ID3D11Resource *pResource,    FLOAT MinLOD)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->SetResourceMinLOD(pResource,MinLOD);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(FLOAT,GetResourceMinLOD)(THIS_ ID3D11Resource *pResource)
    {
        float f;
        DEVICE_CONTEXT_IN();
        f = m_ptr->GetResourceMinLOD(pResource);
        DEVICE_CONTEXT_OUT();
        return f;
    }

    COM_METHOD(void,ResolveSubresource)(THIS_ ID3D11Resource *pDstResource,UINT DstSubresource,
                                        ID3D11Resource *pSrcResource,UINT SrcSubresource,DXGI_FORMAT Format)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->ResolveSubresource(pDstResource,DstSubresource,pSrcResource,SrcSubresource,Format);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,ExecuteCommandList)(THIS_  ID3D11CommandList *pCommandList,    BOOL RestoreContextState)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->ExecuteCommandList(pCommandList, RestoreContextState);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,HSSetShaderResources)(THIS_  UINT StartSlot,UINT NumViews,
                                          ID3D11ShaderResourceView *const *ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->HSSetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,HSSetShader)(THIS_  ID3D11HullShader *pHullShader,ID3D11ClassInstance *const *ppClassInstances,    UINT NumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->HSSetShader(pHullShader,ppClassInstances,NumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,HSSetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState *const *ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->HSSetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,HSSetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer *const *ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->HSSetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DSSetShaderResources)(THIS_  UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView *const *ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DSSetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DSSetShader)(THIS_  ID3D11DomainShader *pDomainShader,ID3D11ClassInstance *const *ppClassInstances,UINT NumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DSSetShader(pDomainShader,ppClassInstances,NumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DSSetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState *const *ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DSSetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DSSetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer *const *ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DSSetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSSetShaderResources)(THIS_  UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView *const *ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSSetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSSetUnorderedAccessViews)(THIS_  UINT StartSlot,UINT NumUAVs,
            ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,const UINT *pUAVInitialCounts)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSSetUnorderedAccessViews(StartSlot,NumUAVs,ppUnorderedAccessViews,pUAVInitialCounts);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSSetShader)(THIS_  ID3D11ComputeShader *pComputeShader,ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSSetShader(pComputeShader,ppClassInstances,NumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSSetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState *const *ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSSetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSSetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer *const *ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSSetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,VSGetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer **ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->VSGetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,PSGetShaderResources)(THIS_  UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView **ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->PSGetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,PSGetShader)(THIS_  ID3D11PixelShader **ppPixelShader,ID3D11ClassInstance **ppClassInstances,UINT *pNumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->PSGetShader(ppPixelShader,ppClassInstances,pNumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }
    COM_METHOD(void,PSGetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState **ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->PSGetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,VSGetShader)(THIS_  ID3D11VertexShader **ppVertexShader,ID3D11ClassInstance **ppClassInstances,UINT *pNumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->VSGetShader(ppVertexShader,ppClassInstances,pNumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,PSGetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer **ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->PSGetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,IAGetInputLayout)(THIS_  ID3D11InputLayout **ppInputLayout)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->IAGetInputLayout(ppInputLayout);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,IAGetVertexBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer **ppVertexBuffers,
                                        UINT *pStrides,UINT *pOffsets)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->IAGetVertexBuffers(StartSlot,NumBuffers,ppVertexBuffers,pStrides,pOffsets);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,IAGetIndexBuffer)(THIS_  ID3D11Buffer **pIndexBuffer,DXGI_FORMAT *Format,UINT *Offset)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->IAGetIndexBuffer(pIndexBuffer,Format,Offset);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,GSGetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer **ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GSGetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,GSGetShader)(THIS_  ID3D11GeometryShader **ppGeometryShader,ID3D11ClassInstance **ppClassInstances,UINT *pNumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GSGetShader(ppGeometryShader,ppClassInstances,pNumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,IAGetPrimitiveTopology)(THIS_  D3D11_PRIMITIVE_TOPOLOGY *pTopology)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->IAGetPrimitiveTopology(pTopology);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,VSGetShaderResources)(THIS_ UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView **ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->VSGetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,VSGetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState **ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->VSGetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,GetPredication)(THIS_  ID3D11Predicate **ppPredicate,BOOL *pPredicateValue)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GetPredication(ppPredicate,pPredicateValue);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,GSGetShaderResources)(THIS_  UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView **ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GSGetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,GSGetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState **ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->GSGetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,OMGetRenderTargets)(THIS_  UINT NumViews,ID3D11RenderTargetView **ppRenderTargetViews,ID3D11DepthStencilView **ppDepthStencilView)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->OMGetRenderTargets(NumViews,ppRenderTargetViews,ppDepthStencilView);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,OMGetRenderTargetsAndUnorderedAccessViews)(THIS_  UINT NumRTVs,ID3D11RenderTargetView **ppRenderTargetViews,
            ID3D11DepthStencilView **ppDepthStencilView,UINT UAVStartSlot,UINT NumUAVs,ID3D11UnorderedAccessView **ppUnorderedAccessViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs,ppRenderTargetViews,ppDepthStencilView,UAVStartSlot,NumUAVs,ppUnorderedAccessViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,OMGetBlendState)(THIS_  ID3D11BlendState **ppBlendState,FLOAT BlendFactor[ 4 ],UINT *pSampleMask)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->OMGetBlendState(ppBlendState,BlendFactor,pSampleMask);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,OMGetDepthStencilState)(THIS_  ID3D11DepthStencilState **ppDepthStencilState,UINT *pStencilRef)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->OMGetDepthStencilState(ppDepthStencilState,pStencilRef);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,SOGetTargets)(THIS_  UINT NumBuffers,ID3D11Buffer **ppSOTargets)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->SOGetTargets(NumBuffers,ppSOTargets);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,RSGetState)(THIS_  ID3D11RasterizerState **ppRasterizerState)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->RSGetState(ppRasterizerState);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,RSGetViewports)(THIS_  UINT *pNumViewports,D3D11_VIEWPORT *pViewports)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->RSGetViewports(pNumViewports,pViewports);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,RSGetScissorRects)(THIS_ UINT *pNumRects,D3D11_RECT *pRects)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->RSGetScissorRects(pNumRects,pRects);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,HSGetShaderResources)(THIS_ UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView **ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->HSGetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,HSGetShader)(THIS_  ID3D11HullShader **ppHullShader,ID3D11ClassInstance **ppClassInstances,UINT *pNumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->HSGetShader(ppHullShader,ppClassInstances,pNumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,HSGetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState **ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->HSGetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,HSGetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer **ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->HSGetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DSGetShaderResources)(THIS_  UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView **ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DSGetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DSGetShader)(THIS_  ID3D11DomainShader **ppDomainShader,ID3D11ClassInstance **ppClassInstances,UINT *pNumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DSGetShader(ppDomainShader,ppClassInstances,pNumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DSGetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState **ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DSGetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,DSGetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer **ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->DSGetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSGetShaderResources)(THIS_  UINT StartSlot,UINT NumViews,ID3D11ShaderResourceView **ppShaderResourceViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSGetShaderResources(StartSlot,NumViews,ppShaderResourceViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSGetUnorderedAccessViews)(THIS_  UINT StartSlot,UINT NumUAVs,ID3D11UnorderedAccessView **ppUnorderedAccessViews)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSGetUnorderedAccessViews(StartSlot,NumUAVs,ppUnorderedAccessViews);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSGetShader)(THIS_  ID3D11ComputeShader **ppComputeShader,ID3D11ClassInstance **ppClassInstances,UINT *pNumClassInstances)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSGetShader(ppComputeShader,ppClassInstances,pNumClassInstances);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSGetSamplers)(THIS_  UINT StartSlot,UINT NumSamplers,ID3D11SamplerState **ppSamplers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSGetSamplers(StartSlot,NumSamplers,ppSamplers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,CSGetConstantBuffers)(THIS_  UINT StartSlot,UINT NumBuffers,ID3D11Buffer **ppConstantBuffers)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->CSGetConstantBuffers(StartSlot,NumBuffers,ppConstantBuffers);
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,ClearState)(THIS)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->ClearState();
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(void,Flush)(THIS)
    {
        DEVICE_CONTEXT_IN();
        m_ptr->Flush();
        DEVICE_CONTEXT_OUT();
        return ;
    }

    COM_METHOD(D3D11_DEVICE_CONTEXT_TYPE,GetType)(THIS)
    {
        D3D11_DEVICE_CONTEXT_TYPE type;
        DEVICE_CONTEXT_IN();
        type = m_ptr->GetType();
        DEVICE_CONTEXT_OUT();
        return type;
    }

    COM_METHOD(UINT,GetContextFlags)(THIS)
    {
        UINT ui;
        DEVICE_CONTEXT_IN();
        ui = m_ptr->GetContextFlags();
        DEVICE_CONTEXT_OUT();
        return ui;
    }

    COM_METHOD(HRESULT,FinishCommandList)(THIS_ 	BOOL RestoreDeferredContextState,ID3D11CommandList **ppCommandList)
    {
        HRESULT hr;
        DEVICE_CONTEXT_IN();
        hr = m_ptr->FinishCommandList(RestoreDeferredContextState,ppCommandList);
        DEVICE_CONTEXT_OUT();
        return hr;
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

#define  D11_DEVICE_IN()  do{}while(0)
#define  D11_DEVICE_OUT()  do{}while(0)

int UnRegisterDevice(ID3D11Device* pPtr)
{
    return 0;
}



class CD3D11DeviceHook : public ID3D11Device
{
private:
    ID3D11Device *m_ptr;
public:
    CD3D11DeviceHook(ID3D11Device* pPtr) : m_ptr(pPtr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_  REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG ul;
        D11_DEVICE_IN();
        ul = m_ptr->AddRef();
        D11_DEVICE_OUT();
        return ul;
    }

    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG ul;
        D11_DEVICE_IN();
        ul = m_ptr->Release();
        if(ul == 1)
        {
            ul = UnRegisterDevice(m_ptr);
        }
        D11_DEVICE_OUT();
        return ul;
    }

    COM_METHOD(HRESULT,CreateBuffer)(THIS_  const D3D11_BUFFER_DESC *pDesc,const D3D11_SUBRESOURCE_DATA *pInitialData,ID3D11Buffer **ppBuffer)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateBuffer(pDesc,pInitialData,ppBuffer);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateTexture1D)(THIS_  const D3D11_TEXTURE1D_DESC *pDesc,const D3D11_SUBRESOURCE_DATA *pInitialData,ID3D11Texture1D **ppTexture1D)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateTexture1D(pDesc,pInitialData,ppTexture1D);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateTexture2D)(THIS_  const D3D11_TEXTURE2D_DESC *pDesc,const D3D11_SUBRESOURCE_DATA *pInitialData,ID3D11Texture2D **ppTexture2D)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateTexture2D(pDesc,pInitialData,ppTexture2D);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateTexture3D)(THIS_  const D3D11_TEXTURE3D_DESC *pDesc,const D3D11_SUBRESOURCE_DATA *pInitialData,ID3D11Texture3D **ppTexture3D)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateTexture3D(pDesc,pInitialData,ppTexture3D);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateShaderResourceView)(THIS_  ID3D11Resource *pResource,const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,ID3D11ShaderResourceView **ppSRView)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateShaderResourceView(pResource,pDesc,ppSRView);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateUnorderedAccessView)(THIS_  ID3D11Resource *pResource,const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,ID3D11UnorderedAccessView **ppUAView)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateUnorderedAccessView(pResource,pDesc,ppUAView);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateRenderTargetView)(THIS_  ID3D11Resource *pResource,const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,ID3D11RenderTargetView **ppRTView)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateRenderTargetView(pResource,pDesc,ppRTView);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateDepthStencilView)(THIS_  ID3D11Resource *pResource,const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,ID3D11DepthStencilView **ppDepthStencilView)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateDepthStencilView(pResource,pDesc,ppDepthStencilView);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateInputLayout)(THIS_  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,UINT NumElements,
                                          const void *pShaderBytecodeWithInputSignature,SIZE_T BytecodeLength,ID3D11InputLayout **ppInputLayout)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateInputLayout(pInputElementDescs,NumElements,pShaderBytecodeWithInputSignature,BytecodeLength,ppInputLayout);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateVertexShader)(THIS_  const void *pShaderBytecode,SIZE_T BytecodeLength,ID3D11ClassLinkage *pClassLinkage,ID3D11VertexShader **ppVertexShader)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateVertexShader(pShaderBytecode,BytecodeLength,pClassLinkage,ppVertexShader);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,CreateGeometryShader)(THIS_  const void *pShaderBytecode,SIZE_T BytecodeLength,ID3D11ClassLinkage *pClassLinkage,
            ID3D11GeometryShader **ppGeometryShader)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateGeometryShader(pShaderBytecode,BytecodeLength,pClassLinkage,ppGeometryShader);
        D11_DEVICE_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,CreateGeometryShaderWithStreamOutput)(THIS_  const void *pShaderBytecode,SIZE_T BytecodeLength,const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
            UINT NumEntries,const UINT *pBufferStrides,UINT NumStrides,UINT RasterizedStream,ID3D11ClassLinkage *pClassLinkage,ID3D11GeometryShader **ppGeometryShader)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateGeometryShaderWithStreamOutput(pShaderBytecode,BytecodeLength,pSODeclaration,NumEntries,pBufferStrides,NumStrides,RasterizedStream,pClassLinkage,ppGeometryShader);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT ,CreatePixelShader)(THIS_  const void *pShaderBytecode,SIZE_T BytecodeLength,ID3D11ClassLinkage *pClassLinkage,ID3D11PixelShader **ppPixelShader)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreatePixelShader(pShaderBytecode,BytecodeLength,pClassLinkage,ppPixelShader);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateHullShader)(THIS_  const void *pShaderBytecode,SIZE_T BytecodeLength,ID3D11ClassLinkage *pClassLinkage,ID3D11HullShader **ppHullShader)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateHullShader(pShaderBytecode,BytecodeLength,pClassLinkage,ppHullShader);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateDomainShader)(THIS_  const void *pShaderBytecode,SIZE_T BytecodeLength,ID3D11ClassLinkage *pClassLinkage,ID3D11DomainShader **ppDomainShader)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateDomainShader(pShaderBytecode,BytecodeLength,pClassLinkage,ppDomainShader);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateComputeShader)(THIS_  const void *pShaderBytecode,SIZE_T BytecodeLength,ID3D11ClassLinkage *pClassLinkage,ID3D11ComputeShader **ppComputeShader)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateComputeShader(pShaderBytecode,BytecodeLength,pClassLinkage,ppComputeShader);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateClassLinkage)(THIS_  ID3D11ClassLinkage **ppLinkage)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateClassLinkage(ppLinkage);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateBlendState)(THIS_  const D3D11_BLEND_DESC *pBlendStateDesc,ID3D11BlendState **ppBlendState)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateBlendState(pBlendStateDesc,ppBlendState);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateDepthStencilState)(THIS_ const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,ID3D11DepthStencilState **ppDepthStencilState)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateDepthStencilState(pDepthStencilDesc,ppDepthStencilState);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateRasterizerState)(THIS_  const D3D11_RASTERIZER_DESC *pRasterizerDesc,ID3D11RasterizerState **ppRasterizerState)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateRasterizerState(pRasterizerDesc,ppRasterizerState);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateSamplerState)(THIS_  const D3D11_SAMPLER_DESC *pSamplerDesc,ID3D11SamplerState **ppSamplerState)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateSamplerState(pSamplerDesc,ppSamplerState);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateQuery)(THIS_  const D3D11_QUERY_DESC *pQueryDesc,ID3D11Query **ppQuery)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateQuery(pQueryDesc,ppQuery);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreatePredicate)(THIS_  const D3D11_QUERY_DESC *pPredicateDesc,ID3D11Predicate **ppPredicate)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreatePredicate(pPredicateDesc,ppPredicate);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateCounter)(THIS_  const D3D11_COUNTER_DESC *pCounterDesc,ID3D11Counter **ppCounter)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateCounter(pCounterDesc,ppCounter);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateDeferredContext)(THIS_  	UINT ContextFlags,ID3D11DeviceContext **ppDeferredContext)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CreateDeferredContext(ContextFlags,ppDeferredContext);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,OpenSharedResource)(THIS_  HANDLE hResource,REFIID ReturnedInterface,void **ppResource)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->OpenSharedResource(hResource,ReturnedInterface,ppResource);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CheckFormatSupport)(THIS_ DXGI_FORMAT Format,UINT *pFormatSupport)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CheckFormatSupport(Format,pFormatSupport);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CheckMultisampleQualityLevels)(THIS_  DXGI_FORMAT Format,UINT SampleCount,UINT *pNumQualityLevels)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CheckMultisampleQualityLevels(Format,SampleCount,pNumQualityLevels);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(void,CheckCounterInfo)(THIS_ D3D11_COUNTER_INFO *pCounterInfo)
    {
        D11_DEVICE_IN();
        m_ptr->CheckCounterInfo(pCounterInfo);
        D11_DEVICE_OUT();
        return;
    }

    COM_METHOD(HRESULT,CheckCounter)(THIS_ const D3D11_COUNTER_DESC *pDesc,D3D11_COUNTER_TYPE *pType,UINT *pActiveCounters,LPSTR szName,
                                     UINT *pNameLength,LPSTR szUnits,UINT *pUnitsLength,LPSTR szDescription,UINT *pDescriptionLength)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CheckCounter(pDesc,pType,pActiveCounters,szName,pNameLength,szUnits,pUnitsLength,szDescription,pDescriptionLength);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CheckFeatureSupport)(THIS_  D3D11_FEATURE Feature,void *pFeatureSupportData,UINT FeatureSupportDataSize)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->CheckFeatureSupport(Feature,pFeatureSupportData,FeatureSupportDataSize);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetPrivateData)(THIS_  REFGUID guid,UINT *pDataSize,void *pData)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->GetPrivateData(guid,pDataSize,pData);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetPrivateData)(THIS_  REFGUID guid,UINT DataSize,const void *pData)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->SetPrivateData(guid,DataSize,pData);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetPrivateDataInterface)(THIS_  REFGUID guid,const IUnknown *pData)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->SetPrivateDataInterface(guid,pData);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(D3D_FEATURE_LEVEL,GetFeatureLevel)(THIS)
    {
        D3D_FEATURE_LEVEL ulevel;
        D11_DEVICE_IN();
        ulevel = m_ptr->GetFeatureLevel();
        D11_DEVICE_OUT();
        return ulevel;
    }

    COM_METHOD(UINT,GetCreationFlags)(THIS)
    {
        UINT ui;
        D11_DEVICE_IN();
        ui = m_ptr->GetCreationFlags();
        D11_DEVICE_OUT();
        return ui;
    }

    COM_METHOD(HRESULT,GetDeviceRemovedReason)(THIS)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->GetDeviceRemovedReason();
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(void,GetImmediateContext)(THIS_  ID3D11DeviceContext **ppImmediateContext)
    {
        D11_DEVICE_IN();
        m_ptr->GetImmediateContext(ppImmediateContext);
        D11_DEVICE_OUT();
        return ;
    }

    COM_METHOD(HRESULT,SetExceptionMode)(THIS_      UINT RaiseFlags)
    {
        HRESULT hr;
        D11_DEVICE_IN();
        hr = m_ptr->SetExceptionMode(RaiseFlags);
        D11_DEVICE_OUT();
        return hr;
    }

    COM_METHOD(UINT,GetExceptionMode)(THIS)
    {
        UINT ui;
        D11_DEVICE_IN();
        ui = m_ptr->GetExceptionMode();
        D11_DEVICE_OUT();
        return ui;
    }
};




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
