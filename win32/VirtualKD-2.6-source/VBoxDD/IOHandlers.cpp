#include "stdafx.h"
#include "../rpcdispatch/rpcdisp.h"
#include "../rpcdispatch/kdcomdisp.h"
#include "../rpcdispatch/reporter.h"
#include "VBoxCmdLine.h"

#include <VBox/vmm/mm.h>

static bool s_bVMWareOpenChannelDetected = false;
static bool s_bChannelDetectSuccessful = false;

static unsigned KDRPCDirectHandler(char *pCommandBody, unsigned CommandBodyLength, char **ppReply);
static unsigned KDRPCProxyHandler(char *pCommandBody, unsigned CommandBodyLength, char **ppReply);

int VirtualKDPortOutHandler( PPDMDEVINS pDevIns, void *pvUser, RTIOPORT Port, uint32_t u32, unsigned cb )
{
	struct  
	{
		unsigned RequestSize;
		unsigned MaxReplySize;
	} RequestHeader = {0, };
	static char CmdBody[262144];
	if (Port == 0x5659)
	{
		int rc = PDMDevHlpPhysRead(pDevIns, (RTGCPHYS)u32, &RequestHeader, sizeof(RequestHeader));
		if (!RT_SUCCESS(rc) || !RequestHeader.RequestSize)
			return VINF_SUCCESS;
		rc = PDMDevHlpPhysRead(pDevIns, (RTGCPHYS)(u32 + sizeof(RequestHeader)), CmdBody, RequestHeader.RequestSize);
		if (!RT_SUCCESS(rc))
			return VINF_SUCCESS;
		ASSERT(!memcmp(CmdBody, g_szRPCCommandHeader, sizeof(g_szRPCCommandHeader) - 1));
		
		char *pReply = NULL;
#ifdef KDVMWARE_USE_PROXY
		unsigned done = KDRPCProxyHandler(CmdBody + sizeof(g_szRPCCommandHeader) - 1, RequestHeader.RequestSize - (sizeof(g_szRPCCommandHeader) - 1), &pReply);
#else
		unsigned done = KDRPCDirectHandler(CmdBody + sizeof(g_szRPCCommandHeader) - 1, RequestHeader.RequestSize - (sizeof(g_szRPCCommandHeader) - 1), &pReply);
#endif

		if (!pReply)
			done = 0;

		char Prefix[sizeof(done) + 2];
		((unsigned *)Prefix)[0] = done + 2;
		Prefix[sizeof(unsigned)] = '1';
		Prefix[sizeof(unsigned) + 1] = ' ';

		rc = PDMDevHlpPhysWrite(pDevIns, (RTGCPHYS)u32, Prefix, sizeof(Prefix));
		if (!RT_SUCCESS(rc))
			return VINF_SUCCESS;
		if (done)
		{
			rc = PDMDevHlpPhysWrite(pDevIns, (RTGCPHYS)(u32 + sizeof(Prefix)), pReply, done);
			if (!RT_SUCCESS(rc))
				return VINF_SUCCESS;
		}
		return VINF_SUCCESS;
	}
	else
	{
		if ((Port == 0x5658) && (u32 == 0x564D5868))
			s_bVMWareOpenChannelDetected = true;
		else
			s_bVMWareOpenChannelDetected = false;
		return VINF_SUCCESS;
	}
}

int VirtualKDPortInHandler( PPDMDEVINS pDevIns, void *pvUser, RTIOPORT Port, uint32_t *pu32, unsigned cb )
{
	if (s_bVMWareOpenChannelDetected)
	{
		*pu32 = 'XOBV';	//Checked in VMWRPC.H
		s_bVMWareOpenChannelDetected = false;
		s_bChannelDetectSuccessful = true;
	}
	else
		*pu32 = -1;
	return VINF_SUCCESS;
}

//-------------------------------------------------------------------------------------------------------------------------------------


#ifdef KDVMWARE_USE_PROXY

int MaxDelay = 0;
static char ReplyBuffer[262144];

static unsigned KDRPCProxyHandler(char *pCommandBody, unsigned CommandBodyLength, char **ppReply)
{
	DWORD done = 0;
	DWORD tick = GetTickCount();
	for (;;)
	{
		BOOL b = CallNamedPipe(_T("\\\\.\\pipe\\kdvmware_proxypipe"), pCommandBody, CommandBodyLength, ReplyBuffer, sizeof(ReplyBuffer), &done, INFINITE);
		if (b)
		{
			int delay = (int)(GetTickCount() - tick);
			if (delay > MaxDelay)
				MaxDelay = delay;
			break;
		}
		int er = GetLastError();
		if ((GetTickCount() - tick) > 1000)
			return 0;
	}
	*ppReply = ReplyBuffer;
	return done;
}

#else

static KdRpcDispatcher *s_pClient = NULL;

static unsigned KDRPCDirectHandler(char *pCommandBody, unsigned CommandBodyLength, char **ppReply)
{
	if (!s_pClient)
		return false;
	return s_pClient->OnRequest(pCommandBody, CommandBodyLength, ppReply);
}

#endif

void InitializeRpcDispatcher()
{
#ifndef KDVMWARE_USE_PROXY
	wchar_t wszPipeName[MAX_PATH] = {0,};
	if (!VBoxCmdLineToPipeNameW(wszPipeName, __countof(wszPipeName)))
		return;
	ASSERT(g_pReporter);
	wcsncpy(g_pReporter->GetStatusPointer()->PipeName, wszPipeName, __countof(g_pReporter->GetStatusPointer()->PipeName));
#else
	wcsncpy(g_pReporter->GetStatusPointer()->PipeName, L"\\\\.\\pipe\\kdvmware_proxypipe", __countof(g_pReporter->GetStatusPointer()->PipeName));
#endif
	s_pClient = new KdRpcDispatcher(new KdComDispatcher(wszPipeName));
	g_pReporter->GetStatusPointer()->PatchErrorPlus1 = 1;
}

void CleanupRpcDispatcher()
{
	delete s_pClient;
	s_pClient = NULL;
}