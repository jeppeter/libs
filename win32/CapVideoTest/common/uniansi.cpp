

#include "uniansi.h"
#include <Windows.h>
#include <assert.h>

extern "C" int UnicodeToAnsi(wchar_t* pWideChar,char** ppChar,int*pCharSize)
{
	char* pRetChar = *ppChar;
	int retcharsize = *pCharSize;
	int ret,wlen,needlen;

	if (pWideChar == NULL)
	{
		if (*ppChar)
		{
			delete [] pRetChar;
		}
		*ppChar = NULL;
		*pCharSize = 0;
		return 0;
	}
	wlen = wcslen(pWideChar);
	needlen = WideCharToMultiByte(CP_ACP,0,pWideChar,wlen,NULL,0,NULL,NULL);
	if (retcharsize <= needlen)
	{
		retcharsize = (needlen+1);
		pRetChar = new char[needlen + 1];
		assert(pRetChar);
	}

	ret = WideCharToMultiByte(CP_ACP,0,pWideChar,wlen,pRetChar,retcharsize,NULL,NULL);
	if (ret != needlen)
	{
		ret = ERROR_INVALID_BLOCK;
		goto fail;
	}
	pRetChar[needlen] = '\0';

	if ((*ppChar) && (*ppChar) != pRetChar)
	{
		char* pTmpChar = *ppChar;
		delete [] pTmpChar;
	}
	*ppChar = pRetChar;
	*pCharSize = retcharsize;

	return ret;
fail:
	if (pRetChar && pRetChar != (*ppChar))
	{
		delete [] pRetChar;
	}
	pRetChar = NULL;
	retcharsize = 0;
	SetLastError(ret);
	return -ret;
}


extern "C" int AnsiToUnicode(char* pChar,wchar_t **ppWideChar,int*pWideCharSize)
{
	wchar_t *pRetWideChar = *ppWideChar;
	int retwidecharsize = *pWideCharSize;
	int ret,len,needlen;

	if (pChar == NULL)
	{
		if (*ppWideChar)
		{
			delete [] pRetWideChar;
		}
		*ppWideChar = NULL;
		*pWideCharSize = 0;
		return 0;
	}

	len = strlen(pChar);
	needlen = MultiByteToWideChar(CP_ACP,0,pChar,len,NULL,0);
	if (retwidecharsize <= needlen)
	{
		retwidecharsize = needlen + 1;
		pRetWideChar = new wchar_t[retwidecharsize];
		assert(pRetWideChar);
	}

	ret = MultiByteToWideChar(CP_ACP,0,pChar,len,pRetWideChar,retwidecharsize);
	if (ret != needlen)
	{
		ret = ERROR_INVALID_BLOCK;
		goto fail;
	}
	pRetWideChar[needlen] = '\0';

	if ( (*ppWideChar) && (*ppWideChar) != pRetWideChar)
	{
		wchar_t *pTmpWideChar = *ppWideChar;
		delete [] pTmpWideChar;
	}
	*ppWideChar = pRetWideChar;
	*pWideCharSize = retwidecharsize;
	return ret;
fail:
	if (pRetWideChar && pRetWideChar != (*ppWideChar))
	{
		delete [] pRetWideChar;
	}
	pRetWideChar = NULL;
	retwidecharsize = 0;
	SetLastError(ret);
	return -ret;
}