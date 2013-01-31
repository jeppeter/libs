
#include "format.h"

#include <stdlib.h>


int vformat_string(char**ppBuf,int* pLen,const char* fmt,va_list ap)
{
	char* pRetBuf = *ppBuf;
	int retsize = *pLen;
	int getsize,setsize;
	int ret;

	if ( (pRetBuf == NULL && retsize > 0 )||
		(pRetBuf && retsize == 0))
	{
		ret = -EINVAL;
		goto fail;
	}

	if (pRetBuf == NULL)
	{
		retsize = 512;
		pRetBuf = (char*)calloc(1,retsize);
		if (pRetBuf == NULL)
		{
			ret = -ENOMEM;
			goto fail;
		}
	}

	do
	{
		setsize = retsize;
		getsize = vsnprintf(pRetBuf,setsize,fmt,ap);
		if (getsize == setsize)
		{
			retsize <<= 1;
			if (pRetBuf && pRetBuf != *ppBuf)
			{
				free(pRetBuf);
			}
			pRetBuf = (char*)calloc(1,retsize);
			if (pRetBuf == NULL)
			{
				ret = -ENOMEM;
				goto fail;
			}
		}
	}while(getsize == setsize);

	if (getsize < 0)
	{
		ret = -errno ? -errno : -ENOMEM;
		goto fail;
	}
	if (pRetBuf != *ppBuf)
	{
		if (*ppBuf)
		{
			free(*ppBuf);
		}
		*ppBuf = pRetBuf;
	}
	*pLen = retsize;

	return getsize;
fail:
	if (pRetBuf && pRetBuf != *ppBuf)
	{
		free(pRetBuf);
	}
	pRetBuf = NULL;
	retsize = 0;
	assert(ret < 0);
	return ret;
}

int vappend_string(char**ppBuf,int* pLen,const char* fmt,va_list ap)
{
	char* pAppend=NULL;
	int appendsize=0;
	int appendlen=0;
	int ret;
	char *pRetBuf=*ppBuf;
	int totalsize=*pLen;
	int totallen=0;
	int originlen=0;

	if ((totalsize == 0 && pRetBuf)
		|| (totalsize > 0 && pRetBuf == NULL))
	{
		ret = -EINVAL;
		goto fail;
	}

	if (*ppBuf)
	{
		originlen = strlen(*ppBuf);
	}
	appendlen = vformat_string(&pAppend,&appendsize,fmt,ap);
	if (appendlen < 0)
	{
		ret = appendlen;
		goto fail;
	}

	if ((originlen + appendlen) < totalsize)
	{
		totallen = (originlen + appendlen);
		strncat(pRetBuf,pAppend,appendlen + 1);
	}
	else
	{
		/*overflow ,we should allocate a new buffer*/
		totalsize = (originlen + appendlen + 200);
		totallen = originlen + appendlen;
		pRetBuf = (char*)calloc(1,totalsize);
		if (pRetBuf== NULL)
		{
			ret = -ENOMEM;
			goto fail;
		}

		if (originlen > 0)
		{
			strncpy(pRetBuf,*ppBuf,originlen+1);
		}
		strncat(pRetBuf,pAppend,appendlen+1);
	}

	/*free append buffer */
	if (pAppend)
	{
		free(pAppend);
	}
	pAppend = NULL;
	appendsize =appendlen = 0;

	if (pRetBuf != *ppBuf)
	{
		if (*ppBuf)
		{
			free(*ppBuf);
		}
		*ppBuf = pRetBuf;
		*pLen = totalsize;
	}

	return  totallen;
fail:

	if (pRetBuf && pRetBuf != *ppBuf)
	{
		free(pRetBuf);
	}
	pRetBuf = NULL;
	totalsize = 0;
	totallen = 0;
	
	if (pAppend)
	{
		free(pAppend);
	}
	pAppend = NULL;
	appendlen = appendsize = 0;
	return ret;
}


int format_string(char**ppBuf,int *pLen,const char* fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	return vformat_string(ppBuf,pLen,fmt,ap);
}

int append_string(char** ppBuf,int *pLen,const char* fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	return vappend_string(ppBuf,pLen,fmt,ap);
}


