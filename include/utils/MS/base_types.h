#pragma once

struct HWND__;

namespace utils::MS
	{
	using DWORD    =  unsigned long ;
	using BOOL     =  int           ;
	using BYTE     =  unsigned char ;
	using WORD     =  unsigned short;
	using FLOAT    =  float         ;
	using PFLOAT   =  FLOAT        *;
	using PBOOL    =  BOOL         *;
	using LPBOOL   =  BOOL         *;
	using PBYTE    =  BYTE         *;
	using LPBYTE   =  BYTE         *;
	using PINT     =  int          *;
	using LPINT    =  int          *;
	using PWORD    =  WORD         *;
	using LPWORD   =  WORD         *;
	using LPLONG   =  long         *;
	using PDWORD   =  DWORD        *;
	using LPDWORD  =  DWORD        *;
	using LPVOID   =  void         *;
	using LPCVOID  =  const void   *;

	using INT      = int            ;
	using UINT     = unsigned int   ;
	using PUINT    = unsigned int  *;


	using INT_PTR    =          __int64 ;
	using PINT_PTR   =          __int64*;
	using UINT_PTR   = unsigned __int64 ;
	using PUINT_PTR  = unsigned __int64*;

	using LONG_PTR   =          __int64 ;
	using PLONG_PTR  =          __int64*;
	using ULONG_PTR  = unsigned __int64 ;
	using PULONG_PTR = unsigned __int64*;

	using WPARAM  = UINT_PTR;
	using LPARAM  = LONG_PTR;
	using LRESULT = LONG_PTR;

	using HWND = HWND__*;
	}