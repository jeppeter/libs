#include "format_string.h"

#define INT_VALUE        1
#define LONG_VALUE       2
#define U_LONG_VALUE     3
#define U_INT_VALUE      4
#define STR_VALUE        5
#define CHAR_VALUE       6
#define U_CHAR_VALUE     7
#define LL_VALUE         8
#define U_LL_VALUE       9

#define N_VALUE          0xff

#include "format_def.h"


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



static args_v_t args1[]=
{
	{.type : U_INT_VALUE ,.u.uintv : 100},
	{.type : STR_VALUE , .u.strv : "world"},
	{.type : N_VALUE   , .u.strv : NULL},
};

static test_args_v_t test1=
{
	.fmt : "hello %d %s",
	.result : "hello 100 world",
	.numargs : 3,
	.args   : args1,
};

#include "test_format_func.c"


static void Usage(int exitcode,const char* msg)
{
	FILE* fp=stderr;

	if (exitcode == 0)
	{
		fp = stdout;
	}

	if (msg)
	{
		fprintf(fp,msg);
		fprintf(fp,"\n");
		fflush(fp);
	}

	fprintf(fp,"testformat [OPTIONS]\n",);
	fprintf(fp,"\t-h|--help                        : to display this help information\n");
	fprintf(fp,"\t-i|--int  intvalue               : to format \n");
	fprintf(fp,"\t-f|--format fmtstr               : the format string\n");
	fprintf(fp,"\t-l|--long longvalue              : the long value\n");
	fprintf(fp,"\t-ul|--ulong ulongvalue           : the unsigned long value\n");
	fprintf(fp,"\t-c|--char charvalue              : the char value\n");
	fprintf(fp,"\t-uc|--uchar ucharvalue           : the unsigned char value\n");
	fprintf(fp,"\t-ll|--longlong longlongvalue     : to long long value\n");
	fprintf(fp,"\t-ull| --ulonglong ulonglongvalue : unsigned long long value\n");
	fprintf(fp,"\t-s|--string stringvalue          : string value\n");
	fprintf(fp,"\t\n");
	fprintf(fp,"\t\n");
	fprintf(fp,"\t\n");
	fprintf(fp,"\t\n");

	exit(exitcode);	
}

int parse_param(int argc,char* argv[],test_args_v_t *args)
{
	
}





int main(int argc,char* argv[])
{
	int i=0;
	while(all_tests[i])
	{
	}
}
