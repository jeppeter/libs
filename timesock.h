#ifndef	__TIME_SOCKET__
#define	__TIME_SOCKET__

#include <stdio.h>

#ifndef	DEBUG_PRINT
#define	DEBUG_PRINT(...) do{fprintf(stdout,"[%-30s][%5d]:\t",__FILE__,__LINE__);fprintf(stdout,__VA_ARGS__);}while(0)
#endif

#ifndef ERROR_PRINT
#define ERROR_PRINT(...) do{{fprintf(stderr,"[%-30s][%5d][ERROR]:\t",__FILE__,__LINE__);fprintf(stderr,__VA_ARGS__);}}while(0)
#endif

typedef int BOOL;


class CTimeSocket
{
public:
    CTimeSocket ( int timeout );
    ~CTimeSocket();
    BOOL Connect ( char* ip, int port );
    BOOL Close();
    BOOL Bind ( int port, int reuse );
    BOOL Write ( void* pBuf, int len );
    BOOL Read ( void* pBuf, int len );
    int	GetSocket();
    BOOL AcceptSocket ( int& accsock, struct sockaddr_in *pAddr, int *pSize, BOOL bTimeout );
    BOOL InitAcceptSocket ( int accsock );
    int GetError();
	BOOL SetTimeout(int timeout);
private:
    int m_Sock;
    int m_Timeout;
    int m_Error;
};

#endif /*__TIME_SOCKET__*/

