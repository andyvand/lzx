#include <stdint.h>
#include <sys/types.h>
#include <ldi.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef void * PVOID;
typedef unsigned char BOOL;
typedef unsigned short USHORT;
typedef const char * LPCSTR;

typedef struct _LZXBOX_BLOCK {
	USHORT CompressedSize;
	USHORT UncompressedSize;
} LZXBOX_BLOCK;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef LZX_CHUNK_SIZE
#define LZX_CHUNK_SIZE (32*1024)
#endif

#ifndef LZX_WINDOW_SIZE
#define LZX_WINDOW_SIZE (128*1024)
#endif

#ifndef LZX_WORKSPACE
#define LZX_WORKSPACE (256*1024)
#endif

static
MI_MEMORY 
DIAMONDAPI LzxAlloc(
    UINT amount
    )
{
    return(malloc((size_t)amount));
}


static
void DIAMONDAPI LzxFree(
    MI_MEMORY pointer
    )
{
    free(pointer);
}

static FILE *inFile = NULL;
static FILE *outFile = NULL;

INT_PTR FAR DIAMONDAPI LzxOpen (char FAR *pszFile,int oflag,int pmode)
{
#if __STDC_WANT_SECURE_LIB__
    FILE *f = NULL;
    fopen_s(&f, pszFile, "rwb");
    return (INT_PTR)f;
#else
    return (INT_PTR)fopen(pszFile, "rwb");
#endif
}

BOOL CompressFile(
    LPCSTR pszDecompressName,
    LPCSTR pszOutName
    )
{
    BOOL bRet = FALSE;
    UINT SourceSize = LZX_CHUNK_SIZE;
    LDI_CONTEXT_HANDLE Handle = (LDI_CONTEXT_HANDLE)0;
    PVOID Source = NULL;
    PVOID Dest = NULL;
    UINT DestSize;
    UINT dwSrcSize = 32768;
    UINT dwDestSize = 32768;
    UINT BytesDecompressed;
    int LDIRet;
    UINT BytesRemaining;
    LZXDECOMPRESS Decomp;
    LZXBOX_BLOCK Block;

    //
    // initialize the compression engine
    //
    Decomp.fCPUtype = LDI_CPU_80386;
    Decomp.WindowSize = LZX_WINDOW_SIZE;

    LDIRet = LDICreateDecompression(&SourceSize, &Decomp, LzxAlloc, LzxFree, &DestSize, &Handle,
        NULL, NULL, NULL,NULL,NULL);
    if (LDIRet != MDI_ERROR_NO_ERROR)
    {
        fprintf(stderr, "Cannot create decompression %d\n", LDIRet);
        Handle = (LDI_CONTEXT_HANDLE)0;
        goto Cleanup;
    }

    //
    // open files
    //
#if __STDC_WANT_SECURE_LIB__
    fopen_s(&inFile, psDecompressName, "rb");
#else
    inFile = fopen(pszDecompressName, "rb");
#endif

    if (inFile == NULL) {
        fprintf(stderr, "Cannot open %s for reading\n", pszDecompressName);
        goto Cleanup;
    }

    //
    // perform the compression
    //
#if __STDC_WANT_SECURE_LIB__
    fopen_s(&outFile, pszOutName, "wb");
#else
    outFile = fopen(pszOutName, "wb");
#endif

    if (outFile == NULL) {
        fprintf(stderr, "Cannot open %s for writing\n", pszDecompressName);
        goto Cleanup;
    }

    fseek(inFile, 0, SEEK_END);
    BytesRemaining = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);

    while (BytesRemaining > 0) {
        fread(&Block, 1, sizeof(LZXBOX_BLOCK), inFile);

        Source = (PVOID)malloc(dwSrcSize);
        Dest = (PVOID)malloc(dwDestSize);

        if (Block.CompressedSize > dwSrcSize)
        {
            dwSrcSize = Block.CompressedSize;
            Source = (PVOID)realloc((void*)Source, dwSrcSize);
        }

        if (Block.UncompressedSize > dwDestSize)
        {
            dwDestSize = Block.UncompressedSize;
            Dest = (PVOID)realloc((void*)Dest, dwDestSize);
        }

        fprintf(stderr, "Decompressing %hu bytes to %hu bytes (source size: %u, dest size: %u)\n", Block.CompressedSize, Block.UncompressedSize, (UINT)SourceSize, (UINT)DestSize);
        fread(Source, 1, Block.CompressedSize, inFile);

        BytesRemaining -= sizeof(LZXBOX_BLOCK) + Block.CompressedSize;
        BytesDecompressed = (UINT)Block.UncompressedSize;

        LDIRet = LDIDecompress(Handle, Source, Block.CompressedSize, Dest, &BytesDecompressed);
        if (LDIRet != MDI_ERROR_NO_ERROR)
        {
            fprintf(stderr, "Decompression failed %d, (bytes decompressed: %u)\n", LDIRet, (UINT)BytesDecompressed);
            goto Cleanup;
        }

        if (Source != NULL)
        {
            LzxFree(Source);
            Source = NULL;
        }

        fwrite(Dest, 1, BytesDecompressed, outFile);

        if (Dest != NULL)
        {
            LzxFree(Dest);
            Dest = NULL;
        }
    }

    bRet = TRUE;
    // fall through

Cleanup:

    if (Handle != (LDI_CONTEXT_HANDLE)0)
    {
        (void)LDIDestroyDecompression(Handle);
    }

    if (inFile != NULL)
    {
        fclose(inFile);
        inFile = NULL;
    }

    if (outFile != NULL)
    {
        fclose(outFile);
        outFile = NULL;
    }

    if (Source != NULL)
    {
        LzxFree(Source);
        Source = NULL;
    }

    if (Dest != NULL)
    {
        LzxFree(Dest);
        Dest = NULL;
    }

    return bRet;
}

void Usage(char *name)
{
    fprintf(stdout, "LZX decompressor app\n");
    fprintf(stdout, "Copyright (C) 2025 - AnV Software\n");
    fprintf(stdout, "Usage:\n%s <infile> <outfile>\n", name);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }

    CompressFile(argv[1], argv[2]);
    fprintf(stdout, "Decompressed %s to %s\n", argv[1], argv[2]);

    return 0;
}
    
