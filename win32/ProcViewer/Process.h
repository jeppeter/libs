#ifndef _PROCESS_H_
#define _PROCESS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Module.h"
#include <afxtempl.h>
#include <TLHELP32.H>
#include "FileVersionInfo.h"
#include <psapi.h>
#include "ProcessIOCounters.h"
#include "ProcessTimes.h"

// Access modes for a process
#define PROC_FULL_ACCESS ( PROCESS_ALL_ACCESS | SYNCHRONIZE )
#define PROC_SAFE_ACCESS ( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE | PROCESS_VM_WRITE )

enum PROCESS_TYPE_e
{
    PRT_WIN32_CONSOLE = 10,
    PRT_WIN32_WINDOWS,
    PRT_MSDOS,
    PRT_INVALID
};

#ifdef __cplusplus
extern "C" 
{
#endif

    typedef LONG NTSTATUS;
    typedef struct _PEB *PPEB;
    typedef ULONG KAFFINITY;
    typedef KAFFINITY *PKAFFINITY;
    typedef LONG KPRIORITY;


    typedef struct _PROCESS_BASIC_INFORMATION 
    {
       NTSTATUS    ExitStatus;
       PPEB        PebBaseAddress;
       KAFFINITY   AffinityMask;
       KPRIORITY   BasePriority;
       ULONG       UniqueProcessId;
       ULONG       InheritedFromUniqueProcessId;
    } PROCESS_BASIC_INFORMATION;
    typedef PROCESS_BASIC_INFORMATION *PPROCESS_BASIC_INFORMATION;

    typedef struct
    {
       DWORD Filler[4];
       DWORD InfoBlockAddress;
    } __PEB;

    typedef struct 
    {
       DWORD Filler[17];
       DWORD wszCmdLineAddress;
    } __INFOBLOCK;

#ifdef __cplusplus
}
#endif



/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * Process - Denotes a single process running in a system
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-06-15
 */
class Process  
{
    public:

        Process();
        virtual ~Process();

        // Set's process handle
        void SetProcessHandle( HANDLE hProcess_i )
        {
            m_ahmProcess = hProcess_i;
        }

        // Returns handle to process
        HANDLE GetProcessHandle() const
        {
            return m_ahmProcess;
        }

        // Returns process id of process
        DWORD GetProcessID() const
        {
            return m_stPrcEntry32.th32ProcessID;
        }

        // Returns a reference to process entry
        const PROCESSENTRY32& GetProcessEntry32() const
        {
            return m_stPrcEntry32;
        }

        // Extracts and stores all information pertaining to a process
        void GetAllInfo();

        // Sets process information
        bool SetProcessEntry32( const PROCESSENTRY32& stpePrcEntry32_i );

        // Adds a kid to this process
        bool AddKid( Process*& pPrcKid_i );

        // Returns true if this process is a kid of some other process
        bool IsKid() const
        {
            return m_stPrcEntry32.th32ParentProcessID != 0;
        }

        // Returns base name of the process
        const CString& GetBaseName() const
        {
            return m_csBaseName;
        }

        const CString& GetFullPath() const
        {
            return m_csFullPath;
        }

        // Returns true if process still alive
        bool IsProcessAlive() const
        {
            if( m_ahmProcess.IsValid() )
            {
                // If a timeout occurs then process is alive
                return WaitForSingleObject( m_ahmProcess, 0 ) == WAIT_TIMEOUT;
            }

            return false;
        }

        // Load modules for this process
        bool EnumModules();

        // Clears
        void Clear();

        // Return type of process
        PROCESS_TYPE_e ExtractProcessType( CString* pcsProcessType_o = 0 ) const;
        static PROCESS_TYPE_e ExtractProcessType( LPCTSTR lpctszProcessPath_i, CString* pcsProcessType_o = 0 );

        // Returns associated process icon
        HICON ExtractAssociatedProcessIcon();

        // Kill process
        bool Kill( const UINT uExitCode_i ) const;

        // Retrieve GID information for process
        bool ExtractGDIInfo();

        // Retrieve IO counters
        bool ExtractProcessIOCounters();

        // Get times for process
        bool ExtractProcessTimes();

        // Get priority of process
        bool ExtractProcessPriority( CString& csPriority_o ) const;

        // User who owns the process
        static bool ExtractProcessOwner( HANDLE hProcess_i, CString& csOwner_o );

        // Retrieves process command line
        static bool ExtractProcessCommandLine( HANDLE hProcess, CString& csCommandLine_o );

        // Get memory counters
        const PROCESS_MEMORY_COUNTERS& ExtractProcessMemoryCounters();

        // Changes process priority
        static bool ChangeProcessPriority( const HANDLE& hProcess_i, const UINT uPriority_i );
        static void EnableDisableAutoStartup( LPCTSTR lpctszProcessPath_i );

    protected:

        // Parse of hidden file name
        void ParseOutFilePath( CString& csFileName_io ) const;

    private:

        // Corresponding file icon for process
        Utils::AutoHICONMgr m_ahmProcessFileIcon;

        // Handle to process
        Utils::AutoHandleMgr m_ahmProcess;

        // Handle to process module
        HMODULE m_hProcessModule;

        // Process entry from tool help
        PROCESSENTRY32 m_stPrcEntry32;

        // Version information
        FileVersionInfo m_fviFileVersion;

        // For process io counter extraction
        ProcessIOCounters m_ProcIoCounters;

        // Process times
        ProcessTimes m_ProcTimes;

        // List of kids
        CList<Process*, Process*&> m_lstKids;
        CList<Module*, Module*&> m_lstModules;

        // Base name of process
        CString m_csBaseName;

        // Full path of process
        CString m_csFullPath;

        // GDI object count
        DWORD m_dwGDIObjs;
        DWORD m_dwUserObjs;

        // Console, windows or ms-dos
        CString m_csProcessType;
    
        // Process priority
        CString m_csPriority;

        // User who created the process
        CString m_csProcessOwner;

        // Process command line
        CString m_csProcessCommandLine;

        // Process id
        DWORD m_dwProcId;

        // Memory counters
        PROCESS_MEMORY_COUNTERS m_stpmcMemCounters;

};// End class Process

#endif // _PROCESS_H_