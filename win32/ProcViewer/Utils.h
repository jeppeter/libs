#ifndef _UTILS_H_
#define _UTILS_H_

#include <atlbase.h>
#include <afxtempl.h>
#include "OSVer.h"
#include "resource.h"

#ifndef CALL_DO_EVENTS
#   define CALL_DO_EVENTS( loopIndex )\
        if((( loopIndex ) % LOOP_THRESH_HOLD_FOR_DO_EVENTS ) == 0 )\
        {\
            Utils::DoEvents();\
        }
#endif //CALL_DO_EVENTS


//A custom macro which helps in tracing.
#if ! defined UNKNOWN_EXCEPTION
#   ifdef _DEBUG
#       define UNKNOWN_EXCEPTION AfxTrace(_T("\n%s(%i): Unknown Exception occurred!!"), _T(__FILE__),__LINE__)
#   else
#       define UNKNOWN_EXCEPTION 0
#   endif
#endif

#ifdef _countof
#undef _countof
#endif

#define _countof(array) (sizeof(array)/sizeof(array[0]))

#ifdef RCAST
    #undef RCAST
#endif
#define RCAST( type, dt ) ( reinterpret_cast<type>(( dt )))

#ifdef DCAST
    #undef DCAST
#endif
#define DCAST( type, dt ) ( dynamic_cast<type>(( dt )))

#ifdef SCAST
    #undef SCAST
#endif
#define SCAST( type, dt ) ( static_cast<type>(( dt )))

#ifdef CCAST
	#undef CCAST
#endif
#define CCAST( type, dt ) ( const_cast<type>(( dt )))

#ifdef _DEBUG
    #define DBREAK( condition ) if( condition ) _asm int 3
#else
    #define DBREAK( condition )
#endif

#ifndef LOOP_THRESH_HOLD_FOR_DO_EVENTS
#   define LOOP_THRESH_HOLD_FOR_DO_EVENTS 10
#endif

namespace Utils
{

#pragma warning( push )
#pragma warning( disable:4127 )
#pragma auto_inline( off )

    class CoAutoInitializer
    {
    public:
        CoAutoInitializer()
        {
            CoInitialize( 0 );
        }

        ~CoAutoInitializer()
        {
            CoUninitialize();
        }
    };

    template< class TYPE, const bool bArray_i > struct AutoDeleter
    {
        TYPE **m_pType;

        AutoDeleter( TYPE** pType_i = 0 );
        ~AutoDeleter();

        void Delete();
        void Attach( TYPE** pType_i )
        {
            m_pType = pType_i;
        }
        TYPE* Detach();
    };

    template< class TYPE, const bool bArray_i >
        AutoDeleter< TYPE, bArray_i >::AutoDeleter( TYPE** pType_i )
    {
        Attach( pType_i );
    }

    template< class TYPE, const bool bArray_i >
        AutoDeleter< TYPE, bArray_i >::~AutoDeleter()
    {
        Delete();
    }

    template< class TYPE, const bool bArray_i >
        void AutoDeleter< TYPE, bArray_i >::Delete()
    {
        try
        {
            // Check pointer to delete
            if( !m_pType || !*m_pType )
            {
                return;
            }

            if( bArray_i )
            {
                delete [] *m_pType;
            }
            else
            {
                delete *m_pType;
            }

            // Clear pointer
            *m_pType = 0;
        }
        catch( ... )
        {
            // Ignore any exception
            UNKNOWN_EXCEPTION;
        }
    }

    template< class TYPE, const bool bArray_i >
        TYPE* AutoDeleter< TYPE, bArray_i >::Detach()
    {
        // Store for return
        TYPE** pType = m_pType;

        // Detach
        m_pType = 0;

        // return detached pointer
        return ( pType ? *pType : 0 );
    }

    template< class TYPE, const bool bArray_i >
    TYPE* Allocate( const DWORD dwSize_i = 0 )
    {
        try
        {
            if( bArray_i )
            {
                return new TYPE[ dwSize_i ];
            }
            else
            {
                return new TYPE;
            }
        }
        catch( ... )
        {
            return 0;
        }
    }

    struct AutoModuleHandler
    {
        HMODULE* m_hModuleToManage;
        AutoModuleHandler( HMODULE* hModule_i ) : m_hModuleToManage( hModule_i )
        {}

        ~AutoModuleHandler()
        {
            ReleaseModule();
        }

        void ReleaseModule()
        {
            if( m_hModuleToManage )
            {
                FreeLibrary( *m_hModuleToManage );
                *m_hModuleToManage = 0;
            }// End if
        }// End FreeModule

        HMODULE* Detach()
        {
            HMODULE* hModuleToManage = m_hModuleToManage;
            *m_hModuleToManage = 0; // Detach

            // Return detached module
            return hModuleToManage;
        }

        void Attach( HMODULE* hModule_i )
        {
            m_hModuleToManage = hModule_i;
        }
    };

    struct AutoHandleMgr
    {
        HANDLE m_hHandle;
        explicit AutoHandleMgr(): m_hHandle( 0 )
        {}

        explicit AutoHandleMgr( HANDLE hHandle_i ) : m_hHandle( hHandle_i )
        {}

        ~AutoHandleMgr()
        {
            Close();
        }

        operator HANDLE() const
        {
            return m_hHandle;
        }

        operator HANDLE&()
        {
            return m_hHandle;
        }

        void Attach( HANDLE& hHandleToAttach_io )
        {
            Close();
            m_hHandle = hHandleToAttach_io;
            hHandleToAttach_io = 0;
        }

        HANDLE Detach()
        {
            HANDLE hForReturn = m_hHandle;
            m_hHandle = 0;
            return hForReturn;
        }

        AutoHandleMgr& operator = ( HANDLE hHandle_i )
        {
            if( m_hHandle != hHandle_i )
            {
                // Close previous
                Close();
                m_hHandle = hHandle_i;
            }
            return *this;
        }

        void Close()
        {
            // Close handle if and only if handle is valid
            IsValid() && CloseHandle( m_hHandle );
            m_hHandle = INVALID_HANDLE_VALUE;
        }

        bool operator !() const
        {
            return !m_hHandle;
        }

        bool IsValid() const
        {
            return m_hHandle && m_hHandle != INVALID_HANDLE_VALUE;
        }
    };

    struct AutoHICONMgr
    {
        HICON m_hIcon;
        AutoHICONMgr( HICON hIcon_i = 0 ) : m_hIcon( hIcon_i )
        {}

        ~AutoHICONMgr()
        {
            Close();
        }

        operator HICON() const
        {
            return m_hIcon;
        }

        operator HICON&()
        {
            return m_hIcon;
        }

        HICON operator &()
        {
            return m_hIcon;
        }

        AutoHICONMgr& operator = ( const HICON& hIcon_i )
        {
            // Close previous
            Close();
            m_hIcon = hIcon_i;
			return *this;
        }

        void Close()
        {
            // Close handle if and only if handle is valid
            IsValid() && ::DestroyIcon( m_hIcon );
            m_hIcon = 0;
        }

        bool operator !()
        {
            return !m_hIcon;
        }

        bool IsValid()
        {
            return m_hIcon != 0;
        }
    };

    struct AutoDCStateHandler
    {
        CDC* m_pDC;
        int nDCRollbackState;

        AutoDCStateHandler( CDC* dcTarget_i ) : m_pDC( 0 ), nDCRollbackState( 0 )
        {
            Attach( dcTarget_i );
        }

        ~AutoDCStateHandler()
        {
            RestoreDCState();
        }

        void Attach( CDC* dcTarget_i )
        {
            m_pDC = dcTarget_i;
            SaveDC();
        }

        CDC* Detach()
        {
            // Store temporarily
            CDC* pTempDC = m_pDC;
            m_pDC = 0; //Detach

            return pTempDC;
        }

        int SaveDC()
        {
            if( IsDCValid() )
            {
                nDCRollbackState = m_pDC->SaveDC();
            }
            else
            {
                nDCRollbackState = 0;
            }

            return nDCRollbackState;
        }

        BOOL RestoreDCState()
        {
            if( nDCRollbackState && IsDCValid() )
            {
                BOOL bResResult = m_pDC->RestoreDC( nDCRollbackState );
                nDCRollbackState = 0;
                return bResResult;
            }// End if

            return FALSE;
        }// End RestoreDC

        bool IsDCValid()
        {
            return ( m_pDC && m_pDC->GetSafeHdc() );
        }
    };

    struct AutoSetRedrawManager
    {
        AutoSetRedrawManager()
        {}

        ~AutoSetRedrawManager()
        {
            EnableRedraw();
        }

        bool AddWindow( CWnd& wndWindow_i )
        {
            // Call handle version
            return AddWindow( wndWindow_i.GetSafeHwnd() );
        }

        bool AddWindow( HWND hWnd_i )
        {
            if( !hWnd_i || !IsWindow( hWnd_i ))
            {
                ASSERT( FALSE );
                return false;
            }

            // Add to list
            m_WindowList.AddTail( hWnd_i );

            // Disable redraw
            ::SendMessage( hWnd_i, WM_SETREDRAW, FALSE, 0 );

            return true;
        }

        bool EnableRedraw()
        {
            POSITION pstPos = m_WindowList.GetHeadPosition();
            while( pstPos )
            {
               HWND hWnd = m_WindowList.GetNext( pstPos );

               if( hWnd && ::IsWindow( hWnd ))
               {
                // Disable redraw
                ::SendMessage( hWnd, WM_SETREDRAW, TRUE, 0 );
               }
            }// End while

            // Remove all windows from list
            m_WindowList.RemoveAll();

            return true;
        }// End EnableRedraw

    private:
        CList<HWND, HWND&> m_WindowList;

    };

    /*******************************************************************
    Purpose: 

    Compares two strings.
    Arguments: 

    lpctszString1_i : First string.
    lpctszString2_i : Second string.

    Return: 

    Returns true is both strings are equal else returns false
    ********************************************************************/
    bool static CompareStr( LPCTSTR lpctszString1_i, LPCTSTR lpctszString2_i )
    {
        // Compare both strings
        const int RESULT =  CompareString( LOCALE_SYSTEM_DEFAULT,
                                           NORM_IGNORECASE,
                                           lpctszString1_i,
                                           -1,
                                           lpctszString2_i,
                                           -1 );

        // Check result
        return RESULT == CSTR_EQUAL;
    }

    // Custom message box
    static int CustomMsgBox( LPCTSTR lpctszMessage_i, 
                             LPCTSTR lpctszTitle_i, 
                             UINT uFlags,
                             HWND hWndParent_i = 0,
                             LPCTSTR lpctszIconResource_i = 0 )
    {
        // Fill out message box parameters
        MSGBOXPARAMS msgParams = { 0 };
        msgParams.hwndOwner = hWndParent_i;
        msgParams.cbSize = sizeof( msgParams );
        msgParams.dwStyle = uFlags | ( lpctszIconResource_i ? MB_USERICON : 0 );
        msgParams.hInstance = ( lpctszIconResource_i ? AfxGetInstanceHandle() : 0 );
        msgParams.lpszCaption = lpctszTitle_i;
        msgParams.lpszText = lpctszMessage_i;
        msgParams.lpszIcon = lpctszIconResource_i;
        
        // Show message box
        return MessageBoxIndirect( &msgParams );
    }

    // Message buffer size
    #define MSG_BUFFER_SIZE 512

    // Shows error dialog
    static int __cdecl ShowError( LPCTSTR lpctszErrorMessage_i, ... )
    {
        va_list vaArgList;
        va_start( vaArgList, lpctszErrorMessage_i );

        TCHAR szMsgBuffer[MSG_BUFFER_SIZE] = { 0 };

        _vsntprintf_s( szMsgBuffer, _countof( szMsgBuffer ), lpctszErrorMessage_i, vaArgList );
        return CustomMsgBox( szMsgBuffer, 
                             _T( "Error!" ), 
                             MB_OK | MB_ICONERROR, 
                             GetForegroundWindow() );
    }// End ShowError

    // Shows warning dialog
    static int __cdecl ShowWarning( LPCTSTR lpctszWarningMessage_i, ... )
    {
       va_list vaArgList;
       va_start( vaArgList, lpctszWarningMessage_i );
       
       TCHAR szMsgBuffer[MSG_BUFFER_SIZE] = { 0 };
       
       _vsntprintf_s( szMsgBuffer, _countof( szMsgBuffer ), lpctszWarningMessage_i, vaArgList );
       
       return CustomMsgBox( szMsgBuffer, 
                            _T( "Warning!" ),
                            MB_OK | MB_ICONWARNING,
                            GetForegroundWindow() );
    }// End ShowWarning
    

    // Shows information dialog
    static int __cdecl ShowInfo( LPCTSTR lpctszInfoMessage_i, ... )
    {
        va_list vaArgList;
        va_start( vaArgList, lpctszInfoMessage_i );

        TCHAR szMsgBuffer[MSG_BUFFER_SIZE] = { 0 };

		_vsntprintf_s( szMsgBuffer, _countof( szMsgBuffer ), lpctszInfoMessage_i, vaArgList );

        return CustomMsgBox( szMsgBuffer, 
                             _T( "Info" ),
                             MB_OK | MB_ICONINFORMATION,
                             GetForegroundWindow() );
    }// End ShowInfo

    // Shows information dialog
    static int _cdecl ShowQuestion( LPCTSTR lpctszQuestionMsg_i, ... )
    {
        va_list vaArgList;
        va_start( vaArgList, lpctszQuestionMsg_i );

        TCHAR szMsgBuffer[MSG_BUFFER_SIZE] = { 0 };

        _vsntprintf_s( szMsgBuffer, _countof( szMsgBuffer ), lpctszQuestionMsg_i, vaArgList );
        return CustomMsgBox( szMsgBuffer, 
                             _T( "Confirm" ), 
                             MB_YESNO | MB_ICONQUESTION,
                             GetForegroundWindow() );
    }// End if

    // Get last error message
    static void GetLastErrorMsg( CString* pcsErrorMessage_o = 0 )
    {
		const DWORD dwLastErr = ::GetLastError();
		// If no error then return
		if( dwLastErr == ERROR_SUCCESS )
		{
			return;
		}
		
        LPTSTR lpszMsgBuf;
        if ( !FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                             FORMAT_MESSAGE_FROM_SYSTEM | 
                             FORMAT_MESSAGE_IGNORE_INSERTS,
                             NULL,
                             dwLastErr,
                             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                             RCAST( LPTSTR, &lpszMsgBuf ),
                             0,
                             NULL ))
        {
            // Handle the error.
            return;
        }

        if( !pcsErrorMessage_o )
        {
            ShowError( _T( "The following error occurred:\n%s" ), lpszMsgBuf );
        }
        else
        {
            *pcsErrorMessage_o = lpszMsgBuf;
        }

        // Free the buffer.
        LocalFree( lpszMsgBuf );

    }// End GetLastErrorMsg

    static void DoEvents( HWND hWnd_i )
    {
        // Message structure
        MSG stMsg = { 0 };

        // Peek message flags
        const DWORD dwPMFlags = PM_REMOVE; /*| ( OSVer::Instance().IsXP() ? PM_QS_PAINT | PM_QS_INPUT : 0 );*/

        // Flush out any keyboard, mouse, or painting message, without yielding control
        while( PeekMessage( &stMsg, 
                            hWnd_i, 
                            0, 
                            0, 
                            dwPMFlags ))
        {
            TranslateMessage( &stMsg );
            DispatchMessage( &stMsg );
        }// End while
    }// End DoEvents

    static bool DrawGradient( CDC *pDC_i, 
                              const CRect &crDrawRect_i, 
                              const COLORREF clrStartColor_i, 
                              const COLORREF clrEndColor_i, 
                              const bool bIsHorizontal_i )
    {
        // Validate given dc
        if( !pDC_i || !pDC_i->GetSafeHdc() )
        {
            return false;
        }

        // Vertex object, number of edges
        TRIVERTEX stVertex[2] = { 0 };

        // Extract RGB from starting color
        const BYTE byRed_Start    = GetRValue( clrStartColor_i );    // Red
        const BYTE byGreen_Start  = GetGValue( clrStartColor_i );    // Green
        const BYTE byBlue_Start   = GetBValue( clrStartColor_i );    // Blue

        // Set first stVertex
        stVertex[0] .x      = crDrawRect_i.left;
        stVertex[0] .y      = crDrawRect_i.top;
        stVertex[0] .Red    = SCAST( USHORT, ( byRed_Start   << 8 )); // Set Red
        stVertex[0] .Green  = SCAST( USHORT, ( byGreen_Start << 8 )); // Set Green
        stVertex[0] .Blue   = SCAST( USHORT, ( byBlue_Start  << 8 )); // Set Blue
        stVertex[0] .Alpha  = 0x0000;

        // Extract RGB from ending color
        const BYTE byRed_End    = GetRValue( clrEndColor_i );    // Red
        const BYTE byGreen_End  = GetGValue( clrEndColor_i );    // Green
        const BYTE byBlue_End   = GetBValue( clrEndColor_i );    // Blue

        // Set second stVertex
        stVertex[1] .x      = crDrawRect_i.right;
        stVertex[1] .y      = crDrawRect_i.bottom; 
        stVertex[1] .Red    = SCAST( USHORT, ( byRed_End   << 8 )); // Set Red
        stVertex[1] .Green  = SCAST( USHORT, ( byGreen_End << 8 )); // Set Green
        stVertex[1] .Blue   = SCAST( USHORT, ( byBlue_End  << 8 )); // Set Blue
        stVertex[1] .Alpha  = 0x0000;

        // Gradient stVertex recorder
        GRADIENT_RECT stGr_Rect[1] = { 0 };
        stGr_Rect[0].UpperLeft  = 0; // Upper left corner is in stVertex['0']
        stGr_Rect[0].LowerRight = 1; // Lower right corner is in stVertex['1']

        // Draw gradient rect
        return GradientFill( pDC_i->GetSafeHdc(),       // Target DC
                             stVertex,                  // Vertex array
                             _countof( stVertex ),     // Count of vertexes
                             &stGr_Rect,                // Gradient rect
                             _countof( stGr_Rect ),    // Count of gradient rectangles
                             bIsHorizontal_i ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V ) == TRUE ;    // Draw mode
    }// End DrawGradient

    static void FormatByteSize( const DWORD dwSize_i, CString& csFormat_o )
    {
        // Clear previous contents
        csFormat_o.Empty();

        // Set max buffer size
        const int nMaxSize = 40;

        // Get buffer of specified length
        LPTSTR lptszSizeBuffer = csFormat_o.GetBufferSetLength( nMaxSize );

        // If invalid return
        if( !lptszSizeBuffer )
        {
            csFormat_o.ReleaseBuffer();
            csFormat_o.Empty();
            return;
        }

        // Get formatted string
        if( !StrFormatByteSize( dwSize_i, lptszSizeBuffer, nMaxSize ))
        {
            lptszSizeBuffer[0] = 0;
        }

        // Release bound buffer
        csFormat_o.ReleaseBuffer();

    }// End FormatByteSize

    static LONG WINAPI UnhandledExceptionFilterFunc( EXCEPTION_POINTERS* pstExceptionInfo_i )
    {
        UNREFERENCED_PARAMETER( pstExceptionInfo_i );

        // Display error message
        Utils::ShowError( _T( "Unknown error occurred" ));

        return EXCEPTION_EXECUTE_HANDLER;
        
    }// End UnhandledExceptionFilterFunc

    static bool GetEnvironmentVarValue( LPCTSTR lpctszEnvVar_i, CString& csEnvVarValue_o )
    {
        // Get actual variable length
        const int nValueLength = GetEnvironmentVariable( lpctszEnvVar_i, 0, 0 );

        // Check if failed or not
        if( nValueLength == 0 )
        {
            return false;
        }

        // Retrieve buffer
        LPTSTR lptszValueBuf = csEnvVarValue_o.GetBufferSetLength( nValueLength + 1 );

        // Check if buffer is valid or not
        if( !lptszValueBuf )
        {
            return false;
        }

        const bool bResult = GetEnvironmentVariable( lpctszEnvVar_i, lptszValueBuf, nValueLength ) != 0;
        csEnvVarValue_o.ReleaseBuffer();

        // Check return value
        return bResult;

    }// GetEnvironmentVarValue

    static void PathAppendBackslash( CString& csPath_io )
    {
        // Get zero based length
        const int nLength = csPath_io.GetLength() - 1;

        // If there is no backslash to the end append one
        if( csPath_io[nLength] != _T( '\\' ))
        {
            csPath_io += _T( "\\" );
        }// End if
    }// End PathAppendBackslash

    static CString GetTempPath()
    {
        // Variable to hold temporary folder path
        CString csTempPath;

        // Get "TEMP" environment variable value
        if( !Utils::GetEnvironmentVarValue( _T( "tmp" ), csTempPath ))
        {
            if( !Utils::GetEnvironmentVarValue( _T( "temp" ), csTempPath ))
            {
                csTempPath = _T( "C:\\Temp\\" );
                if( !PathFileExists( csTempPath ))
                {
                    // Create this directory if this does not exists
                    CreateDirectory( csTempPath, 0 );
                }// End if
            }// End if
        }// End if

        // Append a backslash if needed
        Utils::PathAppendBackslash( csTempPath );

        // A temporary directory path
        csTempPath += _T( "RD____1\\" );

        // If directory does not exist create one
        if( !PathFileExists( csTempPath ))
        {
            CreateDirectory( csTempPath, 0 );
        }// End if

        // Return path
        return csTempPath;

    }// End SetTempPath

    static BOOL IsShortcut( LPCTSTR lpctszPath_i )
    {
        SHFILEINFO shFileInfo = { 0 };
		shFileInfo.dwAttributes = SFGAO_LINK;
        return (( SHGetFileInfo( lpctszPath_i, 
                                 0, 
                                 &shFileInfo, 
                                 sizeof( shFileInfo ),
                                 SHGFI_ATTR_SPECIFIED | SHGFI_ATTRIBUTES )) && ( shFileInfo.dwAttributes & SFGAO_LINK ));
    }

	// Find out target file for a shortcut
    static BOOL ResolveShortcut( LPTSTR lptszFileOut_io )
    {
        if( !lptszFileOut_io )
        {
            return FALSE;
        }

        // Object for resolving link
        CComPtr<IShellLink> ShellLinkPtr = NULL;
		HRESULT hCreateRes = ShellLinkPtr.CoCreateInstance( CLSID_ShellLink );
        if( FAILED( hCreateRes ) || ShellLinkPtr == NULL )
        {
            return FALSE;
        }

		// QueryInterface for persist file interface
        CComQIPtr<IPersistFile, &IID_IPersistFile> PersistFilePtr( ShellLinkPtr );
        if ( PersistFilePtr != NULL && SUCCEEDED( PersistFilePtr->Load( T2COLE( lptszFileOut_io ), STGM_READ)) )
        {
            // Specify how to resolve link, don't show "Failed to find shortcut" dialog.
            const WORD wFlags = (const WORD)(SLR_ANY_MATCH | SLR_NO_UI | SLR_NOUPDATE | SLR_NOSEARCH);

			// Start resolving
            if ( SUCCEEDED( ShellLinkPtr->Resolve( 0, wFlags )))
            {
				// Store path
                ShellLinkPtr->GetPath( lptszFileOut_io, MAX_PATH, NULL, 0 );
                return TRUE;
            }// End if
        }// End if

        return FALSE;
    }// End ResolveShortcut

    // Get resource language
    static bool GetResourceLanguage( DWORD dwLangID_i, LPCTSTR lpctszPrefix_i, CString& csLang_o )
    {
        const int MAX_LANG_LEN = 50;

        // Prepare LCID
        const LCID lcidLang = MAKELCID( dwLangID_i, SORT_DEFAULT );

        // Will hold language
        TCHAR szLangBuffer[MAX_LANG_LEN] = { 0 };

        // Get language
        DWORD dwCount = GetLocaleInfo( lcidLang, LOCALE_SENGLANGUAGE, szLangBuffer, MAX_LANG_LEN );
        if( !dwCount )
        {
            AfxTrace( _T( "Failed to get locale language information" ));
            return false;
        }// End if

        // Will hold country
        TCHAR szCountryBuffer[MAX_LANG_LEN] = { 0 };

        // Get country
        dwCount = GetLocaleInfo( lcidLang, LOCALE_SENGCOUNTRY, szCountryBuffer, MAX_LANG_LEN );

        if( !dwCount )
        {
            AfxTrace( _T( "Failed to get locale country information" ));
            return false;
        }// End if

        // Prepare language string
        csLang_o.Format( _T( "%s %s, %s" ), lpctszPrefix_i, szLangBuffer, szCountryBuffer );

        // Return execution status
        return true;
    }// End GetResourceLanguage

    // Returns the restored size of window
    static CRect GetRestoredSizeOfWindow( HWND hWindow_i )
    { 
        WINDOWPLACEMENT wpPlacement = { 0 };
        wpPlacement.length = sizeof( wpPlacement );

        ::GetWindowPlacement( hWindow_i, &wpPlacement );
        return wpPlacement.rcNormalPosition;
    }

    //************************************
    // Method:    IsValidMask
    // FullName:  Utils::IsValidMask
    // Access:    public static 
    // Returns:   bool
    // Qualifier:
    // Parameter: const DWORD dwValue_i
    // Parameter: const DWORD dwMask_i
    //************************************
    static bool IsValidMask( const DWORD dwValue_i, const DWORD dwMask_i )
    {
        return ( dwValue_i & dwMask_i ) == dwMask_i;
    }


    // If given key state is pressed then returns true
    static inline bool IsKeyDown( const UINT uKey_i )
    {
        return Utils::IsValidMask( GetKeyState( uKey_i ), 0x8000 );
    }

    // Control down
    static inline bool IsCtrlDown()
    {
        return IsKeyDown( VK_CONTROL );
    }

    // Alt key state
    static inline bool IsAltDOwn()
    {
        return IsKeyDown( VK_MENU );
    }

    static bool CopyTextToClipboard( HWND hWindow_i, LPCTSTR lpctszText_i )
    {
        // Open clipboard
        if( !lpctszText_i || !::OpenClipboard( hWindow_i ))
        {
            return false;
        }

        // Clear clipboard
        EmptyClipboard();

        const size_t nTotalAllocLen = ( _tcslen( lpctszText_i ) + 1 ) * sizeof( TCHAR ) ;
        HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE, nTotalAllocLen );
        if( !hGlobal )
        {
            CloseClipboard();
            return false;
        }

        // Lock allocated buffer for copying
        LPTSTR lptszCopyStr = RCAST( LPTSTR, GlobalLock( hGlobal ));
        memcpy( lptszCopyStr, lpctszText_i, nTotalAllocLen );
        GlobalUnlock( hGlobal );
        ASSERT( GetLastError() == NO_ERROR );

        // Clipboard format
        UINT uClipBoardFormat = 0;
        #ifdef _UNICODE
            uClipBoardFormat = CF_UNICODETEXT;
        #else
            uClipBoardFormat = CF_TEXT;
        #endif

        // Set data to clipboard
        HANDLE hClip = SetClipboardData( uClipBoardFormat, hGlobal );
        CloseClipboard();

        // Return status
        return ( hClip ? true : false );
    }// End CopyTextToClipboard

    static bool IsKeyRepeated( const UINT uFlags_i )
    {
        // For extracting bit 14
        const UINT uBit14 = SCAST( UINT, 1 ) << 14;
        const bool bRepetition = Utils::IsValidMask( uFlags_i, uBit14 );

        // True if 14th bit is set
        return bRepetition;
    }

    static void GetFormattedTime( CString& csStartTime_o, 
                                  FILETIME& stftTime_i, 
                                  const bool bConvertToLocalTime_i )
    {
        // It's mandatory to convert to FILETIME to system time, since 
        // GetProcessTimes documentation says so
        SYSTEMTIME sysTime = { 0 };

        if( bConvertToLocalTime_i && ( stftTime_i.dwLowDateTime != 0 || stftTime_i.dwHighDateTime != 0 ))
        {
            const FILETIME stftTemp = stftTime_i;
            FileTimeToLocalFileTime( &stftTemp, &stftTime_i );
        }

        FileTimeToSystemTime( &stftTime_i, &sysTime );
        csStartTime_o.Format( _T( "%02d:%02d:%02d" ), sysTime.wHour, sysTime.wMinute, sysTime.wSecond );
    }

   /** 
    * 
    * Searches for a file in PATH environment variable and other paths as specified 
    * in SearchPath documentation.
    * 
    * @param       lpctszFileName_i - File name to search for.
    * @param       csFilePath_o     - On return will hold search path.
    * @return      bool             - Returns execution status.
    * @exception   Nil
    * @see         Nil
    * @since       1.0
    */
    static bool FindFile( LPCTSTR lpctszFileName_i, CString& csFilePath_o )
    {
        const int nAllocLength = MAX_PATH * 2;
		TCHAR szPathBuffer[ nAllocLength ] = { 0 };

        // Search for file
        if( SearchPath( 0, lpctszFileName_i, 0, nAllocLength, szPathBuffer, 0 ))
        {
            csFilePath_o = szPathBuffer;
            return true;
        }

        return false;
    }


    // Returns size in formatted manner
    static void GetFormattedSize( CString& csSize_o, 
                                  const ULONGLONG dwSize_i )
    {
        const int BUFF_SIZE = 30;

        // Allocate sufficient space
        LPTSTR lptszSizeBuffer = csSize_o.GetBufferSetLength( BUFF_SIZE );
        StrFormatByteSize( dwSize_i, lptszSizeBuffer, BUFF_SIZE );
        csSize_o.ReleaseBuffer();
    }

    //************************************
    // Method:    SndMsgTimeOutHelper
    // FullName:  Utils::SndMsgTimeOutHelper
    // Access:    public static 
    // Returns:   bool
    // Qualifier:
    // Parameter: HWND hWnd_i
    // Parameter: UINT uMessage_i
    // Parameter: WPARAM wParam_i
    // Parameter: LPARAM lParam_i
    //************************************
    static bool SndMsgTimeOutHelper( HWND hWnd_i, 
                                     UINT uMessage_i, 
                                     WPARAM wParam_i, 
                                     LPARAM lParam_i,
                                     DWORD& dwResult_i )
    {
        const int nTimeOut = 100;
        return ::SendMessageTimeout( hWnd_i, uMessage_i, wParam_i, lParam_i, SMTO_ABORTIFHUNG | SMTO_BLOCK, nTimeOut, &dwResult_i ) != FALSE;
    }

    // Select a file in explorer
    static void SelectFileInExplorer( LPCTSTR lpctszFileToSelect_i )
    {  
       // This is the command line for explorer which tells it to select the given file  
       CString csPathbackup = _T( "/Select," );  
       csPathbackup +=  lpctszFileToSelect_i;
       
       // Prepare shell execution params  
       SHELLEXECUTEINFO shExecInfo   = { 0 };  
       shExecInfo.cbSize             = sizeof(shExecInfo);  
       shExecInfo.lpFile             = _T( "Explorer.exe" );  
       shExecInfo.lpParameters       = csPathbackup;  
       shExecInfo.nShow              = SW_SHOWNORMAL;  
       shExecInfo.lpVerb             = _T( "Open" ); // Context menu item   
       // Just have a look in MSDN to see the relevance of these flags  
       shExecInfo.fMask              = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI;      
       // Select file in explorer 
       VERIFY( ShellExecuteEx( &shExecInfo ));
    }

    static void ShowFilePropertiesDlg( LPCTSTR lpctszFileName_i )
    {
        if( !lpctszFileName_i )
        {
            return;
        }

        SHELLEXECUTEINFO shExecInfo = { 0 };
        shExecInfo.cbSize = sizeof(shExecInfo);
        shExecInfo.lpFile = lpctszFileName_i;
        shExecInfo.lpVerb = _T( "properties" ); // Context menu item
        shExecInfo.fMask  = SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_FLAG_NO_UI; // This is the key, see MSDN

        // Show properties
        if( !ShellExecuteEx( &shExecInfo ))
        {
            Utils::GetLastErrorMsg();
        }
    }// End ShowFilePropertiesDlg


#pragma auto_inline( on )
#pragma warning( pop )

}; // End namespace Utils

#endif //_UTILS__H