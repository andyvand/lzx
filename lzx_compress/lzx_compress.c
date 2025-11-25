#include <stdint.h>
#include <sys/types.h>
#include <lci.h>
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
    ULONG amount
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

static FILE *s_hWriteFile = NULL;
static FILE *outFile = NULL;

static
int
DIAMONDAPI
CompressCallback(
    void *ptr,
    unsigned char* compressed_data,
    long compressed_size,
    long uncompressed_size
    )
{
    ULONG BytesWritten;
    LZXBOX_BLOCK Block;

    (void)ptr;

    //
    // write block header
    //
    Block.CompressedSize = (USHORT)compressed_size;
    Block.UncompressedSize = (USHORT)uncompressed_size;

    if (!(BytesWritten = (ULONG)fwrite(&Block, 1, sizeof(Block), outFile))) {
        fprintf(stderr, "Cannot write size to file\n");
        return -1;
    } else {
        fprintf(stderr, "Wrote header: uncompressed=%u, compressed=%u\n", Block.UncompressedSize, Block.CompressedSize);
    }

    //
    // write compressed data
    //
    if (!(BytesWritten = (ULONG)fwrite(compressed_data, 1, Block.CompressedSize, outFile))) {
        fprintf(stderr, "Cannot write data to file\n");
        return -1;
    } else {
        fprintf(stderr, "Wrote data: compressed size=%u\n", Block.CompressedSize);
    }

    return 0;
}

BOOL CompressFile(
    LPCSTR pszCompressName,
    LPCSTR pszOutName
    )
{
    BOOL bRet = FALSE;
    UINT SourceSize = LZX_CHUNK_SIZE;
    LCI_CONTEXT_HANDLE Handle = (LCI_CONTEXT_HANDLE)NULL;
    PVOID Source = NULL;
    LZXCONFIGURATION cfg;
    UINT DestSize;
    ULONG BytesRead;
    ULONG BytesCompressed;
    int LCIRet;
    ULONG BytesRemaining;

    //
    // initialize the compression engine
    //
    cfg.WindowSize = LZX_WINDOW_SIZE;
    cfg.SecondPartitionSize = 65536;

    LCIRet = LCICreateCompression(&SourceSize, &cfg, LzxAlloc, LzxFree, &DestSize, &Handle,
        CompressCallback, NULL);
    if (LCIRet != MCI_ERROR_NO_ERROR)
    {
        fprintf(stderr, "Cannot create compression %d\n", LCIRet);
        Handle = (LCI_CONTEXT_HANDLE)NULL;
        goto Cleanup;
    }

    Source = LzxAlloc(SourceSize);

    //
    // open files
    //
#if __STDC_WANT_SECURE_LIB__
    fopen_s(&s_hWriteFile, psxCompressName, "rb");
#else
    s_hWriteFile = fopen(pszCompressName, "rb");
#endif

    if (s_hWriteFile == NULL) {
        fprintf(stderr, "Cannot open %s for reading\n", pszCompressName);
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
        fprintf(stderr, "Cannot open %s for writing\n", pszCompressName);
        goto Cleanup;
    }

    fseek(s_hWriteFile, 0, SEEK_END);
    BytesRemaining = ftell(s_hWriteFile);
    fseek(s_hWriteFile, 0, SEEK_SET);

    (void)LCIFlushCompressorOutput(Handle);
    while (BytesRemaining > 0) {
        BytesRead = (BytesRemaining > SourceSize) ? SourceSize : BytesRemaining;
        Source = (PVOID)LzxAlloc(BytesRead);

        fread(Source, 1, BytesRead, s_hWriteFile);

        BytesRemaining -= BytesRead;

        LCIRet = LCICompress(Handle, Source, BytesRead, NULL, DestSize, &BytesCompressed);
        if (LCIRet != MCI_ERROR_NO_ERROR)
        {
            fprintf(stderr, "Compression failed %d\n", LCIRet);
            goto Cleanup;
        }

        if (Source != NULL)
        {
            LzxFree(Source);
            Source = NULL;
        }
    }
    (void)LCIFlushCompressorOutput(Handle);

    bRet = TRUE;
    // fall through

Cleanup:

    if (Handle != (LCI_CONTEXT_HANDLE)NULL)
    {
        (void)LCIDestroyCompression(Handle);
    }

    if (s_hWriteFile != NULL)
    {
        fclose(s_hWriteFile);
        s_hWriteFile = NULL;
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

    return bRet;
}

void Usage(char *name)
{
    fprintf(stdout, "LZX compressor app\n");
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
    fprintf(stdout, "Compressed %s to %s\n", argv[1], argv[2]);

    return 0;
}
    
