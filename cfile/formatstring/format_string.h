/*************************************
*
*   @file : format_string
*           to make a buffer free of format string
*
*   @author : jeppeter
*           if you have any question or suggestion of this file
*           please send e-mail to jeppeter@gmail.com
*
*   @history :
*           Jan 31st 2013 created this file
*
*************************************/

#ifndef __FORMAT_STRING_H__
#define __FORMAT_STRING_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*************************************
*
*  @function : format_string
*           to format string in the buffer ,
*
*  @params:
*              ppBuf the buffer contains the format string
*                        this member will be modify when the *pLen is not set
*              pLen  the length of the buffer
*
*              fmt is the format string like printf
*
*  @return value:
*              number of bytes not including the '\0' trial bytes in the *ppBuf if success
*              otherwise negative error code
*
*  @notice :
*             this will free *ppBuf when it replace ,so make sure it is NULL or the pointer to the memory allocated by malloc
*************************************/
int format_string(char**ppBuf,int *pLen,const char* fmt,...);
/*************************************
*
*  @function : append_string
*           to format string in the buffer and append to origin string
*
*  @params:
*              ppBuf the buffer contains the format string
*                        this member will be modify when the *pLen is not set
*              pLen  the length of the buffer
*
*              fmt is the format string like printf
*
*  @return value:
*              number of bytes not including the '\0' trial bytes in the *ppBuf if success
*              otherwise negative error code
*
*  @notice :
*             this will free *ppBuf when it replace ,so make sure it is NULL or the pointer to the memory allocated by malloc
*************************************/
int append_string(char** ppBuf,int *pLen,const char* fmt,...);

/*this is the function of inner for format_string*/
int vformat_string(char**ppBuf,int* pLen,const char* fmt,va_list ap);
/*this is the function of inner for append_string*/
int vappend_string(char**ppBuf,int* pLen,const char* fmt,va_list ap);


#ifdef __cplusplus
};
#endif
#endif /*__FORMAT_STRING_H__*/
