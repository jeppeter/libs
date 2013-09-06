// Settings.h: interface for the Settings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGS_H__3DB8CA2E_AE23_4B23_B745_7CA5980B28E8__INCLUDED_)
#define AFX_SETTINGS_H__3DB8CA2E_AE23_4B23_B745_7CA5980B28E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Settings  
{
public:
    virtual ~Settings();
    static Settings& Instance();

    void Load();
    void Save();

    // Save binary information to ini file
    void SaveBinary( LPCTSTR lpctszSection_i, 
                     LPCTSTR lpctszEntry_i, 
                     int* pColumnOrder_i, 
                     const unsigned int uCount_i );

    // Load binary information from ini file
    void LoadBinary( LPCTSTR lpctszSection_i, 
                     LPCTSTR lpctszEntry_i, 
                     int** pColumnOrder_i, 
                     unsigned int& uCount_o );

    // Save main window rectangle
    CRect m_crMainWindowRect;

    // Search setting
    CString m_csLastSrchString;
    int m_nLastSearchChoice;

    // Toolbar setting
    BOOL m_bIsFullPathPressed;
    BOOL m_bIsDependsPressed;

    // Divider settings
    int m_nDividerDirection;
    int m_nMainDividerSpan;
    int m_nProcessDividerSpan;
    int m_nModuleDividerSpan;

    // Process details visibility flags
    BOOL m_bProcessSize;
	BOOL m_bProcessType;
	BOOL m_bProcessHwnd;
	BOOL m_bProcessVersion;
	BOOL m_bProcessMemInfo;
	BOOL m_bProcessTimes;
	BOOL m_bProcessFileTimes;
	BOOL m_bProcessGDIResInfo;
    BOOL m_bProcessPriority;
    BOOL m_bProcessIOCounters;
    BOOL m_bProcessPrivileges;

    // Module details visibility flags
	BOOL m_bModuleCompany;
	BOOL m_bModuleDescription;
	BOOL m_bModuleEntryPoint;
	BOOL m_bModuleFileSize;
	BOOL m_bModuleImageSize;
	BOOL m_bModuleIndex;
	BOOL m_bModuleLoadAddress;
	BOOL m_bModuleName;
	BOOL m_bModulePath;
	BOOL m_bModuleVersion;

    // Prompt before killing process
    BOOL m_bPromptBeforeKillingProcess;

private:

    // Constructor disabled
	Settings();

    // Write CString to ini file
    BOOL WriteString( LPCTSTR lpctszSection_i, 
                      LPCTSTR lpctszEntry_i, 
                      LPCTSTR lpctszVal_i );

    // Read value from ini file
    BOOL ReadString( LPCTSTR lpctszSection_i,
                     LPCTSTR lpctszEntry_i,
                     CString& csVal_o );

    // Write an integer value to an ini file
    BOOL WriteInt( LPCTSTR lpctszSection_i, 
                   LPCTSTR lpctszEntry_i, 
                   int nVal_i );

    // Read an integer value from ini file
    BOOL ReadInt( LPCTSTR lpctszSection_i, 
                  LPCTSTR lpctszEntry_i, 
                  int& nVal_o );

    // Write binary value
    BOOL WriteBinary( LPCTSTR lpctszSection_i,
                      LPCTSTR lpctszEntry_i,
                      unsigned char* pucBinary_i,
                      const unsigned int uLength_i );

    BOOL ReadBinary( LPCTSTR lpctszSection_i,
                     LPCTSTR lpctszEntry_i,
                     unsigned char** ppucBinary_o,
                     unsigned int& uLength_o );
};

#endif // !defined(AFX_SETTINGS_H__3DB8CA2E_AE23_4B23_B745_7CA5980B28E8__INCLUDED_)
