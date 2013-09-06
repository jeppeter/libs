#include "stdafx.h"
#include "Log.h"

Log::Log()
{}

Log::~Log()
{}

// Input redirection operator
Log& Log::operator << ( LPCTSTR lpctszLogString_i )
{
    if( !Write( lpctszLogString_i ))
    {
        ASSERT( FALSE );
    }

    // Return a reference to this class
    return *this;
}

// Write log string
bool Log::Write( LPCTSTR lpctszFormatString_i, ... )
{
    // Prepare for variable argument reading
    va_list vaArgumentList = 0;
    va_start( vaArgumentList, lpctszFormatString_i );

    // Variable argument string, ask CString to do the stuff for us
    CString csVarArgString;
    csVarArgString.FormatV( lpctszFormatString_i, vaArgumentList );
    va_end( vaArgumentList );

    // Write string to stream
    m_cfLogStream.WriteString( csVarArgString );

    return true;
}

// Open a log file for reading and writing
bool Log::Open( LPCTSTR lpctszFileName_i )
{
    // Flags for opening a file
    const UINT uFlags = CFile::modeReadWrite | CFile::modeNoTruncate | CFile::modeCreate | CFile::typeText | CFile::shareDenyNone;

    // Open file
    if( !m_cfLogStream.Open( lpctszFileName_i,  uFlags, 0 ))
    {
        ASSERT( FALSE );
        return false;
    }// End if

    m_cfLogStream.SeekToEnd();

    return true;
}