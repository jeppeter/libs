// FileVersionInfo.h: interface for the FileVersionInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FILE_VERSION_INFO_H_
#define _FILE_VERSION_INFO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define DELETE_PTR( ptr ) delete ptr;\
                          ptr = 0;

#define DELETE_PTR_ARR( ptrArr ) delete [] ptrArr;\
                                 ptrArr = 0;

typedef unsigned char VersionData;


/**
 * Copyright(c) 2007 Nibu babu thomas
 *
 * FileVersionInfo - For extracting version information
 *
 * @author :    Nibu babu thomas
 * @version:    1.0            Date:  2007-05-28
 */
class FileVersionInfo  
{
    public:

	    FileVersionInfo();
        FileVersionInfo( const FileVersionInfo& );
        FileVersionInfo& operator = ( const FileVersionInfo& );
	    virtual ~FileVersionInfo();

        void SetFilePath( LPCTSTR lpctszFilePath_i ){ m_csFilePath = lpctszFilePath_i; }
        const CString& GetFilePath() const { return m_csFilePath; }

		bool IsVersionRetrieved() const
		{
			return m_pVersionData != 0;
		}

        bool GetInfo();
        bool GetProductVersion( CString& csVersion_o ) const;
        bool GetFileVersion( CString& csVersion_o ) const;
        bool GetFileDescription( CString& csFileDesc_o ) const;
        bool GetCompanyName( CString& csCompanyName_o ) const;
        bool GetProductName( CString& csProductName_o ) const;
        bool GetComments( CString& csComments_o ) const;
        bool GetInternalName( CString& csInternalName_o ) const;
        bool GetLegalCopyright( CString& csLegalCopyright_o ) const;
        bool GetLegalTrademarks( CString& csLegalTrademarks_o ) const;
        bool GetPrivateBuild( CString& csPrivateBuild_o ) const;
        bool GetOriginalFileName( CString& csOriginalFileName_o ) const;
        bool GetSpecialBuild( CString& csSpecialBuild_o ) const;

        // Function for returning size of file
        ULONGLONG GetFileSize() const;
        const CFileStatus& GetFileStatus() const;

    private:

        bool GetLanguageList();
        bool ExtractInfo( LPCTSTR lpctszBlockString_i, 
                          LPVOID* ppvData_io ) const;
        bool ExtractStringInfo( LPCTSTR lpctszBlockString_o, 
                                CString& csInfo_o ) const;

    private:

        // Language code page
        typedef struct LANG_CP_t
        {
            WORD wLanguage; // Language
            WORD wCodePage; // Code page
        }*PLANG_CP_t;

        CString m_csFilePath;
        VersionData* m_pVersionData;
        DWORD m_dwVersionSize;
        CFileStatus m_fsStatus;
        PLANG_CP_t m_pLang;

};// End FileVersionInfo

#endif // _FILE_VERSION_INFO_H_
