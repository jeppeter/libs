#pragma once
#include "ServiceCommandProtocol.h"
#include "pipesrv.h"
#include <bzscmn/thread.h>

//This function is defined in both KDCLIENT.DLL and our VBOXDD.DLL.
HRESULT StartRevertingCurrentVMToLastSnapshot();

class ServiceCommandHandler
{
private:
	PipeServer<true, true, false> m_PipeServer;
	BazisLib::Win32::MemberThread m_Thread;
	BazisLib::Win32::Event m_ThreadDoneEvent;

	long m_bInstantBreakRequested;

public:
	unsigned ThreadBody()
	{
		while (!m_PipeServer.IsShutDown())
		{
			ServiceProtocolPacket packet;
			if (m_PipeServer.Receive(&packet, sizeof(packet), false) != sizeof(packet))
				continue;

			ServiceProtocolReply reply = {0,};
			switch (packet.Command)
			{
			case kInstantBreak:
				m_bInstantBreakRequested = true;
				reply.Status = S_OK;
				break;
			case kRevertToLastSnapshot:
				reply.Status = StartRevertingCurrentVMToLastSnapshot();
				break;
			}

			m_PipeServer.Send(&reply, sizeof(reply));
		}
		m_ThreadDoneEvent.Set();
		//Returning/calling ExitThread() when DLL_PROCESS_DETACH is active would cause deadlock in WaitForSingleObject()
		m_Thread.Terminate();
		return 0;
	}

	/*!
		\param lpPipeName Specifies the base VirtualKD pipe name. The service pipe name will be formed by appending the suffix.
	*/
	ServiceCommandHandler(LPCTSTR lpPipeName)
		: m_PipeServer((BazisLib::String(lpPipeName) + VIRTUALKD_SERVICE_PROTOCOL_PIPE_SUFFIX).c_str())
		, m_Thread(this, &ServiceCommandHandler::ThreadBody)
		, m_bInstantBreakRequested(0)
	{
		m_Thread.Start();
	}

	~ServiceCommandHandler()
	{
		m_PipeServer.Shutdown();
		//If this destructor is called from DLL_PROCESS_DETACH, waiting for the thread won't work. 

		HANDLE waitObjects[2] = {m_ThreadDoneEvent.GetHandle(), m_Thread.GetHandle()};
		WaitForMultipleObjects(_countof(waitObjects), waitObjects, FALSE, INFINITE);

		m_Thread.Terminate();
	}

	bool CheckAndClearInstantBreakRequestedFlag()
	{
		return InterlockedExchange(&m_bInstantBreakRequested, 0) != 0;
	}

};