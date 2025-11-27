/*
 * typedefs.h
 *
 * Type definitions for LZX
 */
#ifndef _TYPEDEFS_H

#define _TYPEDEFS_H

#if defined(__GNUC__) && !defined(__MINGW32__)
#include <stdint.h>
#include <sys/types.h>
#else
#include <basetsd.h>
#endif

/*
 * Definitions for LZX
 */
typedef unsigned char	byte;
typedef unsigned short	ushort;
typedef unsigned long	ulong;
typedef unsigned int    uint;

#define bool int
#define false 0
#define true 1

/*
 * Definitions for Diamond/CAB memory allocation
 */
typedef unsigned char   BYTE;
typedef unsigned short	USHORT;
typedef unsigned long	ULONG;
typedef unsigned int    UINT;

#if defined(__GNUC__) && !defined(__MINGW32__)
typedef int *           INT_PTR;
typedef unsigned int *  UINT_PTR;
#endif

#ifdef BIT16

//** 16-bit build
#ifndef HUGE
#   define HUGE huge
#endif

#ifndef FAR
#   define FAR far
#endif

#ifndef NEAR
#   define NEAR near
#endif

#else // !BIT16

//** Define away for 32-bit (NT/Chicago) build
#ifndef HUGE
#	define HUGE
#endif

#ifndef FAR
#	define FAR
#endif

#ifndef NEAR
#   define NEAR
#endif

#endif // !BIT16

#ifndef DIAMONDAPI
#   if defined(__GNUC__) && !defined(__MINGW32__)
#       define DIAMONDAPI
#   else
#       define DIAMONDAPI __cdecl
#   endif
#endif

typedef void HUGE * (FAR DIAMONDAPI *PFNALLOC)(UINT cb); /* pfna */
#define FNALLOC(fn) void HUGE * FAR DIAMONDAPI fn(UINT cb)

typedef void (FAR DIAMONDAPI *PFNFREE)(void HUGE *pv); /* pfnf */
#define FNFREE(fn) void FAR DIAMONDAPI fn(void HUGE *pv)

typedef INT_PTR (FAR DIAMONDAPI *PFNOPEN) (char FAR *pszFile,int oflag,int pmode);
typedef UINT (FAR DIAMONDAPI *PFNREAD) (INT_PTR hf, void FAR *pv, UINT cb);
typedef UINT (FAR DIAMONDAPI *PFNWRITE)(INT_PTR hf, void FAR *pv, UINT cb);
typedef int  (FAR DIAMONDAPI *PFNCLOSE)(INT_PTR hf);
typedef int (FAR DIAMONDAPI *PFNSEEK) (INT_PTR hf, int dist, int seektype);

#endif /* _TYPEDEFS_H */
