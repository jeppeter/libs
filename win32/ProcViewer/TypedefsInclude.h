#ifndef _TYPEDEFS_INCLUDE_H_
#define _TYPEDEFS_INCLUDE_H_

// Typedefs for CStrings and it's iterators
typedef std::vector<CString> StringVector;
typedef std::vector<CString>::iterator SVItr;
typedef const std::vector<CString> StringVectorC;
typedef std::vector<CString>::const_iterator SVCItr;

// Typedefs for not so common functions
typedef BOOL ( WINAPI * TD_IsAppThemed )( VOID );
typedef int  ( WINAPI * TD_StrCmpLogicalW )( LPCWSTR, LPCWSTR );

typedef CMap<CString, LPCTSTR, class CPrivilege, const CPrivilege& > PrivilegeMap;

#endif // _TYPEDEFS_INCLUDE_H_ 