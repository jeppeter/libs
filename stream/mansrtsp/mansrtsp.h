/******************************************
*
*   this file is for MANSRTSP protocol  for GB28181 
*
*   author : jeppeter@gmail.com  if you have some suggestion or questions about this
*                please send an e-mail to me
*
*   history :
*            Jan 30th 2013 created
*
*  
******************************************/


#ifndef __MANSRTSP_H__
#define __MANSRTSP_H__

struct mans_rtsp
{
	char* _payload;
	char* action;
	int version;
	float start_time;
	float stop_time;
	long long int seq;
	float scale;
};

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/


struct mans_rtsp* mans_parse(char* line);
int dump_mans_rtsp(struct mans_rtsp* pMans,char** ppBuf,int *pLen);
void mans_destroy(struct mans_rtsp* pMans);


#ifdef __cplusplus
};
#endif /*__cplusplus*/


#endif /*__MANSRTSP_H__*/
