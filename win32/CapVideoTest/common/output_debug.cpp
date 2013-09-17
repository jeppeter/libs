
#include "output_debug.h"



extern "C" void InnerDebug(char* pFmtStr)
{
#ifdef UNICODE
    LPWSTR pWide=NULL;
    int len;
    BOOL bret;
    len = (int) strlen(pFmtStr);
    pWide = new wchar_t[len*2];
    bret = MultiByteToWideChar(CP_ACP,NULL,pFmtStr,-1,pWide,len*2);
    if (bret)
    {
        OutputDebugString(pWide);
    }
    else
    {
        OutputDebugString(L"can not change fmt string");
    }
    delete [] pWide;
#else
    OutputDebugString(pFmtStr);
#endif
    return ;
}

extern "C" void DebugOutString(const char* file,const char* func,int lineno,const char* fmt,...)
{
    char* pFmt=NULL;
    char* pLine=NULL;
    char* pWhole=NULL;
    va_list ap;

    pFmt = new char[2000];
    pLine = new char[2000];
    pWhole = new char[4000];

    _snprintf_s(pLine,2000,1999,"%s:AAA%sAAA:%d\t",file,func,lineno);
    va_start(ap,fmt);
    _vsnprintf_s(pFmt,2000,1999,fmt,ap);
    strcpy_s(pWhole,4000,pLine);
    strcat_s(pWhole,4000,pFmt);

    InnerDebug(pWhole);
    delete [] pFmt;
    delete [] pLine;
    delete [] pWhole;

    return ;
}

extern "C" void DebugBuffer(const char* file,const char* func,int lineno,unsigned char* pBuffer,int buflen)
{
    int fmtlen=2000;
    char*pLine=NULL,*pCur;
    int formedlen;
    int ret;
    int i;
    pLine = new char[fmtlen];
    pCur = pLine;
    formedlen = 0;

    ret = _snprintf_s(pCur,fmtlen-formedlen,fmtlen-formedlen-1,"[%s:AAA%sAAA:%d]\tbuffer %p (%d)",file,func,lineno,pBuffer,buflen);
    pCur += ret;
    formedlen += ret;

    for (i=0; i<buflen; i++)
    {
        if ((formedlen +100)>fmtlen)
        {
            InnerDebug(pLine);
            pCur = pLine;
            formedlen = 0;
        }
        if ((i%16)==0)
        {
        	ret = _snprintf_s(pCur,fmtlen-formedlen,fmtlen-formedlen-1,"\n");
            InnerDebug(pLine);
            pCur = pLine;
            formedlen = 0;
            ret = _snprintf_s(pCur,fmtlen-formedlen,fmtlen-formedlen-1,"[0x%08x]\t",i);
            pCur += ret;
            formedlen += ret;
        }

        ret = _snprintf_s(pCur,fmtlen-formedlen,fmtlen-formedlen-1,"0x%02x ",pBuffer[i]);
        pCur += ret;
        formedlen += ret;
    }

    if (formedlen > 0)
    {
        InnerDebug(pLine);
        pCur = pLine;
        formedlen = 0;
    }

	delete [] pLine;
	pLine = NULL;
	return ;
}


