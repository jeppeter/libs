#ifndef _LOG_H_
#define _LOG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TRACE_MSG( msg_str ) g_Log.Write( _T( "%s, Location File: %s, Line: %d\n" ), \
                                          msg_str, \
                                          _tcsrchr( _T( __FILE__ ), _T( '\\' )), \
                                          __LINE__ )
#define TRACE_ERR( err_str ) TRACE_MSG( _T( "Error: " ) _T( err_str ))

class Log
{
    public:

	    Log();
	    virtual ~Log();

        bool Open( LPCTSTR lpctszFileName_i );
        Log& operator << ( LPCTSTR lpctszLogString_i );
        bool Write( LPCTSTR lpctszFormatString_i, ... );

    private:

        CStdioFile m_cfLogStream;
};// End Log

#endif // _LOG_H_
