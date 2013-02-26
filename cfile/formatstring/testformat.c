#include "format_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include "testformat_def.h"
#define INT_VALUE        1
#define LONG_VALUE       2
#define U_LONG_VALUE     3
#define U_INT_VALUE      4
#define STR_VALUE        5
#define CHAR_VALUE       6
#define U_CHAR_VALUE     7
#define LL_VALUE         8
#define U_LL_VALUE       9
#define DOUBLE_VALUE     10
#define FLOAT_VALUE      11

#define N_VALUE          0xff



typedef union
{
	int intv;
	long longv;
	unsigned long ulongv;
	unsigned int uintv;
	char* strv;
	char charv;
	unsigned char ucharv;
	long long llv;
	unsigned long long ullv;
	double fv;
	double dv;
} uv_t;



typedef struct __args_v
{
	int type;
	uv_t u;
}args_v_t;

typedef struct __test_args_v
{
	const char* fmt;
	const char* result;
	int numargs;
	args_v_t *args;
}test_args_v_t;


void free_args(test_args_v_t *pArgs)
{
	if (pArgs->args)
	{
		free(pArgs->args);
	}
	memset(pArgs,0,sizeof(*pArgs));
	return ;
}

#include "testformat_func.c"


static void Usage(int exitcode,const char* msg,...)
{
	FILE* fp=stderr;
	va_list ap;

	if (exitcode == 0)
	{
		fp = stdout;
	}

	if (msg)
	{
		va_start(ap,msg);
		vfprintf(fp,msg,ap);
		fprintf(fp,"\n");
	}

	fprintf(fp,"testformat [OPTIONS]\n");
	fprintf(fp,"\t-h|--help                        : to display this help information\n");
	fprintf(fp,"\t-f|--format fmtstr               : the format string\n");
	fprintf(fp,"\t-r|--result resultvalue          : result string to compare\n");
	fprintf(fp,"\t-i|--int  intvalue               : to format \n");
	fprintf(fp,"\t-l|--long longvalue              : the long value\n");
	fprintf(fp,"\t-ul|--ulong ulongvalue           : the unsigned long value\n");
	fprintf(fp,"\t-c|--char charvalue              : the char value\n");
	fprintf(fp,"\t-uc|--uchar ucharvalue           : the unsigned char value\n");
	fprintf(fp,"\t-ll|--longlong longlongvalue     : to long long value\n");
	fprintf(fp,"\t-ull| --ulonglong ulonglongvalue : unsigned long long value\n");
	fprintf(fp,"\t-s|--string stringvalue          : string value\n");
	fprintf(fp,"\t-F|--float floatvalue            : float value\n");
	fprintf(fp,"\t-d|--double doubldvalue          : double value\n");
	fprintf(fp,"\toutput string current max value is %d\n",MAX_FORMAT_ARGS);
	fprintf(fp,"\t\n");

	exit(exitcode);	
}

int check_for_args(test_args_v_t *args)
{
	if (args->fmt == NULL)
	{
		return 0;
	}
	if (args->result == NULL)
	{
		return 0;
	}

	return 1;
}

static double ParseFloat(const char* str)
{
	double ret = 0.0;
	char* pCur=(char*)str;
	int dotted=0;
	double base=0.1;
	

	while(1)
	{
		if (isdigit(*pCur))
		{
			if (dotted == 0)
			{
				ret *= 10;
				ret += (*pCur - '0');
			}
			else
			{
				int bnum = *pCur - '0';
				double f = (base*bnum);
				ret += f;
				/*we should add base to 1/10*/
				base *= 0.1;
			}
		}
		else if (*pCur == '.' && dotted == 0)
		{
			dotted = 1;
		}
		else
		{
			break;
		}

		pCur ++ ;
	}
	return ret;
}

static double ParseDouble(const char* str)
{
	double ret = 0.0;
	char* pCur=(char*)str;
	int dotted=0;
	double base=0.1;

	while(1)
	{
		if (isdigit(*pCur))
		{
			if (dotted == 0)
			{
				ret *= 10;
				ret += (*pCur - '0');
			}
			else
			{
				int bnum = *pCur - '0';
				ret += (base * bnum);
				/*we should add base to 1/10*/
				base *= 0.1;
			}
		}
		else if (*pCur == '.' && dotted == 0)
		{
			dotted = 1;
		}
		else
		{
			break;
		}

		pCur ++ ;
	}
	return ret;
}

static long long ParseLongLong(const char* str)
{
	long long ret=0;
	char* pCur = (char*)str;
	int minus = 0;
	if (*pCur == '-')
	{
		minus = 1;
		pCur ++;
	}
	while(isdigit(*pCur))
	{
		ret *= 10;
		ret += *pCur - '0';
		pCur ++;
	}


	return minus ? -ret : ret;
}

static unsigned long long ParseULongLong(const char* str)
{
	unsigned long long ret=0;
	char* pCur =(char*) str;
	while(isdigit(*pCur))
	{
		ret *= 10;
		ret += *pCur - '0';
		pCur ++;
	}
	return  ret;
}


static int add_args(test_args_v_t* args,const char* str,int type)
{
	args_v_t *pnewArgs=NULL;
	int numargs=0;
	char* pend;
	int ret;
	numargs = args->numargs;
	numargs ++;
	pnewArgs = (args_v_t*)calloc(numargs,sizeof(*pnewArgs));
	if (pnewArgs== NULL)
	{
		return -ENOMEM;
	}
	if (args->args)
	{
		memcpy(pnewArgs,args->args,args->numargs*sizeof(*pnewArgs));
	}

	switch(type)
	{
		case INT_VALUE:
			pnewArgs[numargs-1].type = INT_VALUE;
			pnewArgs[numargs-1].u.intv = atoi(str);
			fprintf(stderr,"INT_VALUE %s (%d)\n",str,pnewArgs[numargs-1].u.intv);
			break;
		case LONG_VALUE:
			pnewArgs[numargs-1].type = LONG_VALUE;
			pnewArgs[numargs-1].u.longv = strtol(str,&pend,10);
			fprintf(stderr,"LONG_VALUE %s (%ld)\n",str,pnewArgs[numargs-1].u.longv);
			break;
		case U_LONG_VALUE:
			fprintf(stderr,"U_LONG_VALUE %s\n",str);
			pnewArgs[numargs-1].type = U_LONG_VALUE;
			pnewArgs[numargs-1].u.ulongv = strtoul(str,&pend,10);
			break;
		case U_INT_VALUE:
			fprintf(stderr,"U_INT_VALUE %s\n",str);
			pnewArgs[numargs-1].type = U_INT_VALUE;
			pnewArgs[numargs-1].u.uintv = atoi(str);			
			break;
		case STR_VALUE:
			fprintf(stderr,"STR_VALUE %s\n",str);
			pnewArgs[numargs-1].type = STR_VALUE;
			pnewArgs[numargs-1].u.strv = (char*)str;
			break;
		case CHAR_VALUE:
			fprintf(stderr,"CHAR_VALUE %s\n",str);
			pnewArgs[numargs-1].type = CHAR_VALUE;
			pnewArgs[numargs-1].u.charv = str[0];
			break;
		case U_CHAR_VALUE:
			fprintf(stderr,"U_CHAR_VALUE %s\n",str);
			pnewArgs[numargs-1].type = U_CHAR_VALUE;
			pnewArgs[numargs-1].u.charv = (unsigned char)str[0];			
			break;
		case LL_VALUE:
			pnewArgs[numargs-1].type = LL_VALUE;
			pnewArgs[numargs-1].u.llv = ParseLongLong(str);
			fprintf(stderr,"LL_VALUE %s (%lld)\n",str,pnewArgs[numargs-1].u.llv);
			break;
		case U_LL_VALUE:
			pnewArgs[numargs-1].type = U_LL_VALUE;
			pnewArgs[numargs-1].u.ullv = ParseULongLong(str);
			fprintf(stderr,"U_LL_VALUE %s (%llu)\n",str,pnewArgs[numargs-1].u.ullv);
			break;
		case DOUBLE_VALUE:
			pnewArgs[numargs-1].type = DOUBLE_VALUE;
			pnewArgs[numargs-1].u.dv = ParseDouble(str);
			fprintf(stderr,"DOUBLE_VALUE %s (%g)\n",str,pnewArgs[numargs-1].u.dv);
			break;
		case FLOAT_VALUE:
			
			pnewArgs[numargs-1].type = FLOAT_VALUE;
			pnewArgs[numargs-1].u.fv = ParseFloat(str);
			fprintf(stderr,"FLOAT_VALUE %s (%f)\n",str,pnewArgs[numargs-1].u.fv);
			break;
		default:
			assert(0!=0);
			break;
	}

	ret = 0;
	if (args->args)
	{
		free(args->args);
	}
	args->args = pnewArgs;
	args->numargs = numargs;

	return ret;
}


#define CHECK_FOR_ARGS(notice) \
do\
{\
	if ((i+1)>=argc)\
	{\
		Usage(3,notice);\
	}\
}while(0)

#define ADD_ARGS(notice,type,errormsg) \
do\
{\
	if ((i+1)>=argc)\
	{\
		Usage(3,notice);\
	}\
	i ++;\
	ret = add_args(args,argv[i],type);\
	if (ret < 0)\
	{\
		Usage(3,errormsg);\
	}\
}while(0)


int parse_param(int argc,char* argv[],test_args_v_t *args)
{
	int i;
	int ret;
	for (i=1;i<argc;i++)
	{
		fprintf(stderr,"argv[%d] %s\n",i,argv[i]);
		if (strcmp(argv[i],"-h")==0||
			strcmp(argv[i],"--help")==0)
		{
			Usage(0,NULL);
		}
		else if (strcmp(argv[i],"-f") == 0 || 
			strcmp(argv[i],"--format")==0)
		{
			CHECK_FOR_ARGS("-f|--format need args");
			i ++;
			args->fmt = argv[i];
		}
		else if (strcmp(argv[i],"-r") == 0 || 
			strcmp(argv[i],"--result")==0)
		{
			CHECK_FOR_ARGS("-r|--result need args");
			i ++;
			args->result = argv[i];
		}
		else if (strcmp(argv[i],"-i") == 0 || 
			strcmp(argv[i],"--int")==0)
		{
			ADD_ARGS("-i|--int need args",INT_VALUE,"parse -i|--int error");
		}
		else if (strcmp(argv[i],"-l") == 0 || 
			strcmp(argv[i],"--long")==0)
		{
			ADD_ARGS("-l|--long need args",LONG_VALUE,"parse -l|--long error");
		}
		else if (strcmp(argv[i],"-ul") == 0 || 
			strcmp(argv[i],"--ulong")==0)
		{
			ADD_ARGS("-ul|--ulong need args",U_LONG_VALUE,"parse -ul|--ulong error");
		}
		else if (strcmp(argv[i],"-s") == 0 || 
			strcmp(argv[i],"--string")==0)
		{
			ADD_ARGS("-s|--string need args",STR_VALUE,"parse -s|--string error");
		}
		else if (strcmp(argv[i],"-c") == 0 || 
			strcmp(argv[i],"--char")==0)
		{
			ADD_ARGS("-c|--char need args",CHAR_VALUE,"parse -c|--char error");
		}
		else if (strcmp(argv[i],"-uc") == 0 || 
			strcmp(argv[i],"--uchar")==0)
		{
			ADD_ARGS("-uc|--uchar need args",U_CHAR_VALUE,"parse -uc|--uchar error");
		}
		else if (strcmp(argv[i],"-ll") == 0 || 
			strcmp(argv[i],"--longlong")==0)
		{
			ADD_ARGS("-ll|--longlong need args",LL_VALUE,"parse -ll|--longlong error");
		}
		else if (strcmp(argv[i],"-ull") == 0 || 
			strcmp(argv[i],"--ulonglong")==0)
		{
			ADD_ARGS("-ull|--ulonglong need args",U_LL_VALUE,"parse -ull|--ulonglong error");
		}
		else if (strcmp(argv[i],"-d") == 0 || 
			strcmp(argv[i],"--double")==0)
		{
			ADD_ARGS("-d|--double need args",DOUBLE_VALUE,"parse -d|--double error");
		}
		else if (strcmp(argv[i],"-F") == 0 || 
			strcmp(argv[i],"--float")==0)
		{
			ADD_ARGS("-F|--float need args",FLOAT_VALUE,"parse -F|--float error");
		}
		else 
		{
			Usage(3,"not reconize args %s",argv[i]);
		}
	}

	if (check_for_args(args)==0)
	{
		Usage(3,NULL);
	}
	return 0;
}





int main(int argc,char* argv[])
{
	test_args_v_t args;
	int ret;
	int i;
	memset(&args,0,sizeof(args));
	ret = parse_param(argc,argv,&args);
	if (ret < 0)
	{
		fprintf(stderr,"format ");
		for (i=1;i<argc;i++)
		{
			fprintf(stderr," (%s)",argv[i]);
		}
		fprintf(stderr,"\nerror %d\n",ret);
		return ret;
	}
	ret = FactoryFunc(&args);
	if (ret == 0)
	{
		fprintf(stderr,"format ");
		for (i=1;i<argc;i++)
		{
			fprintf(stderr," (%s)",argv[i]);
		}
		fprintf(stderr,"\nsuccess\n");		
	}
	else
	{
		fprintf(stderr,"format ");
		for (i=1;i<argc;i++)
		{
			fprintf(stderr," (%s)",argv[i]);
		}
		fprintf(stderr,"\nerror %d\n",ret);
	}
	free_args(&args);
	return ret;
}
