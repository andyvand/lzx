/*
 * encapi.h
 *
 * Definitions for calling LZX apis (encapi.c)
 */

/*
 * Return codes for LZX_Encode()
 */
#define ENCODER_SUCCESS			0
#define ENCODER_READ_FAILURE 	1
#define ENCODER_WRITE_FAILURE 	2
#define ENCODER_CONSOLE_FAILURE	3


bool LZX_EncodeInit(

	t_encoder_context *	context,
	int				compression_window_size,
	int				second_partition_size,
	PFNALLOC			pfnma,
	PFNFREE				pfnmf,

	int FAR (DIAMONDAPI *pfnlzx_output_callback)(
			void *			pfol,
			unsigned char *	compressed_data,
			int			compressed_size,
			int			uncompressed_size
    ),

    void FAR *          fci_data
);

void LZX_EncodeFree(t_encoder_context *context);

void LZX_EncodeNewGroup(t_encoder_context *context);

int LZX_Encode(
	t_encoder_context *	context,
	byte *				input_data,
	int				input_size,
	int *				bytes_compressed,
	int				file_size_for_translation
);

bool LZX_EncodeFlush(t_encoder_context *context);

unsigned char *LZX_GetInputData(
    t_encoder_context *context,
    unsigned int *input_position,
    unsigned int *bytes_available
);

