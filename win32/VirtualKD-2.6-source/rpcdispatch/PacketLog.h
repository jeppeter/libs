/*! \file PacketLog.h
    \author Ivan Shcherbakov (Bazis)
    $Id: PacketLog.h,v 1.2 2009-06-07 10:51:22 Administrator Exp $
    \brief Contains declarations for class that writes debug packet logs
*/

#pragma once
#include <bzscmn/file.h>
#include <bzscmn/datetime.h>
#include "../kdvm/kdxxx.h"

//! Manages HTML log files for sent and receiving packets
class PacketLogger
{
private:
	BazisLib::ManagedPointer<BazisLib::AIFile> m_pFile;
	BazisLib::FilePath m_LogFilePath;

	unsigned m_SendPacketNumber;
	unsigned m_RecvPacketNumber;
	BazisLib::DateTime m_StartTime;
	
	std::wstring m_SessionName;
	bool m_bTableHeaderPrinted;

public:
	PacketLogger(const TCHAR *pszKdPipeName);
	~PacketLogger();
	
	void OnSendReceivePacket(bool bLoggingEnabled,
							 bool bSendPacket,
							 ULONG PacketType,
							 PKD_BUFFER FirstBuffer,
							 PKD_BUFFER SecondBuffer,
							 PKD_CONTEXT KdContext);

	void OnWindowsTerminationSimulated();	
	void OnWindowsTerminationSimDone(char *pDebugMsg);
};