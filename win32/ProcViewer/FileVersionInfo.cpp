#include "stdafx.h"
#include "FileVersionInfo.h"


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
FileVersionInfo::FileVersionInfo() : m_pVersionData( 0 ), 
                                     m_dwVersionSize( 0 ),
                                     m_pLang( 0 )
{}


/** 
 * 
 * Copy constructor overloaded.
 * 
 * @param       FileVersionInfo - File version information to copy
 * @return      Nil
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
FileVersionInfo::FileVersionInfo( const FileVersionInfo& fviVersionInfo_i )
{
    // Call default constructor first
    this->FileVersionInfo::FileVersionInfo();
    *this = fviVersionInfo_i;
}


/** 
 * 
 * Assignment operator overloaded.
 * 
 * @param       fviVersionInfo_i - Version information to copy. 
 * @return      FileVersionInfo& - Returns a reference to this class.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
FileVersionInfo& FileVersionInfo::operator = ( const FileVersionInfo& fviVersionInfo_i )
{
    // Prevent self assignment
    if( this != &fviVersionInfo_i )
    {
        // Delete previous version if any
        if( m_pVersionData )
        {
            DELETE_PTR_ARR( m_pVersionData );
        }

        m_pVersionData = new VersionData[fviVersionInfo_i.m_dwVersionSize];
        if( m_pVersionData )
        {
            memcpy( m_pVersionData, 
                    fviVersionInfo_i.m_pVersionData, 
                    fviVersionInfo_i.m_dwVersionSize ); 
        }// End if
    }// End if

    return *this;
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
FileVersionInfo::~FileVersionInfo()
{
    if( m_pVersionData )
    {
        DELETE_PTR_ARR( m_pVersionData );
    }
}


/** 
 * 
 * Should called once before extraction of version information. Calling more
 * than once on a file, unless some change has taken place, does not help.
 * 
 * @param       Nil
 * @return      bool - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetInfo()
{
    // Get status of file
    CFile::GetStatus( m_csFilePath, m_fsStatus );

    // Verify
    LPTSTR lptszFileName = m_csFilePath.GetBuffer( 0 );

    // Handle of version
    DWORD dwHandle = 0;

    // Get version size
    const DWORD dwSize = GetFileVersionInfoSize( lptszFileName, &dwHandle );

    // Is size valid
    if( !dwSize )
    {
        return false;
    }

    // Version information
    if( m_pVersionData )
    {
        DELETE_PTR_ARR( m_pVersionData );
    }

    // Allocate version data buffer
    m_pVersionData = new VersionData[ dwSize ];
    if( !m_pVersionData )
    {
        return false;
    }
    
    if( !GetFileVersionInfo( lptszFileName, dwHandle, dwSize, m_pVersionData ))
    {
        DELETE_PTR_ARR( m_pVersionData );
        return false;
    }

    // Extract language list
    if( !GetLanguageList() )
    {
        return false;
    }


    return true;
}// End GetInfo


/** 
 * 
 * Returns product version.
 * 
 * @param       csVersion_o - On return will product version
 * @return      bool        - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetProductVersion( CString& csVersion_o ) const
{
    return ExtractStringInfo( _T( "ProductVersion" ), csVersion_o );
}// GetProductVersion



/** 
 * 
 * Returns file version.
 * 
 * @param       csVersion_o - Version
 * @return      bool        - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetFileVersion( CString& csVersion_o ) const
{
    return ExtractStringInfo( _T( "FileVersion" ), csVersion_o );
}// End GetFileVersion


/** 
 * 
 * Extracts file description.
 * 
 * @param       csFileDesc_o - File description.
 * @return      bool         - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetFileDescription( CString& csFileDesc_o ) const
{
    return ExtractStringInfo( _T( "FileDescription" ), csFileDesc_o );
}


/** 
 * 
 * Returns company name
 * 
 * @param       csCompanyName_o - On return will hold company name.
 * @return      bool            - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetCompanyName( CString& csCompanyName_o ) const
{
    return ExtractStringInfo( _T( "CompanyName" ), csCompanyName_o );
}


/** 
 * 
 * Returns product name.
 * 
 * @param       csProductName_o - Product name.
 * @return      bool            - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetProductName( CString& csProductName_o ) const
{
    return ExtractStringInfo( _T( "ProductName" ), csProductName_o );
}


/** 
 * 
 * Returns comments.
 * 
 * @param       csComments_o - On return will hold comments
 * @return      bool         - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetComments( CString& csComments_o ) const
{
    return ExtractStringInfo( _T( "Comments" ), csComments_o );
}


/** 
 * 
 * Returns internal name of product
 * 
 * @param       csInternalName_o - Internal name
 * @return      bool             - Returns execution status
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetInternalName( CString& csInternalName_o ) const
{
    return ExtractStringInfo( _T( "InternalName" ), csInternalName_o );
}


/** 
 * 
 * Returns legal copyright of file.
 * 
 * @param       csLegalCopyright_o - On return will hold legal copyright of file.
 * @return      bool               - Return execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetLegalCopyright( CString& csLegalCopyright_o ) const
{
    return ExtractStringInfo( _T( "LegalCopyright" ), csLegalCopyright_o );
}


/** 
 * 
 * Returns legal trademarks of file.
 * 
 * @param       csLegalTrademarks_o - On return will hold legal trademarks.
 * @return      bool                - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetLegalTrademarks( CString& csLegalTrademarks_o ) const
{
    return ExtractStringInfo( _T( "LegalTrademarks" ), csLegalTrademarks_o );
}


/** 
 * 
 * Return private build string of file.
 * 
 * @param       csPrivateBuild_o - On return will hold private build information.
 * @return      bool             - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetPrivateBuild( CString& csPrivateBuild_o ) const
{
    return ExtractStringInfo( _T( "PrivateBuild" ), csPrivateBuild_o );
}


/** 
 * 
 * Return original file name of file.
 * 
 * @param       csOriginalFileName_o - On return will hold original file name.
 * @return      bool                 - Return execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetOriginalFileName( CString& csOriginalFileName_o ) const
{
    return ExtractStringInfo( _T( "OrignalFileName" ), csOriginalFileName_o );
}


/** 
 * 
 * Returns special build of file.
 * 
 * @param       csSpecialBuild_o - On return will hold special build
 * @return      bool             - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetSpecialBuild( CString& csSpecialBuild_o ) const
{
    return ExtractStringInfo( _T( "SpecialBuild" ), csSpecialBuild_o );
}


/** 
 * 
 * Returns size of file.
 * 
 * @param       Nil
 * @return      DWORD - Size of file
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
ULONGLONG FileVersionInfo::GetFileSize() const
{
    return m_fsStatus.m_size;
}


/** 
 * 
 * Returns status of file
 * 
 * @param       Nil
 * @return      CFileStatus - Status of file
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
const CFileStatus& FileVersionInfo::GetFileStatus() const
{
    return m_fsStatus;
}


/** 
 * 
 * Extracts list of languages supported
 * 
 * @param       Nil
 * @return      bool - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::GetLanguageList()
{
    // Verify version
    if( !m_pVersionData )
    {
        return false;
    }

    // Prepare language block
    CString csBlock = _T( "\\VarFileInfo\\Translation" );

    // Get length
    UINT uLength = 0;
    const bool bResult =  ( VerQueryValue( m_pVersionData, 
                                           csBlock.GetBuffer( 0 ), 
                                           reinterpret_cast<void**>( &m_pLang ), 
                                           &uLength ) && uLength );

    csBlock.ReleaseBuffer();
    return bResult;
}


/** 
 * 
 * Returns information from specified block.
 * 
 * @param       lpctszBlockString_i - Block
 * @param       ppvData_io          - Data to return
 * @return      bool                - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::ExtractInfo( LPCTSTR lpctszBlockString_i, LPVOID* ppvData_io ) const
{
    if( !lpctszBlockString_i || !ppvData_io || !m_pVersionData || !m_pLang )
    {
        return false;
    }

    CString csBlock;
    csBlock.Format( _T( "\\StringFileInfo\\%04X%04X\\%s" ), 
                        m_pLang[0].wLanguage, 
                        m_pLang[0].wCodePage,
                        lpctszBlockString_i );

    UINT uVersionDataLength = 0;
    const BOOL bResult = VerQueryValue( m_pVersionData, 
                                        csBlock.GetBuffer( 0 ),
                                        ppvData_io,
                                        &uVersionDataLength );
    csBlock.ReleaseBuffer();
    return uVersionDataLength != 0 && bResult != 0;
}


/** 
 * 
 * Extracts string type of information from file.
 * 
 * @param       lpctszBlockString_o - Identifies block.
 * @param       csInfo_o            - On return will hold information
 * @return      bool                - Returns execution status.
 * @exception   Nil
 * @see         Nil
 * @since       1.0
 */
bool FileVersionInfo::ExtractStringInfo( LPCTSTR lpctszBlockString_o, CString& csInfo_o ) const
{
    LPTSTR lptszInfo = 0;
    
    // Extract file description information
    if( !ExtractInfo( lpctszBlockString_o, ( LPVOID* )&lptszInfo ))
    {
        return false;
    }

    // Return value
    csInfo_o = lptszInfo;
    return true;
}// End ExtractStringInfo