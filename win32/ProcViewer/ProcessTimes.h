#ifndef _PROCESS_TIMES_H_
#define _PROCESS_TIMES_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ProcessTimes  
{
    public:

	    ProcessTimes();
	    virtual ~ProcessTimes();

        const CString& GetProcessStartTime() const { return m_csProcessStartTime; }
        CString& GetProcessStartTime() { return m_csProcessStartTime; }

        const CString& GetProcessExitTime() const { return m_csProcessExitTime; }
        CString& GetProcessExitTime() { return m_csProcessExitTime; }

        const CString& GetProcessKernelTime() const { return m_csProcessKernelTime; }
        CString& GetProcessKernelTime() { return m_csProcessKernelTime; }

        const CString& GetProcessUserTime() const { return m_csProcessUserTime; }
        CString& GetProcessUserTime() { return m_csProcessUserTime; }

        bool GetProcessTimes( const Utils::AutoHandleMgr& ahmProcess_i );

    private:

        // Process times
        CString m_csProcessStartTime;
        CString m_csProcessExitTime;
        CString m_csProcessKernelTime;
        CString m_csProcessUserTime;
};// End ProcessTimes

#endif // _PROCESS_TIMES_H_
