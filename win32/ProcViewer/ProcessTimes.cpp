#include "stdafx.h"
#include "ProcessTimes.h"

ProcessTimes::ProcessTimes()
{}

ProcessTimes::~ProcessTimes()
{}

bool ProcessTimes::GetProcessTimes( const Utils::AutoHandleMgr& ahmProcess_i )
{
    // Get file times for process
    FILETIME ftStartTime = { 0 },
             ftExitTime = { 0 },
             ftKernelModeTime = { 0 },
             ftUserModeTime = { 0 };

    // Get times of process
    const bool bResult = ::GetProcessTimes( ahmProcess_i, 
                                            &ftStartTime, 
                                            &ftExitTime, 
                                            &ftKernelModeTime, 
                                            &ftUserModeTime ) != FALSE;
    // Get formatted
    Utils::GetFormattedTime( GetProcessStartTime(), ftStartTime, true );
    Utils::GetFormattedTime( GetProcessExitTime(), ftExitTime, true );
    Utils::GetFormattedTime( GetProcessKernelTime(), ftKernelModeTime, false );
    Utils::GetFormattedTime( GetProcessUserTime(), ftUserModeTime, false );

    return bResult;
}// End GetProcessTimes