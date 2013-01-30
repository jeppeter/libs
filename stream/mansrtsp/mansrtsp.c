
#include "mansrtsp.h"

#define STEP(ptr)
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
	*ptr = '\0';\
	STEP(ptr);\
}while(0)

#define MAKE_SURE_PTR(ptr,c) \
do\
{\
	if ((*ptr) != c)\
	{\
		return NULL;\
	}\
}while(0)

#define MAKE_SURE_STR(ptr,str,n) \
do\
{\
	if (strncmp((ptr),(str),(n)) != 0)\
	{\
		return NULL;\
	}\
}while(0)

static char* load_action(char* p,char** action,char**version)
{
	char* pc=p,pe;
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
		if ( c >= 'A' && c <= 'Z')
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
	char* pc=p;
	char* pe,pf;
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

static char* split_value(char* p,int sep,char**key,char**value)
{
}


	

struct mans_rtsp* mans_parse(char* line)
{
	struct mans_rtsp* pMans=NULL;
	char* pk,*pv,*pa;
	char* p;

	pMans = (struct mans_rtsp*)calloc(sizeof(*pMans));
	if (pMans == NULL)
	{
		goto fail;
	}

	pMans->_payload = strdup(line);
	if (pMans->_payload)
	{
		goto fail;
	}

	p = load_action(p,&pa,&pv);
	if (p == NULL)
	{
		goto fail;
	}

	/*now to give the action*/
	pMans->action = pa;
	pMans->version = atof(pv);

	while(1)
	{
		p = load_value_key(p,&pk,&pv);
		if (p == NULL)
		{
			break;
		}

		if (strcasecmp(pk,"cseq")==0)
		{
			pMans->seq = atoi(pv);
		}
		else if (strcasecmp(pk,"scale")==0)
		{
			pMans->scale = atof(pv);
		}
		else if (strcasecmp(pk,"range")==0)
		{
			
		}
		else
		{
			goto fail;
		}
	}
	

	return pMans;
fail:
	mans_destroy(pMans);
	return NULL;
}

int dump_mans_rtsp(struct mans_rtsp* pMans,char** ppBuf,int *pLen)
{
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


