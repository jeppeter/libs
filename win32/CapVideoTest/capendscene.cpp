#include <d3d9.h> 
#include <d3dx9.h> 

#pragma comment (lib, "d3d9.lib") 
#pragma comment (lib, "d3dx9.lib") 

#pragma once 

typedef HRESULT (WINAPI *CreateDevice_t) (IDirect3D9* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,  
                     DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,  
                     IDirect3DDevice9 ** ppReturnedDeviceInterface); 
typedef HRESULT (WINAPI *EndScene_t) (IDirect3DDevice9* surface); 


 CreateDevice_t D3DCreateDevice_orig; 
 EndScene_t D3DEndScene_orig; 

 HRESULT WINAPI D3DCreateDevice_hook (IDirect3D9* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,  
                     DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,  
                     IDirect3DDevice9 ** ppReturnedDeviceInterface); 
 HRESULT WINAPI D3DEndScene_hook (IDirect3DDevice9* device); 

 PDWORD IDirect3D9_vtable = NULL; 

#define CREATEDEVICE_VTI 16 
#define ENDSCENE_VTI 42 

 HRESULT WINAPI HookCreateDevice (); 
 DWORD WINAPI VTablePatchThread (LPVOID threadParam); 

BOOL APIENTRY DllMain (HMODULE hModule, 
                       DWORD ul_reason_for_call, 
                       LPVOID lpReserved 
                     )
{ 
   //HMODULE d3dmodule; 
   //PBYTE funcAddress; 
    switch (ul_reason_for_call) 
    { 
    case DLL_PROCESS_ATTACH: 
        //MessageBoxA (NULL, "DLL Injected", "DLL Injected", MB_ICONEXCLAMATION); 
        if (HookCreateDevice () == D3D_OK) 
        { 
        //pBits = malloc (10000000); 
            return TRUE; 
        } else { 
            return FALSE; 
        }

        break; 
    case DLL_THREAD_ATTACH: 
       //MessageBoxA (NULL, "DLL_THREAD_ATTACH", "DLL", MB_ICONEXCLAMATION); 
        break; 
    case DLL_THREAD_DETACH: 
       //MessageBoxA (NULL, "DLL_THREAD_DETACH", "DLL", MB_ICONEXCLAMATION); 
        break; 
    case DLL_PROCESS_DETACH: 
       //MessageBoxA (NULL, "DLL_PROCESS_DETACH", "DLL", MB_ICONEXCLAMATION); 
        break; 
    }
    return TRUE; 
}

HRESULT WINAPI HookCreateDevice () 
{ 
    IDirect3D9* device = Direct3DCreate9 (D3D_SDK_VERSION); 
    if (! device) 
    { 
        return D3DERR_INVALIDCALL; 
    }
    IDirect3D9_vtable = (DWORD *) * (DWORD *) device; 
    device-> Release (); 

    DWORD protectFlag; 
    if (VirtualProtect (&IDirect3D9_vtable [CREATEDEVICE_VTI], sizeof (DWORD), PAGE_READWRITE, &protectFlag)) 
    { 
        *(DWORD *) &D3DCreateDevice_orig = IDirect3D9_vtable [CREATEDEVICE_VTI]; 
        *(DWORD *) &IDirect3D9_vtable [CREATEDEVICE_VTI] = (DWORD) D3DCreateDevice_hook; 

        if (! VirtualProtect (&IDirect3D9_vtable [CREATEDEVICE_VTI], sizeof (DWORD), protectFlag, &protectFlag)) 
        { 
            return D3DERR_INVALIDCALL; 
        }
    } else { 
        return D3DERR_INVALIDCALL; 
    }
    ///// 
    flag_device = true; 
    CreateThread (NULL, 0, VTablePatchThread, NULL, NULL, NULL); 

    
////    
    return D3D_OK; 
}

HRESULT WINAPI D3DCreateDevice_hook (IDirect3D9* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,  
                    DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters,  
                    IDirect3DDevice9 ** ppReturnedDeviceInterface) 
{ 
 //MessageBoxA (NULL, "DLL Injected D3DCreateDevice_hook", "DLL Injected", MB_ICONEXCLAMATION); 
    HRESULT result = D3DCreateDevice_orig (Direct3D_Object, Adapter, DeviceType, hFocusWindow, BehaviorFlags | D3DCREATE_MULTITHREADED, pPresentationParameters, ppReturnedDeviceInterface); 

    DWORD protectFlag; 
    if (VirtualProtect (&IDirect3D9_vtable [CREATEDEVICE_VTI], sizeof (DWORD), PAGE_READWRITE, &protectFlag)) 
    { 
        *(DWORD *) &IDirect3D9_vtable [CREATEDEVICE_VTI] = (DWORD) D3DCreateDevice_orig; 

        if (! VirtualProtect (&IDirect3D9_vtable [CREATEDEVICE_VTI], sizeof (DWORD), protectFlag, &protectFlag)) 
        { 
            return D3DERR_INVALIDCALL; 
        }
    } else { 
        return D3DERR_INVALIDCALL; 
    }

    if (result == D3D_OK) 
    { 
/ ***/ 
flag_device = false; 
        int A = D3DXCreateFont ( 
      *ppReturnedDeviceInterface, 15, 0, FW_BOLD, 1, 
      false, DEFAULT_CHARSET, 
      OUT_DEFAULT_PRECIS, 
      ANTIALIASED_QUALITY, 
      DEFAULT_PITCH | FF_DONTCARE, 
      _T ("Arial"), 
      &g_Font 
    );
/ ***/ 
        IDirect3D9_vtable = (DWORD *) * (DWORD *)*ppReturnedDeviceInterface; 
        *(PDWORD) &D3DEndScene_orig = (DWORD) IDirect3D9_vtable [ENDSCENE_VTI]; 
*(DWORD *) &IDirect3D9_vtable [ENDSCENE_VTI] = (DWORD) D3DEndScene_hook; 
       /* if (! CreateThread (NULL, 0, VTablePatchThread, NULL, NULL, NULL)) 
        { 
            return D3DERR_INVALIDCALL; 
        }*/ 
flag_end = true; 
    }

    return result; 
}

DWORD WINAPI VTablePatchThread (LPVOID threadParam) 
{ 
    while (true) 
    { 
 //MessageBoxA (NULL, "DLL Injected VTablePatchThread", "DLL Injected", MB_ICONEXCLAMATION); 

        //* (DWORD *) &IDirect3D9_vtable [ENDSCENE_VTI] = (DWORD) D3DEndScene_hook; 
        DWORD protectFlag; 
        if (flag_device) { 
            if (VirtualProtect (&IDirect3D9_vtable [CREATEDEVICE_VTI], sizeof (DWORD), PAGE_READWRITE, &protectFlag)) 
            { 
                *(DWORD *) &IDirect3D9_vtable [CREATEDEVICE_VTI] = (DWORD) D3DCreateDevice_hook; 
                if (! VirtualProtect (&IDirect3D9_vtable [CREATEDEVICE_VTI], sizeof (DWORD), protectFlag, &protectFlag)) 
                { 
                    return D3DERR_INVALIDCALL; 
                }
            } else { 
                return D3DERR_INVALIDCALL; 
            }
        }

        if (flag_end) { 
            if (VirtualProtect (&IDirect3D9_vtable [ENDSCENE_VTI], sizeof (DWORD), PAGE_READWRITE, &protectFlag)) 
            { 
                *(DWORD *) &IDirect3D9_vtable [ENDSCENE_VTI] = (DWORD) D3DEndScene_hook; 
                if (! VirtualProtect (&IDirect3D9_vtable [ENDSCENE_VTI], sizeof (DWORD), protectFlag, &protectFlag)) 
                { 
                    return D3DERR_INVALIDCALL; 
                }                
            } else { 
                return D3DERR_INVALIDCALL; 
            }
        }

        Sleep (1000); 


    }        
}
HRESULT WINAPI D3DEndScene_hook (IDirect3DDevice9* device) 
{ 
//MessageBox (0, _T ("D3DEndScene_hook"), _T ("DLL Injection Successful!"), 0); 
    HRESULT result = NULL; 
static IDirect3DSurface9* pSurface = NULL; 
static IDirect3DSurface9* Buffer = NULL; 
    D3DLOCKED_RECT lockedRect; 

    HRESULT hr; 


    if (gl_update_pbits == false) { 
        if (pSurface == NULL) { 
            D3DSURFACE_DESC desc; 
            device-> GetBackBuffer (0,0, D3DBACKBUFFER_TYPE_MONO, &pSurface); 
            device-> GetRenderTarget (0, &pSurface); 
            pSurface-> GetDesc (&desc); 
            device-> CreateOffscreenPlainSurface (desc. Width, desc. Height, desc. Format, D3DPOOL_SYSTEMMEM, &Buffer, NULL); 
            device-> GetRenderTargetData (pSurface, Buffer); 
            pSurface-> Release (); 

            hr = Buffer-> LockRect (&lockedRect,NULL,D3DLOCK_NO_DIRTY_UPDATE |D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY); 
            //surface saving in the buffer 
            ScreenWidth = desc. Width; 
            ScreenHeight = desc. Height; 
                    
            BITSPERPIXEL = (lockedRect. Pitch/ScreenWidth) *8; 
            oldBufferRealloc = pBits; 
            if ((pBits = realloc (pBits, ScreenHeight*lockedRect. Pitch)) == NULL) {  
                free (oldBufferRealloc);      
                result = D3DEndScene_orig (device); 
                return result; 
            }
            ZeroMemory (pBits, sizeof (pBits)); 
            memcpy ((BYTE *) pBits, (BYTE *) lockedRect.pBits, ScreenWidth * ScreenHeight * BITSPERPIXEL / 8); 

            Buffer-> UnlockRect (); 
        } else { 
            device-> GetBackBuffer (0,0, D3DBACKBUFFER_TYPE_MONO, &pSurface); 
            device-> GetRenderTarget (0, &pSurface); 
            device-> GetRenderTargetData (pSurface, Buffer); 
            pSurface-> Release (); 

            hr = Buffer-> LockRect (&lockedRect,NULL,D3DLOCK_NO_DIRTY_UPDATE |D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY); 
            oldBufferRealloc = pBits; 
            if ((pBits = realloc (pBits, ScreenHeight*lockedRect. Pitch)) == NULL) {  
                free (oldBufferRealloc);      
                result = D3DEndScene_orig (device); 
                return result; 
            }
            ZeroMemory (pBits, sizeof (pBits)); 
            memcpy ((BYTE *) pBits, (BYTE *) lockedRect.pBits, ScreenWidth * ScreenHeight * BITSPERPIXEL / 8); 

            Buffer-> UnlockRect (); 
        }

    }


    RECT TextRect = {50,100,300,300};    

    g_Font-> DrawTextA (NULL, "blablabla",-1, &TextRect,DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA (0x00, 0xff, 0x00, 0xc8)); 
    

    result = D3DEndScene_orig (device); 
    return result; 
}


