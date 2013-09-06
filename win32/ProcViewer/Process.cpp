#include "stdafx.h"
#include "Process.h"


/** 
 * 
 * Default constructor.
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
Process::Process() : m_ahmProcessFileIcon( 0 ),
                     m_hProcessModule( 0 )
{
    ZeroMemory( &m_stPrcEntry32, sizeof( m_stPrcEntry32 ));
    m_stPrcEntry32.dwSize = sizeof( m_stPrcEntry32 );
}


/** 
 * 
 * Destructor.
 * 
 * @param       Nil
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
Process::~Process()
{
    Clear();
}

void Process::GetAllInfo()
{
    // Get all information pertaining to a process
    ExtractGDIInfo();
    ExtractAssociatedProcessIcon();
    ExtractProcessType( m_csProcessType );
    ExtractProcessIOCounters();
    ExtractProcessTimes();
    ExtractProcessPriority( m_csPriority );
}


/** 
 * 
 * Loads information pertaining to a process
 * 
 * @param       stpePrcEntry32_i - Process entry
 * @return      bool             - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::SetProcessEntry32( const PROCESSENTRY32& stpePrcEntry32_i )
{
    // Copy information
    m_stPrcEntry32 = stpePrcEntry32_i;

    Clear();

    // Open this process
    m_ahmProcess = ::OpenProcess( PROC_FULL_ACCESS, FALSE, stpePrcEntry32_i.th32ProcessID );
    if( !m_ahmProcess )
    {
        // Make a last ditch attempt to open this process
        m_ahmProcess = ::OpenProcess( PROC_SAFE_ACCESS, TRUE, stpePrcEntry32_i.th32ProcessID );
        if( !m_ahmProcess )
        {
            return false;
        }// End if
    }// End if

    // Get process module
    DWORD dwNeeded = 0;
    ::EnumProcessModules( m_ahmProcess, &m_hProcessModule,  sizeof( m_hProcessModule ), &dwNeeded );

    // Did we find the module
    if( !dwNeeded )
    {
        return false;
    }

    // Get base name for module
    ::GetModuleBaseName( m_ahmProcess, m_hProcessModule, m_csBaseName.GetBufferSetLength( MAX_PATH ), MAX_PATH );
    m_csBaseName.ReleaseBuffer();

    // Get full path
    ::GetModuleFileNameEx( m_ahmProcess, m_hProcessModule, m_csFullPath.GetBufferSetLength( MAX_PATH ), MAX_PATH );
    m_csFullPath.ReleaseBuffer();

    // We are here since all went well
    return true;
}// End set


/** 
 * 
 * Adds a kid to this process
 * 
 * @param       pKid_i - Kid
 * @return      bool   - Returns execution status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::AddKid( Process*& pKid_i )
{
    // Verify kid
    if( !pKid_i )
    {
        return false;
    }

    // Add to tail
    m_lstKids.AddTail( pKid_i );
    return true;
}


/** 
 * 
 * Enumerate all modules
 * 
 * @param       Nil
 * @return      bool - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::EnumModules()
{
    if( !m_ahmProcess )
    {
        return false;
    }

    HANDLE hSnapModules = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, m_stPrcEntry32.th32ProcessID );
    if( hSnapModules == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    // Closes handle of opened snapshot
    Utils::AutoHandleMgr ahmHandleCloser( hSnapModules );

    MODULEENTRY32 stmeModuleEntry32 = { 0 };
    stmeModuleEntry32.dwSize = sizeof( stmeModuleEntry32 );

    // First entry
    if( !Module32First( hSnapModules, &stmeModuleEntry32 ))
    {
        return false;
    }

    // Loop through and add items
    do 
    {
        Module* pModule = new Module;
        if( !pModule )
        {
            continue;
        }

        // Set module entry for module
        pModule->SetModuleEntry32( stmeModuleEntry32 );

        // Add this module to module list
        m_lstModules.AddTail( pModule );
    }while( Module32Next( hSnapModules, &stmeModuleEntry32 )); // Get next module

    // Return success
    return true;
}// EnumModules


/** 
 * 
 * Clears internal module list
 * 
 * @param       Nil
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void Process::Clear()
{
    // Loop through and delete all entries
    POSITION pstPos = m_lstModules.GetHeadPosition();
    while( pstPos )
    {
        Module*& pModule = m_lstModules.GetNext( pstPos );
        delete pModule;
        pModule = 0;
    }

    // Clear list
    m_lstModules.RemoveAll();

    // Destroy process icon
    m_ahmProcessFileIcon && ::DestroyIcon( m_ahmProcessFileIcon );
    m_ahmProcessFileIcon = 0;

    // Close process handle
    m_ahmProcess.Close();

    // Clear process module handle
    m_hProcessModule = 0;

    // Clear structure of it's entries
    ::ZeroMemory( &m_stPrcEntry32, sizeof( m_stPrcEntry32 ));
    m_stPrcEntry32.dwSize = sizeof( m_stPrcEntry32 );

    m_csBaseName.Empty();
    m_csFullPath.Empty();
}// End Clear


/** 
 * 
 * Returns type of process
 * 
 * @param       Nil
 * @return      PROCESS_TYPE_e: Returns type of process
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
PROCESS_TYPE_e Process::ExtractProcessType( CString* pcsProcessType_o) const
{
    return ExtractProcessType( GetFullPath(), pcsProcessType_o );
}


/** 
 * 
 * Static helper that returns type of process.
 * 
 * @param       lpctszProcessPath_i - Process path.
 * @return      PROCESS_TYPE_e      - Return type of process
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
PROCESS_TYPE_e Process::ExtractProcessType( LPCTSTR lpctszProcessPath_i, CString* pcsProcessType_o )
{
    // Check parameter
    if( !lpctszProcessPath_i ||             // Should not be null
        !lpctszProcessPath_i[0] ||          // Should not be empty
        !PathIsExe( lpctszProcessPath_i ))  // Should be an executable file
    {
        return PRT_INVALID;
    }


    // Second call is for retrieving file type
    const DWORD dwRetVal = SHGetFileInfo( lpctszProcessPath_i, 
                                          FILE_ATTRIBUTE_NORMAL, 
                                          0, 
                                          0, 
                                          SHGFI_EXETYPE );
    // Check return value from API 
    if( !dwRetVal )
    {
        // Some error
        return PRT_INVALID;
    }

    
   /************************************************************************
       dwRetVal is interpreted as follows...
       LOWORD = NE or PE and HIWORD = 3.0, 3.5, or 4.0  Windows application 
       LOWORD = MZ and HIWORD = 0  MS-DOS .exe, .com, or .bat file 
       LOWORD = PE and HIWORD = 0  Win32 console application 
    ************************************************************************/

    const WORD wLowWord =  LOWORD( dwRetVal );
    const WORD wHiWord = HIWORD( dwRetVal );
    const WORD wPEWord = MAKEWORD( 'P', 'E' );
    const WORD wMZWord = MAKEWORD( 'M', 'Z' );
    const WORD wNEWord = MAKEWORD( 'N', 'E' );

    // Read above comments to understand what's happening
    if( wLowWord == wPEWord || wLowWord == wNEWord )
    {
        if( wHiWord == 0 )
        {
            if( pcsProcessType_o )
            {
                *pcsProcessType_o = _T( "Win32 Console application" );
            }

            // Console process
            return PRT_WIN32_CONSOLE;
        }
        else
        {
            if( *pcsProcessType_o )
            {
                *pcsProcessType_o = _T( "Win32 Windows application" );
            }

            // Windows process
            return PRT_WIN32_WINDOWS;
        }// End if
    }
    else if( wLowWord == wMZWord && wHiWord == 0 )
    {
        if( *pcsProcessType_o )
        {
            *pcsProcessType_o = _T( "MS-DOS application" );
        }

        // MS-DOS process
        return PRT_MSDOS;
    }// End if

    // We are here because type is invalid
    return PRT_INVALID;
}// End GetProcessType


/** 
 * 
 * Returns associated file icon.
 * 
 * @param       Nil
 * @return      HICON - Returns file icon
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
HICON Process::ExtractAssociatedProcessIcon()
{
    // If there is already an icon return that
    if( m_ahmProcessFileIcon.IsValid() )
    {
        return m_ahmProcessFileIcon;
    }

    // Check path
    if( GetFullPath().IsEmpty() )
    {
        return 0;
    }

    SHFILEINFO shFileInfo = { 0 };

    // For retrieving icon
    VERIFY( SHGetFileInfo( GetFullPath(), 
                           FILE_ATTRIBUTE_NORMAL, 
                           &shFileInfo, 
                           sizeof( shFileInfo ), 
                           SHGFI_SMALLICON | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES ));

    // Do we have an icon, then store this icon handle
    // for destruction later on
    if( shFileInfo.hIcon )
    {
        m_ahmProcessFileIcon = shFileInfo.hIcon;
    }

    // Icon to return
    return m_ahmProcessFileIcon;
}// End GetAssociatedProcessIcon


/** 
 * 
 * Kills process. Process handle won't be closed. This handle can be reused
 * for taking snapshot of killed process.
 * 
 * @param       uExitCode_i - Return code
 * @return      bool        - Return status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::Kill( const UINT uExitCode_i ) const
{
    return TerminateProcess( m_ahmProcess, uExitCode_i ) != FALSE;
}// End Kill


/** 
 * 
 * Retrieve GDI resource usage for process.
 * 
 * @param       Nil
 * @return      bool - Return status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::ExtractGDIInfo()
{
    // Check whether process handle is valid
    if( !m_ahmProcess.IsValid() )
    {
        return false;
    }

    // Get resource usage count
    m_dwGDIObjs = GetGuiResources( m_ahmProcess, GR_GDIOBJECTS );
    m_dwUserObjs = GetGuiResources( m_ahmProcess, GR_USEROBJECTS );

    return GetLastError() == NO_ERROR;
}


/** 
 * 
 * Retrieves IO counters for process
 * 
 * @param       Nil
 * @return      bool - Returns execution status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::ExtractProcessIOCounters()
{
    return m_ProcIoCounters.GetProcessIoCounters( m_ahmProcess );
}// End GetProcessIoCounters


/** 
 * 
 * Retrieves process times for a process.
 * 
 * @param       Nil
 * @return      bool - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::ExtractProcessTimes()
{
    return m_ProcTimes.GetProcessTimes( m_ahmProcess );
}// End GetProcessTimes


/** 
 * 
 * Returns priority of process
 * 
 * @param       csPriority_o - Process priority as string.
 * @return      bool         - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::ExtractProcessPriority( CString& csPriority_o ) const
{
    const DWORD dwPriorityClass = GetPriorityClass( m_ahmProcess );
    switch( dwPriorityClass )
    {
        case NORMAL_PRIORITY_CLASS:
            csPriority_o = _T( "Priority: Normal" );
            break;
        case ABOVE_NORMAL_PRIORITY_CLASS:
            csPriority_o = _T( "Priority: Above normal" );
            break;
        case BELOW_NORMAL_PRIORITY_CLASS:
            csPriority_o = _T( "Priority: Below normal" );
            break;
        case HIGH_PRIORITY_CLASS:
            csPriority_o = _T( "Priority: High" );
            break;
        case REALTIME_PRIORITY_CLASS:
            csPriority_o = _T( "Priority: Realtime" );
            break;
        case IDLE_PRIORITY_CLASS:
            csPriority_o = _T( "Priority: Idle" );
            break;
        default:
            return false;
    }// End switch

    return true;
}// End GetProcessPriority


/** 
 * 
 * Return memory counters for process
 * 
 * @param       Nil
 * @return      PROCESS_MEMORY_COUNTERS - Process memory counter structure
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
const PROCESS_MEMORY_COUNTERS& Process::ExtractProcessMemoryCounters()
{
    GetProcessMemoryInfo( m_ahmProcess, &m_stpmcMemCounters, sizeof( m_stpmcMemCounters ));
    return m_stpmcMemCounters;
}


/** 
 * 
 * Returns file path from a file string. For eg: \\??\\C:\SomeBlah.exe.
 * Since there is a path hidden we can extract it. If there is a valid path then
 * it should be containing a semi colon
 * 
 * @param       csFileName_io - On return will hold parse out file name
 * @return      void
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
void Process::ParseOutFilePath( CString& csFileName_io ) const
{
    // If there is a colon in this path then there should be a path hidden inside it
    const int nIndex = csFileName_io.Find( _T( ':' ));
    if( nIndex == -1 )
    {
        // return
        return;
    }

    // Copy valid path
    csFileName_io = csFileName_io.Mid( nIndex - 1 );
}



/** 
 * 
 * Returns owner of a process i.e. name of user who created this process
 * 
 * @param       hProcess_i - Process handle
 * @param       csOwner_o  - Owner of process
 * @return      bool       - Returns execution status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool Process::ExtractProcessOwner( HANDLE hProcess_i, 
                                   CString& csOwner_o )
{
    // Get process token
    HANDLE hProcessToken = NULL;
    if ( !::OpenProcessToken( hProcess_i, TOKEN_READ, &hProcessToken ) || !hProcessToken )
    {
        return false;
    }

    // Auto closes token on destruction
    Utils::AutoHandleMgr ahmTokenHandleMgr( hProcessToken );
    
    // Obtain the size of the user information in the token.
    DWORD dwProcessTokenInfoAllocSize = 0;
    ::GetTokenInformation(hProcessToken, TokenUser, NULL, 0, &dwProcessTokenInfoAllocSize);
    
    // Call should have failed due to zero-length buffer.
    if( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
    {
        // Allocate buffer for user information in the token.
        PTOKEN_USER pUserToken = RCAST( PTOKEN_USER, new BYTE[dwProcessTokenInfoAllocSize] );
        if (pUserToken != NULL)
        {
            // Delete ptr
            Utils::AutoDeleter<BYTE, true> adTokenBuffDeletor( RCAST( LPBYTE*, &pUserToken ));

            // Retrieve the user information from the token.
            if (::GetTokenInformation( hProcessToken, TokenUser, pUserToken, dwProcessTokenInfoAllocSize, &dwProcessTokenInfoAllocSize ))
            {
                SID_NAME_USE   snuSIDNameUse;
                TCHAR          szUser[MAX_PATH] = { 0 };
                DWORD          dwUserNameLength = MAX_PATH;
                TCHAR          szDomain[MAX_PATH] = { 0 };
                DWORD          dwDomainNameLength = MAX_PATH;

                // Retrieve user name and domain name based on user's SID.
                if ( ::LookupAccountSid( NULL, 
                                         pUserToken->User.Sid, 
                                         szUser, 
                                         &dwUserNameLength, 
                                         szDomain, 
                                         &dwDomainNameLength, 
                                         &snuSIDNameUse ))
                {
                    // User name string
                    csOwner_o = _T("\\\\");
                    csOwner_o += szDomain;
                    csOwner_o += _T("\\");
                    csOwner_o += szUser;

                    // We succeeded
                    return true;
                }//End if
            }// End if
        }// End if
    }// End if

    return false;
}// End GetProcessOwner

bool Process::ExtractProcessCommandLine( HANDLE hProcess, CString& csCommandLine_o )
{
    // sanity checks
    ASSERT(hProcess != NULL);
    if( hProcess == NULL )
    {
        return false;
    }
    
    // 0. get the Process Environment Block address
    int   iReturn = 1;
    DWORD dwSize = 0;
    
    
    PROCESS_BASIC_INFORMATION  pbi = { 0 };
    pbi.PebBaseAddress = (_PEB*)0x7ffdf000;
    
    if (iReturn >= 0) 
    {
        // 1. find the Process Environment Block 
        __PEB PEB = { 0 };
        if (!::ReadProcessMemory(hProcess, pbi.PebBaseAddress, &PEB, sizeof(PEB), &dwSize))
        {
            TRACE_ERR( "ReadProcessMemory failed, process environment block extraction failed" );

            // call GetLastError() to know why
            return true;
        }
        
        // 2. from this PEB, get the address of the block containing a pointer to the CmdLine
        __INFOBLOCK Block = { 0 };
        if (!::ReadProcessMemory(hProcess, (LPVOID)PEB.InfoBlockAddress, &Block, sizeof(Block), &dwSize))
        {
            TRACE_ERR( "ReadProcessMemory failed, block extraction failed" );

            // call GetLastError() to know why
            return true;
        }
        
        // 3. get the CmdLine
        wchar_t wszCmdLine[_MAX_PATH+1] = { 0 };
        if (!::ReadProcessMemory(hProcess, (LPVOID)Block.wszCmdLineAddress, wszCmdLine, _MAX_PATH*sizeof(wchar_t), &dwSize))
        {
            TRACE_ERR( "ReadProcessMemory failed, command line extraction failed" );

            // call GetLastError() to know why
            return true;
        }
        
        // Strip command line arguments from path
        csCommandLine_o = PathGetArgs( wszCmdLine );
        return true;
    }  
    else
    {
        TRACE_ERR( "Impossible to get command line for process" );
        return false;
    }// End if
    
    return true;
}// End GetProcessCommandLine

bool Process::ChangeProcessPriority( const HANDLE& hProcess_i, const UINT uPriority_i )
{
    return SetPriorityClass( hProcess_i, uPriority_i ) != 0;
}