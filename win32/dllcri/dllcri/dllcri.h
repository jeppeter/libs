
#ifndef __DLL_CRI_H__
#define __DLL_CRI_H__


#ifdef DLLCRI_API
#define DLLCRI_API extern "C" _declspec(dllimport) 
#else
#define DLLCRI_API extern "C" _declspec(dllexport) 
#endif

DLLCRI_API int SnapWholePicture(const char* pFileName);

#endif  /*__DLL_CRI_H__*/
