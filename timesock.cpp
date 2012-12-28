
#ifndef	__TIMEOUT_SOCKET_CPP__
#define	__TIMEOUT_SOCKET_CPP__

#include <assert.h>
#include <time.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <memory>
#include <netdb.h>
#include <string.h>
#include <timesock.h>



CTimeSocket::CTimeSocket ( int timeout ) : m_Sock ( -1 ), m_Timeout(timeout),m_Error ( 0 ) 
{
}

CTimeSocket::~CTimeSocket()
{
    this->Close();
    this->m_Timeout = 0;
    assert ( this->m_Sock == -1 );
    this->m_Error = 0;
}

BOOL CTimeSocket::SetTimeout(int timeout)
{
	this->m_Timeout = timeout;
	return true;
}

int CTimeSocket::GetSocket()
{
    return this->m_Sock;
}

int CTimeSocket::GetError()
{
    return this->m_Error;
}

BOOL CTimeSocket::AcceptSocket ( int & accsock, struct sockaddr_in * pAddr, int * pSize, BOOL bTimeout )
{
    int ret;
	int timeout;
    if ( this->m_Sock < 0 )
    {
		this->m_Error = EINVAL;
        return false;
    }
    if ( bTimeout )
    {
        /*if set timeout ,so we should select */
        time_t start, curr, left;
        start = time ( NULL );
		timeout = this->m_Timeout;
        while ( 1 )
        {
            fd_set rset;
            struct timeval tmout;
            curr = time ( NULL );
            left = timeout - ( curr - start );
            if ( left < 1 && timeout )
            {
                /*if we have time out*/
                this->m_Error = ETIMEDOUT;
                return false;
            }
            FD_ZERO ( &rset );
            FD_SET ( this->m_Sock , &rset );
            tmout.tv_sec = left;
            tmout.tv_usec = 0;

            ret = select ( this->m_Sock + 1, &rset, NULL, NULL, timeout ? &tmout : NULL );
            if ( ret < 0 )
            {
                /*it is error*/
                this->m_Error = errno;
                return false;
            }
            else if ( ret == 0 )
            {
				this->m_Error = ETIMEDOUT;
                return false;
            }

            if ( FD_ISSET ( this->m_Sock, &rset ) )
            {
                ret = accept ( this->m_Sock, ( struct sockaddr* ) pAddr, ( socklen_t* ) pSize );
                if ( ret < 0 )
                {
					this->m_Error = errno;
                    return false;
                }
				accsock = ret;
				return true;
            }
        }
    }
    else
    {
        while ( 1 )
        {
            fd_set rset;
            FD_ZERO ( &rset );
            FD_SET ( this->m_Sock, &rset );
            ret = select ( this->m_Sock + 1, &rset, NULL, NULL, NULL );
            if ( ret < 0 )
            {
				this->m_Error = errno;
                return false;
            }
            else if ( ret == 0 )
            {
				this->m_Error = ETIMEDOUT;
                return false;
            }
            ret = accept ( this->m_Sock, ( struct sockaddr* ) pAddr, ( socklen_t* ) pSize );
            if ( ret < 0 )
            {
				this->m_Error = errno;
                return false;
            }
            accsock = ret;
            return true;
        }
    }
    return false;
}

BOOL CTimeSocket::InitAcceptSocket ( int accsock )
{
    int ret;
    int flags;
    if ( this->m_Sock >= 0 )
    {
		this->m_Error = EINVAL;
        return false;
    }

    flags = fcntl ( accsock, F_GETFL, 0 );
    if ( flags == -1 )
    {
		this->m_Error = errno;
		ERROR_PRINT("[%d]errno %d %m\n",accsock,errno);
        return false;
    }
    ret = fcntl ( accsock, F_SETFL, flags | O_NONBLOCK );
    if ( ret < 0 )
    {
		this->m_Error = errno;
        return false;
    }
    this->m_Sock = accsock;
    return true;
}

BOOL CTimeSocket::Connect ( char * ip, int port )
{
    int sock = -1;
    int ret;
    struct hostent *hent = NULL;
    int val;
    int vallen;
    struct sockaddr_in saddr;
    int saddrlen;
    time_t start, curr;
    int lefttime;
    fd_set wset;
    struct timeval tmout;
    int hentsize = 128;
    std::auto_ptr<char> pBuf2 ( new char[hentsize] );
    char * pBuf;
    int herror = 0;
	int tryagain=0;
    struct hostent hentdummy;

    if ( this->m_Sock >= 0 )
    {
		this->m_Error = EINVAL;
        return false;
    }
try_alloc:
    pBuf2.reset ( new char[hentsize] );
    pBuf = pBuf2.get();
    herror = 0;
    ret = gethostbyname_r ( ip, &hentdummy, pBuf, hentsize, &hent, &herror );
    if ( ret < 0 )
    {
        if ( herror == TRY_AGAIN )
        {
			DEBUG_PRINT("hentsize %d errno = %d\n",hentsize,errno);
			tryagain ++;
			if (tryagain > 3)
			{
				this->m_Error = EHOSTUNREACH;
				goto fail;
			}
            goto try_alloc;
        }
		switch(herror)
		{
		case HOST_NOT_FOUND:
		case NO_ADDRESS:
			this->m_Error = ENODATA;
			break;
		case NO_RECOVERY:
			this->m_Error = ENOTRECOVERABLE;
			break;
		default:
			this->m_Error = ENOENT;
			break;
		}
        /*can not get the */
        goto fail;
    }
    if ( herror == -1 )
    {
        hentsize <<= 1;
		DEBUG_PRINT("hentsize %d errno = %d\n",hentsize,errno);
        goto try_alloc;
    }
    if ( herror )
    {
        if ( herror == TRY_AGAIN )
        {
			DEBUG_PRINT("hentsize %d errno = %d\n",hentsize,errno);
			tryagain ++;
			if (tryagain > 3)
			{
				this->m_Error = EHOSTUNREACH;
				goto fail;
			}
            goto try_alloc;
        }
		switch(herror)
		{
		case HOST_NOT_FOUND:
		case NO_ADDRESS:
			this->m_Error = ENODATA;
			break;
		case NO_RECOVERY:
			this->m_Error = ENOTRECOVERABLE;
			break;
		default:
			this->m_Error = ENOENT;
			break;
		}
        goto fail;
    }

    /* first to get the host ent*/

    sock = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( sock < 0 )
    {
		this->m_Error = errno;
        goto fail;
    }

    /* set the socket non-blocking*/
    val = fcntl ( sock, F_GETFL, 0 );
    if ( val == -1 )
    {
		this->m_Error = errno;
        goto fail;
    }
    ret = fcntl ( sock, F_SETFL, val | O_NONBLOCK );
    if ( ret < 0 )
    {
		this->m_Error = errno;
        goto fail;
    }

    /* now for the connect*/
    saddr.sin_family = AF_INET;
	if (hent->h_length <= (int)sizeof(saddr.sin_addr))
	{
    	memcpy ( &saddr.sin_addr, hent->h_addr, hent->h_length );
	}
	else
	{
		memcpy ( &saddr.sin_addr, hent->h_addr, sizeof(saddr.sin_addr) );
	}
    saddr.sin_port = htons ( ( u_short ) port );
    saddrlen = sizeof ( saddr );

    ret = connect ( sock, ( struct sockaddr* ) &saddr, saddrlen );
    if ( ret < 0 )
    {
        if ( errno != EINPROGRESS )
        {
			this->m_Error = errno;
            goto fail;
        }
    }
    else
    {
        /* all is success*/
        goto success;
    }

    start = time ( NULL );
    while ( 1 )
    {
        curr = time ( NULL );
        lefttime = this->m_Timeout - ( ( int ) ( curr - start ) );
        if ( this->m_Timeout && lefttime <= 0 )
        {
			this->m_Error = ETIMEDOUT;
            goto fail;
        }

        FD_ZERO ( &wset );
        FD_SET ( sock, &wset );
        tmout.tv_sec = lefttime;
        tmout.tv_usec = 0;
        ret = select ( sock + 1, NULL, &wset, NULL, this->m_Timeout ? &tmout : NULL );
        if ( ret < 0 )
        {
            if ( errno == EINTR )
            {
                /* just for interrupt*/
                continue;
            }
			this->m_Error = errno;
            goto fail;
        }
        else if ( ret == 0 )
        {
			this->m_Error = ETIMEDOUT;
            goto fail;
        }
        vallen = sizeof ( val );
        ret = getsockopt ( sock, SOL_SOCKET, SO_ERROR, ( char* ) &val, ( socklen_t* ) &vallen );
        if ( ret < 0 )
        {
			this->m_Error = errno;
            goto fail;
        }

        if ( val )
        {
            /* has value ,failed*/
			this->m_Error = val;
            goto fail;
        }
        /* all is success*/
        break;
    }

success:
    this->m_Sock = sock;
    return true;

fail:
    if ( sock >= 0 )
    {
        close ( sock );
    }
    sock = -1;
    return false;
}

BOOL CTimeSocket::Bind ( int port, int reuse )
{
    int sock = -1;
    int ret;
    struct sockaddr_in saddr;
    int saddrlen = 0;
    int val = 1;

    if ( this->m_Sock != -1 )
    {
		this->m_Error = EINVAL;
        return false;
    }
    sock = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( sock < 0 )
    {
		this->m_Error = errno;
        goto fail;
    }

    /* to set reuse*/
    if ( reuse )
    {
        val = 1;
    }
    else
    {
        val = 0;
    }


    ret = setsockopt ( sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &val, sizeof ( val ) );
    if ( ret < 0 )
    {
		this->m_Error = errno;
        goto fail;
    }

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons ( ( u_short ) port );
    saddrlen = sizeof ( saddr );

    ret = bind ( sock, ( struct sockaddr* ) &saddr, saddrlen );
    if ( ret < 0 )
    {
		this->m_Error = errno;
        goto fail;
    }

    ret = listen ( sock, 10 );
    if ( ret < 0 )
    {
		this->m_Error = errno;
        goto fail;
    }
    this->m_Sock = sock;

    return true;


fail:
    if ( sock >= 0 )
    {
        close ( sock );
    }
    sock = -1;

    return false;
}

BOOL CTimeSocket::Close()
{
	DEBUG_PRINT("close socket %d\n",this->m_Sock);
    if ( this->m_Sock != -1 )
    {
        close ( this->m_Sock );
    }
    this->m_Sock = -1;
    return true;
}

BOOL CTimeSocket::Write ( void * pBuf, int len )
{
    fd_set wset;
    time_t start, curr;
    struct timeval tmout;
    int ret;
    int curlen = 0;
    char *curptr = ( char* ) pBuf;
    int sock;
	int tryagains=0;
	int errval;
	socklen_t slen;

    if ( this->m_Sock < 0 )
    {
		this->m_Error = EINVAL;
        return false;
    }
    sock = this->m_Sock;

    start = time ( NULL );


    while ( curlen < len )
    {
        curr = time ( NULL );
        tmout.tv_sec = this->m_Timeout - ( ( int ) ( curr - start ) );
        tmout.tv_usec = 0;
        FD_ZERO ( &wset );
        FD_SET ( sock, &wset );

        /*expired*/
        if ( this->m_Timeout && tmout.tv_sec <= 0 )
        {
			this->m_Error = ETIMEDOUT;
            /* time out*/
            goto fail;
        }

		errno = 0;
        ret = select ( sock + 1, NULL, &wset, NULL, this->m_Timeout ? &tmout : NULL );
        if ( ret < 0 )
        {
            /* interrupt*/
            if ( (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)&& tryagains < 3)
            {
				tryagains ++;
                continue;
            }
			this->m_Error = errno;
            goto fail;
        }
        else if ( ret == 0 )
        {
            /* time out */
			this->m_Error = ETIMEDOUT;
            goto fail;
        }

        ret = write ( sock, curptr, ( len - curlen ));
        if ( ret < 0 )
        {
            /* interrupt*/
            if ( (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)&& tryagains < 3)
            {
				tryagains ++;
                continue;
            }
			this->m_Error = errno;
            goto fail;
        }

        curlen += ret;
        curptr += ret;
    }

	slen = sizeof(errval);
	errno = 0;
	errval = 0;
	ret = getsockopt(this->m_Sock,SOL_SOCKET,SO_ERROR,&errval,&slen);
	if (ret < 0 || errval)
	{
		/*this means failed*/
		if (errval)
		{
			this->m_Error = errval;
		}
		else
		{
			this->m_Error = errno;
		}
		ERROR_PRINT("send error %d\n",this->m_Error);
		goto fail;
	}

    /* all is success*/
    return true;

fail:
    return false;

}


BOOL CTimeSocket::Read ( void * pBuf, int len )
{
    int ret;
    char *curptr = ( char* ) pBuf;
    int curlen = 0;
    fd_set rset;
    time_t start, curr;
    struct timeval tmout;
    int sock ;
    int timeout;
	int tryagains = 0;
    if ( this->m_Sock < 0 )
    {
		this->m_Error = EINVAL;
        return false;
    }
    sock = this->m_Sock;
    timeout = this->m_Timeout;

    start = time ( NULL );
    while ( curlen < len )
    {
        curr = time ( NULL );
        tmout.tv_sec = timeout - ( ( int ) ( curr - start ) );
        tmout.tv_usec = 0;

        /* expired */
        if ( timeout && tmout.tv_sec <= 0 )
        {
			this->m_Error = ETIMEDOUT;
            goto fail;
        }

        FD_ZERO ( &rset );
        FD_SET ( sock, &rset );

		errno = 0;
        ret = select ( sock + 1, &rset, NULL, NULL, timeout ? &tmout : NULL );
        if ( ret < 0 )
        {
            if ( errno == EINTR && tryagains < 3)
            {
				tryagains ++;
                continue;
            }
			this->m_Error = errno;
            goto fail;
        }
        else if ( ret == 0 )
        {
			this->m_Error = ETIMEDOUT;
            goto fail;
        }

        ret = read ( sock, curptr, ( len - curlen ) );
        if ( ret < 0 )
        {
            if ( errno == EINTR )
            {
                continue;
            }
			this->m_Error = errno;
            goto fail;
        }
        else if ( ret == 0 )
        {
            /* peer socket closed*/
			this->m_Error = EPIPE;
            goto fail;
        }

        curptr += ret;
        curlen += ret;

    }

    /* all is success*/
    return true;
fail:
    return false;
}

#endif	/*__TIMEOUT_SOCKET_CPP__*/

