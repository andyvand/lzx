/*
 *  Microsoft Confidential
 *  Copyright (C) Microsoft Corporation 1996
 *  All Rights Reserved.
 *
 *  LCI.H - Diamond Memory Compression Interface (LCI)
 *
 *  History:
 *      03-Jul-1996     jforbes     Created from QCI.H
 *
 *  Functions:
 *      LCICreateCompression    - Create and reset an LCI compression context
 *      LCICompress             - Compress a block of data
 *      LCIResetCompression     - Reset compression context
 *      LCIDestroyCompression   - Destroy LCI compression context
 *      LCISetTranslationSize   - Set file translation size
 *
 *  Types:
 *      LCI_CONTEXT_HANDLE      - Handle to an LCI compression context
 *      PFNALLOC                - Memory allocation function for LCI
 *      PFNFREE                 - Free memory function for LCI
 */

/* --- types -------------------------------------------------------------- */

#ifndef __LCI_H__
#define __LCI_H__

#ifndef DIAMONDAPI
#if defined(__GNUC__) && !defined(__MINGW32__)
#define DIAMONDAPI
#else
#define DIAMONDAPI __cdecl
#endif
#endif

#if defined(_WIN16) || defined(WIN16) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#if defined(DLL_EXPORTS)
#define DLLEXPORT __declspec(dllexport)
#elif defined(LIB_EXPORTS)
#define DLLEXPORT
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#elif defined(__GNUC__)
#define DLLEXPORT __attribute__((visibility("default")))
#else
#define DLLEXPORT
#endif

#ifdef __GNUC__
#define ALIGNED(X) __attribute__((aligned(X)))
#else
#define ALIGNED(X)
#endif

#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef unsigned char  BYTE;
#endif

#ifndef _UINT_DEFINED
#define _UINT_DEFINED
typedef unsigned int  UINT;
typedef unsigned int  uint;
#endif

#ifndef _ULONG_DEFINED
#define _ULONG_DEFINED
typedef unsigned long  ULONG;
#endif

#ifndef FAR
#ifdef BIT16
#define FAR far
#else
#define FAR
#endif
#endif

#ifndef HUGE
#ifdef BIT16
#define HUGE huge
#else
#define HUGE
#endif
#endif

#ifndef _MI_MEMORY_DEFINED
#define _MI_MEMORY_DEFINED
typedef void HUGE *  MI_MEMORY;
#endif

#ifndef _MHANDLE_DEFINED
#define _MHANDLE_DEFINED
#if defined(_WIN64) && !defined(__MINGW32__)
typedef unsigned __int64 MHANDLE;
#elif defined(_WIN64)
typedef unsigned long long MHANDLE;
#else
typedef unsigned long MHANDLE;
#endif
#endif

/* --- LCI-defined types -------------------------------------------------- */

/* LCI_CONTEXT_HANDLE - Handle to an LCI compression context */

typedef MHANDLE LCI_CONTEXT_HANDLE;      /* hmc */


/***    PFNALLOC - Memory allocation function for LCI
 *
 *  Entry:
 *      cb - Size in bytes of memory block to allocate
 *
 *  Exit-Success:
 *      Returns !NULL pointer to memory block
 *
 *  Exit-Failure:
 *      Returns NULL; insufficient memory
 */
#ifndef _PFNALLOC_DEFINED
#define _PFNALLOC_DEFINED
typedef MI_MEMORY (FAR DIAMONDAPI *PFNALLOC)(UINT cb);       /* pfnma */
#endif


/***    PFNFREE - Free memory function for LCI
 *
 *  Entry:
 *      pv - Memory block allocated by matching PFNALLOC function
 *
 *  Exit:
 *      Memory block freed.
 */
#ifndef _PFNFREE_DEFINED
#define _PFNFREE_DEFINED
typedef void (FAR DIAMONDAPI *PFNFREE)(MI_MEMORY pv);          /* pfnmf */
#endif

/* --- prototypes --------------------------------------------------------- */

/***    LCICreateCompression - Create LCI compression context
 *
 *  Entry:
 *      pcbDataBlockMax     *largest uncompressed data block size desired,
 *                          gets largest uncompressed data block allowed
 *      pvConfiguration     passes implementation-specific info to compressor.
 *      pfnma               memory allocation function pointer
 *      pfnmf               memory free function pointer
 *      pcbDstBufferMin     gets required compressed data buffer size
 *      pmchHandle          gets newly-created context's handle
 *
 *  Exit-Success:
 *      Returns MCI_ERROR_NO_ERROR;
 *      *pcbDataBlockMax, *pcbDstBufferMin, *pmchHandle filled in.
 *
 *  Exit-Failure:
 *      MCI_ERROR_NOT_ENOUGH_MEMORY, could not allocate enough memory.
 *      MCI_ERROR_BAD_PARAMETERS, something wrong with parameters.
 */
DLLEXPORT int FAR DIAMONDAPI LCICreateCompression(
        UINT FAR *      pcbDataBlockMax,  /* max uncompressed data block size */
        void FAR *      pvConfiguration,  /* See LZXCONFIGURATION */
        PFNALLOC        pfnma,            /* Memory allocation function ptr */
        PFNFREE         pfnmf,            /* Memory free function ptr */
        UINT FAR *      pcbDstBufferMin,  /* gets required output buffer size */
        LCI_CONTEXT_HANDLE FAR *pmchHandle, /* gets newly-created handle */
		int FAR	(DIAMONDAPI *pfnlzx_output_callback)(
			void *			pfol,
			unsigned char *	compressed_data,
			int			compressed_size,
			int			uncompressed_size
        ),
        void FAR *      fci_pv // not the same as the FCI client's pv
);


/***    LCICompress - Compress a block of data
 *
 *  Entry:
 *      hmc                 handle to compression context
 *      pbSrc               source buffer (uncompressed data)
 *      cbSrc               size of data to be compressed
 *      pbDst               destination buffer (for compressed data)
 *      cbDst               size of destination buffer
 *      pcbResult           receives compressed size of data
 *
 *  Exit-Success:
 *      Returns MCI_ERROR_NO_ERROR;
 *      *pcbResult has size of compressed data in pbDst.
 *      Compression context possibly updated.
 *
 *  Exit-Failure:
 *      MCI_ERROR_BAD_PARAMETERS, something wrong with parameters.
 */
DLLEXPORT int FAR DIAMONDAPI LCICompress(
        LCI_CONTEXT_HANDLE  hmc,         /* compression context */
        void FAR *          pbSrc,       /* source buffer */
        UINT                cbSrc,       /* source buffer size */
        void FAR *          pbDst,       /* target buffer */
        UINT                cbDst,       /* target buffer size */
        UINT FAR *          pcbResult);  /* gets target data size */


/***    LCIResetCompression - Reset compression history (if any)
 *
 *  De-compression can only be started on a block which was compressed
 *  immediately following a LCICreateCompression() or LCIResetCompression()
 *  call.  This function forces such a new "compression boundary" to be
 *  created (only by causing the compressor to ignore history, can the data
 *  output be decompressed without history.)
 *
 *  Entry:
 *      hmc - handle to compression context
 *
 *  Exit-Success:
 *      Returns MCI_ERROR_NO_ERROR;
 *      Compression context reset.
 *
 *  Exit-Failure:
 *      Returns MCI_ERROR_BAD_PARAMETERS, invalid context handle.
 */
DLLEXPORT int FAR DIAMONDAPI LCIResetCompression(LCI_CONTEXT_HANDLE hmc);


/***    LCIDestroyCompression - Destroy LCI compression context
 *
 *  Entry:
 *      hmc - handle to compression context
 *
 *  Exit-Success:
 *      Returns MCI_ERROR_NO_ERROR;
 *      Compression context destroyed.
 *
 *  Exit-Failure:
 *      Returns MCI_ERROR_BAD_PARAMETERS, invalid context handle.
 */
DLLEXPORT int FAR DIAMONDAPI LCIDestroyCompression(LCI_CONTEXT_HANDLE hmc);


/*
 * Forces encoder to flush remaining output
 */
DLLEXPORT int FAR DIAMONDAPI LCIFlushCompressorOutput(LCI_CONTEXT_HANDLE hmc);


/*
 * Set the file translation size
 * (this must be done immediately after a reset, or an LCICreateCompression)
 */
DLLEXPORT int FAR DIAMONDAPI LCISetTranslationSize(LCI_CONTEXT_HANDLE hmc, unsigned int size);


/*
 * Returns a pointer to the input data present in LZX's buffers.
 *
 * input_position is the offset of the data from the beginning of the file
 * bytes_available is the number of bytes available from that offset
 */
DLLEXPORT unsigned char * FAR DIAMONDAPI LCIGetInputData(
    LCI_CONTEXT_HANDLE hmc,
    unsigned int *input_position,
    unsigned int *bytes_available
);



/* --- constants ---------------------------------------------------------- */

/* return codes */

#define     MCI_ERROR_NO_ERROR              0
#define     MCI_ERROR_NOT_ENOUGH_MEMORY     1
#define     MCI_ERROR_BAD_PARAMETERS        2
#define     MCI_ERROR_BUFFER_OVERFLOW       3
#define     MCI_ERROR_FAILED                4
#define     MCI_ERROR_CONFIGURATION         5

/* --- LZX configuration details ------------------------------------- */

/***    LZX pvConfiguration structure
 *
 * The SecondPartitionSize must be >= 32K.  It is the amount of
 * data which LZX compresses before copymem'ing the window and
 * tree contents around.
 */

#ifdef _MSC_VER
#ifdef BIT16
#pragma pack (2)
#else
#pragma pack (4)
#endif
#endif

typedef struct {
    int WindowSize;           // buffer size
    int SecondPartitionSize;
} ALIGNED(4) LZXCONFIGURATION; /* lcfg */

#ifdef _MSC_VER
#pragma pack ()
#endif

typedef LZXCONFIGURATION *PLZXCONFIGURATION; /* plcfg */

#endif /* __LCI_H__ */

/* ----------------------------------------------------------------------- */
