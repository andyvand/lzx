/*
 * decin.c
 *
 * Decoder inputting of compressed data
 */
#include "decoder.h"


/*
 * Initialises the bit buffer state
 */
void NEAR initialise_decoder_bitbuf(t_decoder_context *context)
{
	byte *p;

    /*
     * If we're decoding an uncompressed block, don't use the
     * bit buffer; we're reading directly out of the input.
     */
    if (context->dec_block_type == BLOCKTYPE_UNCOMPRESSED)
        return;

    if ((context->dec_input_curpos + sizeof(uint)) > context->dec_end_input_pos)
        return;

    p = context->dec_input_curpos;

	context->dec_bitbuf =
        ((uint) p[2] | (((uint) p[3]) << 8)) |
        ((((uint) p[0] | (((uint) p[1]) << 8))) << 16);

	context->dec_bitcount = 16;
    context->dec_input_curpos += 4;
}


/*
 * Initialise input buffer and bitwise i/o
 */
void NEAR init_decoder_input(t_decoder_context *context)
{
   initialise_decoder_bitbuf(context);
}


void NEAR fillbuf(t_decoder_context *context, int n)
{
	context->dec_bitbuf <<= n;            					
	context->dec_bitcount -= (char)n;
	
	if (context->dec_bitcount <= 0)      					
	{                                 				
		if (context->dec_input_curpos >= context->dec_end_input_pos)	
        {
            context->dec_error_condition = true;
            return;
        }

        context->dec_bitbuf |= ((((uint) *context->dec_input_curpos | (((uint) *(context->dec_input_curpos+1)) << 8))) << (-context->dec_bitcount));
        context->dec_input_curpos += 2;
        context->dec_bitcount += 16;

		if (context->dec_bitcount <= 0)							
		{                                 				
            if (context->dec_input_curpos >= context->dec_end_input_pos)
            {
                context->dec_error_condition = true;
                return;
            }

            context->dec_bitbuf |= ((((uint) *context->dec_input_curpos | (((uint) *(context->dec_input_curpos+1)) << 8))) << (-context->dec_bitcount));
            context->dec_input_curpos += 2;
			context->dec_bitcount += 16;               			
		}												
   }                                    				
}


uint NEAR getbits(t_decoder_context *context, int n)
{
	uint value;

	value = context->dec_bitbuf >> (32-(n));			
	fillbuf(context, n);

	return value;
}
