#include "stdafx.h"
#include "processviewer.h"
#include "Settings.h"
#include "DividerWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

Settings& g_Settings = Settings::Instance();

// ini file sections
LPCTSTR lpctszToolBarSection = _T( "Toolbar setting" );
LPCTSTR lpctszDividerSection = _T( "Divider" );
LPCTSTR lpctszSearchSection = _T( "Search" );
LPCTSTR lpctszProcessSection = _T( "Process" );
LPCTSTR lpctszMainWindow = _T( "MainWindow" );
LPCTSTR lpctszModuleSection = _T( "Modules" );
LPCTSTR lpctszGeneralSection = _T( "General" );


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Settings::Settings() :  m_nLastSearchChoice( 0 ),

                        m_bIsFullPathPressed( false ),
                        m_bIsDependsPressed( false ),

                        m_nDividerDirection( DIR_VERT ),
                        m_nMainDividerSpan( 250 ),
                        m_nProcessDividerSpan( 200 ),
                        m_nModuleDividerSpan( 200 ),

                        m_bProcessSize( true ),
                        m_bProcessType( true ),
                        m_bProcessHwnd( true ),
                        m_bProcessVersion( true ),
                        m_bProcessMemInfo( true ),
                        m_bProcessTimes( true ),
                        m_bProcessFileTimes( true ),
                        m_bProcessGDIResInfo( true ),
                        m_bProcessPriority( true ),
                        m_bProcessIOCounters( true ),
                        m_bProcessPrivileges( true ),
                        
                        m_bModuleCompany( true ),
                        m_bModuleDescription( true ),
                        m_bModuleEntryPoint( true ),
                        m_bModuleFileSize( true ),
                        m_bModuleImageSize( true ),
                        m_bModuleIndex( true ),
                        m_bModuleLoadAddress( true ),
                        m_bModuleName( true ),
                        m_bModulePath( true ),
                        m_bModuleVersion( true ),

                        m_bPromptBeforeKillingProcess( true )
{
    
    m_crMainWindowRect.SetRect( 200, 100, 800, 500 );
}

Settings::~Settings()
{}

Settings& Settings::Instance()
{
    static Settings __settings;    
    return __settings;
}

void Settings::Load()
{
    // Read rectangle co-ordinates
    ReadInt( lpctszMainWindow, _T( "Co-ordsLeft" ), ( int& )m_crMainWindowRect.left );
    ReadInt( lpctszMainWindow, _T( "Co-ordsTop" ), ( int& )m_crMainWindowRect.top );
    ReadInt( lpctszMainWindow, _T( "Co-ordsRight" ), ( int& )m_crMainWindowRect.right );
    ReadInt( lpctszMainWindow, _T( "Co-ordsBottom" ), ( int& )m_crMainWindowRect.bottom );

    // Load search options
    ReadString( lpctszSearchSection, _T( "SrchString" ), m_csLastSrchString );
    ReadInt( lpctszSearchSection, _T( "SrchOption" ), m_nLastSearchChoice );

    // Load Toolbar settings
    ReadInt( lpctszToolBarSection, _T( "Depends" ), m_bIsDependsPressed );
    ReadInt( lpctszToolBarSection, _T( "FullPath" ), m_bIsFullPathPressed );

    // Load divider settings
    ReadInt( lpctszDividerSection, _T( "Direction" ), m_nDividerDirection );
    ReadInt( lpctszDividerSection, _T( "MainDividerSpan" ), m_nMainDividerSpan );
    ReadInt( lpctszDividerSection, _T( "ProcessDividerSpan" ), m_nProcessDividerSpan );
    ReadInt( lpctszDividerSection, _T( "ModuleDividerSpan" ), m_nModuleDividerSpan );

    // Load process detail settings
    ReadInt( lpctszProcessSection, _T( "ProcessSize" ), m_bProcessSize );
	ReadInt( lpctszProcessSection, _T( "ProcessType" ), m_bProcessType );
	ReadInt( lpctszProcessSection, _T( "ProcessHWND" ), m_bProcessHwnd );
	ReadInt( lpctszProcessSection, _T( "ProcessVersion" ), m_bProcessVersion );
	ReadInt( lpctszProcessSection, _T( "ProcessMemInfo" ), m_bProcessMemInfo );
	ReadInt( lpctszProcessSection, _T( "ProcessProcTimes" ), m_bProcessTimes );
	ReadInt( lpctszProcessSection, _T( "ProcessFileTimes" ), m_bProcessFileTimes );
	ReadInt( lpctszProcessSection, _T( "ProcessResInfo" ), m_bProcessGDIResInfo );
    ReadInt( lpctszProcessSection, _T( "ProcessPriority" ), m_bProcessPriority );
    ReadInt( lpctszProcessSection, _T( "ProcessIOCounters" ), m_bProcessIOCounters );
    ReadInt( lpctszProcessSection, _T( "ProcessPrivileges" ), m_bProcessPrivileges );

    // Load module display settings
    ReadInt( lpctszModuleSection, _T( "ModuleCompany" ), m_bModuleCompany );
    ReadInt( lpctszModuleSection, _T( "ModuleDescription" ), m_bModuleDescription );
    ReadInt( lpctszModuleSection, _T( "ModuleEntryPoint" ), m_bModuleEntryPoint );
    ReadInt( lpctszModuleSection, _T( "ModuleFileSize" ), m_bModuleFileSize );
    ReadInt( lpctszModuleSection, _T( "ModuleImageSize" ), m_bModuleImageSize );
    ReadInt( lpctszModuleSection, _T( "ModuleIndex" ), m_bModuleIndex );
    ReadInt( lpctszModuleSection, _T( "ModuleLoadAddress" ), m_bModuleLoadAddress );
    ReadInt( lpctszModuleSection, _T( "ModuleName" ), m_bModuleName );
    ReadInt( lpctszModuleSection, _T( "ModulePath" ), m_bModulePath );
    ReadInt( lpctszModuleSection, _T( "ModuleVersion" ), m_bModuleVersion );

    // Read in general settings
    ReadInt( lpctszGeneralSection, _T( "PromptBeforeKillingProcess" ), m_bPromptBeforeKillingProcess );
}

void Settings::Save()
{
    // Write rect down
    WriteInt( lpctszMainWindow, _T( "Co-ordsLeft" ), m_crMainWindowRect.left );
    WriteInt( lpctszMainWindow, _T( "Co-ordsTop" ), m_crMainWindowRect.top );
    WriteInt( lpctszMainWindow, _T( "Co-ordsRight" ), m_crMainWindowRect.right );
    WriteInt( lpctszMainWindow, _T( "Co-ordsBottom" ), m_crMainWindowRect.bottom );

    // Save search options
    WriteString( lpctszSearchSection, _T( "SrchString" ), m_csLastSrchString );
    WriteInt( lpctszSearchSection, _T( "SrchOption" ), m_nLastSearchChoice );

    // Save Toolbar settings
    WriteInt( lpctszToolBarSection, _T( "Depends" ), m_bIsDependsPressed );
    WriteInt( lpctszToolBarSection, _T( "FullPath" ), m_bIsFullPathPressed );

    // Save divider settings
    WriteInt( lpctszDividerSection, _T( "Direction" ), m_nDividerDirection );
    WriteInt( lpctszDividerSection, _T( "MainDividerSpan" ), m_nMainDividerSpan );
    WriteInt( lpctszDividerSection, _T( "ProcessDividerSpan" ), m_nProcessDividerSpan );
    WriteInt( lpctszDividerSection, _T( "ModuleDividerSpan" ), m_nModuleDividerSpan );

    // Write process settings
    WriteInt( lpctszProcessSection, _T( "ProcessSize" ), m_bProcessSize );
	WriteInt( lpctszProcessSection, _T( "ProcessType" ), m_bProcessType );
	WriteInt( lpctszProcessSection, _T( "ProcessHWND" ), m_bProcessHwnd );
	WriteInt( lpctszProcessSection, _T( "ProcessVersion" ), m_bProcessVersion );
	WriteInt( lpctszProcessSection, _T( "ProcessMemInfo" ), m_bProcessMemInfo );
	WriteInt( lpctszProcessSection, _T( "ProcessProcTimes" ), m_bProcessTimes );
	WriteInt( lpctszProcessSection, _T( "ProcessFileTimes" ), m_bProcessFileTimes );
	WriteInt( lpctszProcessSection, _T( "ProcessResInfo" ), m_bProcessGDIResInfo );
    WriteInt( lpctszProcessSection, _T( "ProcessPriority" ), m_bProcessPriority );
    WriteInt( lpctszProcessSection, _T( "ProcessIOCounters" ), m_bProcessIOCounters );
    WriteInt( lpctszProcessSection, _T( "ProcessPrivileges" ), m_bProcessPrivileges );

    // Write module settings
    WriteInt( lpctszModuleSection, _T( "ModuleCompany" ), m_bModuleCompany );
    WriteInt( lpctszModuleSection, _T( "ModuleDescription" ), m_bModuleDescription );
    WriteInt( lpctszModuleSection, _T( "ModuleEntryPoint" ), m_bModuleEntryPoint );
    WriteInt( lpctszModuleSection, _T( "ModuleFileSize" ), m_bModuleFileSize );
    WriteInt( lpctszModuleSection, _T( "ModuleImageSize" ), m_bModuleImageSize );
    WriteInt( lpctszModuleSection, _T( "ModuleIndex" ), m_bModuleIndex );
    WriteInt( lpctszModuleSection, _T( "ModuleLoadAddress" ), m_bModuleLoadAddress );
    WriteInt( lpctszModuleSection, _T( "ModuleName" ), m_bModuleName );
    WriteInt( lpctszModuleSection, _T( "ModulePath" ), m_bModulePath );
    WriteInt( lpctszModuleSection, _T( "ModuleVersion" ), m_bModuleVersion );

    // Read in general settings
    WriteInt( lpctszGeneralSection, _T( "PromptBeforeKillingProcess" ), m_bPromptBeforeKillingProcess );
}// End Save

void Settings::SaveBinary( LPCTSTR lpctszSection_i, 
                           LPCTSTR lpctszEntry_i, 
                           int* pColumnOrder_i, 
                           const unsigned int uCount_i )
{
    WriteBinary( lpctszSection_i, 
                 lpctszEntry_i, 
                 RCAST( LPBYTE, pColumnOrder_i ), 
                 uCount_i * sizeof( int ));
}

void Settings::LoadBinary( LPCTSTR lpctszSection_i, LPCTSTR lpctszEntry_i, int** pColumnOrder_i, unsigned int& uCount_o )
{
    ReadBinary( lpctszSection_i, 
                lpctszEntry_i, 
                RCAST( LPBYTE*, pColumnOrder_i ), 
                uCount_o );

    uCount_o /= sizeof( int );
}

BOOL Settings::WriteString( LPCTSTR lpctszSection_i, 
                            LPCTSTR lpctszEntry_i, 
                            LPCTSTR lpctszVal_i )
{
    return AfxGetApp()->WriteProfileString( lpctszSection_i, 
                                            lpctszEntry_i, 
                                            lpctszVal_i );
}

BOOL Settings::ReadString( LPCTSTR lpctszSection_i,
                           LPCTSTR lpctszEntry_i,
                           CString& csVal_o )
{
    csVal_o = AfxGetApp()->GetProfileString( lpctszSection_i, lpctszEntry_i );
    return TRUE;
}

BOOL Settings::WriteInt( LPCTSTR lpctszSection_i, 
                         LPCTSTR lpctszEntry_i, 
                         const int nVal_i )
{
    return AfxGetApp()->WriteProfileInt( lpctszSection_i, 
                                         lpctszEntry_i, 
                                         nVal_i );
}

BOOL Settings::ReadInt( LPCTSTR lpctszSection_i, 
                        LPCTSTR lpctszEntry_i, 
                        int& nVal_o )
{
    nVal_o = AfxGetApp()->GetProfileInt( lpctszSection_i, 
                                         lpctszEntry_i, 
                                         nVal_o );

    return TRUE;
}

BOOL Settings::WriteBinary( LPCTSTR lpctszSection_i,
                            LPCTSTR lpctszEntry_i,
                            unsigned char* pucBinary_i,
                            const unsigned int uLength_i )
{
    return AfxGetApp()->WriteProfileBinary( lpctszSection_i, 
                                            lpctszEntry_i, 
                                            pucBinary_i, 
                                            uLength_i );
}

BOOL Settings::ReadBinary( LPCTSTR lpctszSection_i, 
                           LPCTSTR lpctszEntry_i,
                           unsigned char** ppucBinary_o,
                           unsigned int& uLength_o )
{
    return AfxGetApp()->GetProfileBinary( lpctszSection_i, 
                                          lpctszEntry_i, 
                                          ppucBinary_o, 
                                          &uLength_o );
}