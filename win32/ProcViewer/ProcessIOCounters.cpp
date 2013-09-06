#include "stdafx.h"
#include "ProcessIOCounters.h"

ProcessIOCounters::ProcessIOCounters()
{}

ProcessIOCounters::~ProcessIOCounters()
{}


bool ProcessIOCounters::GetProcessIoCounters( const Utils::AutoHandleMgr& ahmProcess_i )
{
    // Check process validity status
    if( !ahmProcess_i.IsValid() )
    {
        return false;
    }

    IO_COUNTERS sticCounter = { 0 };
    if( !::GetProcessIoCounters( ahmProcess_i, &sticCounter ))
    {
        return false;
    }

    // Get information
    GetReadOperationCount().Format( _T( "Read operation: %I64d" ), sticCounter.ReadOperationCount );
    GetWriteOperationCount().Format( _T( "Write operation: %I64d" ), sticCounter.WriteOperationCount );
    GetOtherOperationCount().Format( _T( "Other operation: %I64d" ), sticCounter.OtherOperationCount );
    GetReadTransferCount().Format( _T( "Read transfer: %I64d" ), sticCounter.ReadTransferCount );
    GetWriteTransferCount().Format( _T( "Write transfer: %I64d" ), sticCounter.WriteTransferCount );
    GetOtherTransferCount().Format( _T( "Other transfer: %I64d" ), sticCounter.OtherTransferCount );

    return true;

}