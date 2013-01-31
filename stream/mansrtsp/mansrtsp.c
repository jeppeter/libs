#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include "mansrtsp.h"

#define STEP(ptr) \
do\
{\
	ptr ++;\
}while(0)

#define STEP_N(ptr,n) \
do\
{\
	int __i=0;\
	while(__i < (n))\
	{\
		STEP(ptr);\
		__i ++;\
	}\
}while(0)

#define MARK_END_AND_STEP(ptr) \
do\
{\
	*(ptr) = '\0';\
	STEP(ptr);\
}while(0)

#define MAKE_SURE_PTR(ptr,c) \
do\
{\
	if ((*(ptr)) != c)\
	{\
		return NULL;\
	}\
}while(0)

#define MAKE_SURE_STR(ptr,str,n) \
do\
{\
	if (strncasecmp((ptr),(str),(n)) != 0)\
	{\
		return NULL;\
	}\
}while(0)

static char* load_action(char* p,char** action,char**version)
{
	char* pc=p,*pe;
	char* pa=NULL,*pv=NULL;
	char c;

	pe = strchr(pc,'\r');
	if (pe == NULL)
	{
		pe = strchr(pc,'\n');
	}

	if (pe == NULL)
	{
		pe = pc + strlen(pc);
	}
	else
	{
		while(*pe == '\r' ||
			*pe == '\n')
		{
			*pe = '\0';
			pe ++;
		}
		
	}

	pa=pc;
	while( 1 )
	{
		c = *pc;
		if ( (c >= 'A' && c <= 'Z')
			|| (c >= 'a' && c <= 'z'))
		{			
			pc ++;
			continue;
		}		
		break;
	}

	MAKE_SURE_PTR(pc,' ');
	MARK_END_AND_STEP(pc);
	/*now to make*/
	MAKE_SURE_STR(pc,"MANSRTSP/",9);
	STEP_N(pc,9);
	MAKE_SURE_STR(pc,"1.0",3);
	pv = pc;
	STEP_N(pc,3);
	MAKE_SURE_PTR(pc,'\0');

	*action = pa;
	*version = pv;

	return pe;	
}

static char* load_value_key(char* p,char**key,char**value)
{
	char *pc=p;
	char *pe,*pf;
	char *pk=NULL,*pv=NULL;

	pe = strchr(pc,'\r');
	if (pe == NULL)
	{
		pe = strchr(pc,'\n');
	}

	if (pe == NULL)
	{
		pe = pc + strlen(pc);
	}
	else
	{
		while(*pe == '\r' ||
			*pe == '\n')
		{
			*pe = '\0';
			pe ++;
		}
	}

	pf = strchr(pc,':');
	if (pf == NULL)
	{
		return NULL;
	}

	pk = pc;
	pc = pf;
	MARK_END_AND_STEP(pc);
	pv = pc;

	*key = pk;
	*value = pv;
	return pe;		
}

static char* split_value(char* p,char* fmt,...)
{
	char* pc,*pe,*fc,*pp;
	va_list va;
	char** s;

	pc = p;
	pe = pc + strlen(pc);
	fc = fmt;
	va_start(va,fmt);

	while(1)
	{
		if (*fc == '\0')
		{
			break;
		}

		switch(*fc)
		{
			case 's':
				s = va_arg(va,char**);
				*s = pc;
				break;
			default:
				pp = strchr(pc,*fc);
				if (pp)
				{
					pc = pp+1;
					*pp = '\0';
				}
				else
				{
					return NULL;
				}
				break;
		}

		fc ++;
	}
	return pe;
}


static int mans_range_parse(struct mans_rtsp* pMans,char* p)
{
	char* pnpt,*pstart,*pend;
	char* pret;

	/*the range format is like npt=0- this kind*/
	pret = split_value(p,"s=s-s",&pnpt,&pstart,&pend);
	if (pret == NULL)
	{
		return -1;
	}

	if (strcasecmp(pnpt,"npt")!=0)
	{
		return -1;
	}

	pMans->start_time = atof(pstart);
	pMans->stop_time = atof(pend);

	return 0;
}

static int check_mans(struct mans_rtsp* pMans)
{
	if (pMans->action == NULL)
	{
		return 0;
	}

	if (pMans->version == NULL)
	{
		return 0;
	}

	if (pMans->seq == 0)
	{
		return 0;
	}

	return 1;
}
	

struct mans_rtsp* mans_parse(char* line)
{
	struct mans_rtsp* pMans=NULL;
	char* pk,*pv,*pa,*pe;
	char* p;
	int ret;

	pMans = (struct mans_rtsp*)calloc(1,sizeof(*pMans));
	if (pMans == NULL)
	{
		goto fail;
	}

	pMans->_payload = strdup(line);
	if (pMans->_payload == NULL)
	{
		goto fail;
	}

	pMans->scale = 1.0;

	p = pMans->_payload;
	p = load_action(p,&pa,&pv);
	if (p == NULL)
	{
		goto fail;
	}

	/*now to give the action*/
	pMans->action = pa;
	pMans->version = pv;

	while(1)
	{
		p = load_value_key(p,&pk,&pv);
		if (p == NULL)
		{
			break;
		}

		if (strcasecmp(pk,"cseq")==0)
		{
			pMans->seq = strtoull(pv,&pe,10);
		}
		else if (strcasecmp(pk,"scale")==0)
		{
			pMans->scale = atof(pv);
		}
		else if (strcasecmp(pk,"range")==0)
		{
			ret = mans_range_parse(pMans,pv);
			if (ret < 0)
			{
				goto fail;
			}
		}
		else
		{
			goto fail;
		}
	}

	ret = check_mans(pMans);
	if (ret == 0)
	{
		goto fail;
	}
	

	return pMans;
fail:
	mans_destroy(pMans);
	return NULL;
}

#define BUFFER_SNPRINTF(...) \
do\
{\
	ret = snprintf(pCurPtr,leftlen,__VA_ARGS__);\
	if (ret < 0)\
	{\
		return -2;\
	}\
	if (ret == leftlen)\
	{\
		return -1;\
	}\
	retsize += ret;\
	pCurPtr += ret;\
	leftlen -= ret;\
}while(0)


static int __dump_mans_rtsp(struct mans_rtsp* pMans,char* pBuf,int len)
{
	char* pCurPtr = pBuf;
	int leftlen = len;
	int retsize = 0;
	int ret;

	BUFFER_SNPRINTF("%s MANSRTSP/%s\n",pMans->action,pMans->version);
	BUFFER_SNPRINTF("CSeq:%lld\n",pMans->seq);
	BUFFER_SNPRINTF("Scale:%.1f\n",pMans->scale);
	if (pMans->stop_time != 0.0)
	{
		BUFFER_SNPRINTF("Range:npt=%.1f-%.1f\n",pMans->start_time,pMans->stop_time);
	}
	else 
	{
		/*stop_time == 0.0*/
		BUFFER_SNPRINTF("Range:npt=%.1f-\n",pMans->start_time);
	}
	return retsize;
}

int dump_mans_rtsp(struct mans_rtsp* pMans,char** ppBuf,int *pLen)
{
	char* pRetBuf=NULL;
	int retbuflen=0;
	int retsize;
	int setsize;
	int getsize;
	int ret;

	ret = check_mans(pMans);
	if (ret == 0)
	{
		ret = -EINVAL;
		goto fail;
	}

	pRetBuf = *ppBuf;
	retbuflen = *pLen;

	if (pRetBuf==NULL||
		retbuflen == 0)
	{
		if (retbuflen == 0)
		{
			retbuflen = 512;
		}
		pRetBuf = (char*)calloc(1,retbuflen);
		if (pRetBuf==NULL)
		{
			ret = -ENOMEM;
			goto fail;
		}
	}

	do
	{
		setsize = retbuflen;
		getsize = __dump_mans_rtsp( pMans,pRetBuf,retbuflen);
		if (getsize == -1)
		{
			retbuflen <<=1;
			if (pRetBuf && pRetBuf != *ppBuf)
			{
				free(pRetBuf);
			}
			pRetBuf = (char*)calloc(1,retbuflen);
			if (pRetBuf == NULL)
			{
				ret = -ENOMEM;
				goto fail;
			}
		}
	}while(getsize == -1);

	if (getsize < 0)
	{
		ret = -errno ? -errno: -EIO;
		goto fail;
	}

	retsize = getsize;
	*pLen = retbuflen;
	*ppBuf = pRetBuf;

	return retsize;

fail:
	if (pRetBuf && pRetBuf != *ppBuf)
	{
		free(pRetBuf);
	}
	pRetBuf = NULL;
	retbuflen = 0;
	assert(ret < 0);
	return ret;	
}


void mans_destroy(struct mans_rtsp* pMans)
{
	if (pMans == NULL)
	{
		return ;
	}
	if (pMans->_payload)
	{
		free(pMans->_payload);
	}
	pMans->_payload = NULL;
	free(pMans);
	return ;
}


