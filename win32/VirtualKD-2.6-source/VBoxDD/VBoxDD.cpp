// VBoxDD.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "IOHandlers.h"

#define VIRTUALKD_HOOKED_VBOX_DEVICE_NAME	"pci"

/*  The ich9pci hooking code fixes the ICH9 compatibility issue. Solution proposed by RonD.
	For more details see http://forum.sysprogs.org/viewtopic.php?f=4&t=521&start=20#p2170
*/
#define VIRTUALKD_HOOKED_VBOX_DEVICE_NAME_ALT	"ich9pci"

namespace
{
	PCPDMDEVREG s_pOriginalDevReg = NULL, s_pOriginalAltDevReg = NULL;
	bool s_bIORegistered = false;

	template <PCPDMDEVREG &pOriginalReg> int RTCALL ConstructOverride(PPDMDEVINS pDevIns, int iInstance, PCFGMNODE pCfgHandle)
	{
		if (!pOriginalReg)
			return VINF_OBJECT_DESTROYED;
		pDevIns->pReg = pOriginalReg;
		int rc = pOriginalReg->pfnConstruct(pDevIns, iInstance, pCfgHandle);
		if (!RT_SUCCESS(rc))
			return rc;
		if (!s_bIORegistered)
		{
			rc = PDMDevHlpIOPortRegister(pDevIns, 0x5658, 2, NULL,  VirtualKDPortOutHandler, VirtualKDPortInHandler, NULL, NULL, "VirtualKD interface");
			if (!RT_SUCCESS(rc))
				return rc;
			s_bIORegistered = true;
		}
		return VINF_SUCCESS;
	}
}


struct DevRegisterCallbacksHook : public PDMDEVREGCB
{
	PPDMDEVREGCB pOriginalCallbacks;

	DevRegisterCallbacksHook(const PDMDEVREGCB &Callbacks) : PDMDEVREGCB(Callbacks)
	{
		pfnRegister = &RegisterOverride;
//		pfnMMHeapAlloc = &AllocOverride;
	}

	static int RTCALL RegisterOverride(PPDMDEVREGCB pCallbacks, PCPDMDEVREG pDevReg)
	{
		if (pDevReg)
		{
			if (!strcmp(VIRTUALKD_HOOKED_VBOX_DEVICE_NAME, pDevReg->szName))
			{
				PPDMDEVREG pReg = new PDMDEVREG();
				*pReg = *pDevReg;
				s_pOriginalDevReg = pDevReg;
				pReg->pfnConstruct = &ConstructOverride<s_pOriginalDevReg>;
				pDevReg = pReg;
			}			
			else if (!strcmp(VIRTUALKD_HOOKED_VBOX_DEVICE_NAME_ALT, pDevReg->szName))
			{
				PPDMDEVREG pReg = new PDMDEVREG();
				*pReg = *pDevReg;
				s_pOriginalAltDevReg = pDevReg;
				pReg->pfnConstruct = &ConstructOverride<s_pOriginalAltDevReg>;
				pDevReg = pReg;
			}			
		}
		return ((DevRegisterCallbacksHook *)pCallbacks)->pOriginalCallbacks->pfnRegister(((DevRegisterCallbacksHook *)pCallbacks)->pOriginalCallbacks, pDevReg);
	}

// 	static void * RTCALL AllocOverride(PPDMDEVREGCB pCallbacks, size_t cb)
// 	{
// 		return ((DevRegisterCallbacksHook *)pCallbacks)->pOriginalCallbacks->pfnMMHeapAlloc(((DevRegisterCallbacksHook *)pCallbacks)->pOriginalCallbacks, cb);
// 	}
};

extern "C"
{
	typedef int (*PVBoxDevicesRegisterOriginal)(PPDMDEVREGCB pCallbacks, uint32_t u32Version);
	static int VBoxDevicesRegisterOriginal(PPDMDEVREGCB pCallbacks, uint32_t u32Version)
	{
		PVBoxDevicesRegisterOriginal p = (PVBoxDevicesRegisterOriginal)GetProcAddress(GetModuleHandle(_T("VBoxDD0.dll")), "VBoxDevicesRegister");
		if (!p)
			return VERR_UNRESOLVED_ERROR;
		return p(pCallbacks, u32Version);
	}

	int _VBoxDevicesRegister(PPDMDEVREGCB pCallbacks, uint32_t u32Version)
	{
		if (!pCallbacks)
			return VBoxDevicesRegisterOriginal(pCallbacks, u32Version);
		
		DevRegisterCallbacksHook Callbacks(*pCallbacks);
		Callbacks.pOriginalCallbacks = pCallbacks;
		return VBoxDevicesRegisterOriginal(&Callbacks, u32Version);
	}
}