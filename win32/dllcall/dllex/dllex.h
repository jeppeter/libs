#pragma once


#ifdef DLLEX_EXPORTS
#define DLLEX_API extern "C" __declspec(dllexport)
#else
#define DLLEX_API extern "C" __declspec(dllimport)
#endif


DLLEX_API int PrintFunc(const char* pName);
DLLEX_API int RepeatFunc(const char* pName);
DLLEX_API int CrashFunc(const char* pName);