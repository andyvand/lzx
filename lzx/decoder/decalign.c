/*
 * decalign.c
 *
 * Decoding aligned offset block
 */
#include "decoder.h"


static int special_decode_aligned_block(t_decoder_context *context, int bufpos, int amount_to_decode)
{
	uint	match_pos;
	uint	temp_pos;
	int	bufpos_end;
	int		match_length;
	int		c;
	uint	dec_bitbuf;
	char	dec_bitcount;
    byte   *dec_input_curpos;
    byte   *dec_end_input_pos;
	byte   *dec_mem_window;
	char	m;

	/*
	 * Store commonly used variables locally
	 */
	dec_bitcount	  = context->dec_bitcount;
	dec_bitbuf		  = context->dec_bitbuf;
	dec_input_curpos  = context->dec_input_curpos;
	dec_end_input_pos = context->dec_end_input_pos;
	dec_mem_window	  = context->dec_mem_window;

	bufpos_end = bufpos + amount_to_decode;

	while (bufpos < bufpos_end)
	{
		/*
		 * Decode an item
		 */
		DECODE_MAIN_TREE(c);

		if ((c -= NUM_CHARS) < 0)
		{
			dec_mem_window[bufpos] = (byte) c;
			dec_mem_window[context->dec_window_size + bufpos] = (byte) c;
			bufpos++;
		}
		else
		{
	 		/*
	  		 * Get match length slot
	  		 */
			if ((match_length = c & NUM_PRIMARY_LENGTHS) == NUM_PRIMARY_LENGTHS)
			{
				DECODE_LEN_TREE_NOEOFCHECK(match_length);
			}

	 		/*
	  		 * Get match position slot
	  		 */
			m = c >> NL_SHIFT;

			if (m > 2)
			{
				if (dec_extra_bits[(uint)m] >= 3)
				{
					if (dec_extra_bits[(uint)m]-3)
					{
						/* no need to getbits17 */
	    				GET_BITS_NOEOFCHECK(dec_extra_bits[(uint)m] - 3, temp_pos);
					}
					else
					{
						temp_pos = 0;
					}

	    			match_pos = MP_POS_minus2[(uint)m] + (temp_pos << 3);

	    			DECODE_ALIGNED_NOEOFCHECK(temp_pos);
	    			match_pos += temp_pos;
				}
				else
				{
					if (dec_extra_bits[(uint)m])
					{
	    				GET_BITS_NOEOFCHECK(dec_extra_bits[(uint)m], match_pos);

						match_pos += MP_POS_minus2[(uint)m];
					}
					else
					{
                        match_pos = 1; // MP_POS_minus2[m==3];
					}
				}

				context->dec_last_matchpos_offset[2] = context->dec_last_matchpos_offset[1];
				context->dec_last_matchpos_offset[1] = context->dec_last_matchpos_offset[0];
				context->dec_last_matchpos_offset[0] = match_pos;
			}
			else
	 		{
				match_pos = context->dec_last_matchpos_offset[(uint)m];

				if (m)
				{
					context->dec_last_matchpos_offset[(uint)m] = context->dec_last_matchpos_offset[0];
					context->dec_last_matchpos_offset[0] = match_pos;
				}
	 		}

			match_length += MIN_MATCH;

			do
			{
                dec_mem_window[bufpos] = dec_mem_window[(bufpos-match_pos) & context->dec_window_mask];

				if (bufpos < MAX_MATCH)
					dec_mem_window[context->dec_window_size+bufpos] = dec_mem_window[bufpos];

				bufpos++;
			} while (--match_length > 0);
		}
	}

	context->dec_bitcount	  = dec_bitcount;
	context->dec_bitbuf		  = dec_bitbuf;
	context->dec_input_curpos = dec_input_curpos;

    return bufpos;
}


#ifndef ASM_DECODE_ALIGNED_OFFSET_BLOCK
int fast_decode_aligned_offset_block(t_decoder_context *context, int bufpos, int amount_to_decode)
{
	uint	match_pos;
	uint	temp_pos;
	int	bufpos_end;
	int	decode_residue;
	int		match_length;
	int		c;
	uint	dec_bitbuf;
	char	dec_bitcount;
    byte   *dec_input_curpos;
    byte   *dec_end_input_pos;
	byte   *dec_mem_window;
	uint	match_ptr;
	char	m;

	/*
	 * Store commonly used variables locally
	 */
	dec_bitcount	  = context->dec_bitcount;
	dec_bitbuf		  = context->dec_bitbuf;
	dec_input_curpos  = context->dec_input_curpos;
	dec_end_input_pos = context->dec_end_input_pos;
	dec_mem_window	  = context->dec_mem_window;

	bufpos_end = bufpos + amount_to_decode;

	while (bufpos < bufpos_end)
	{
		/*
		 * Decode an item
		 */
		DECODE_MAIN_TREE(c);

		if ((c -= NUM_CHARS) < 0)
		{
			dec_mem_window[bufpos++] = (byte) c;
		}
		else
		{
	 		/*
	  		 * Get match length slot
	  		 */
			if ((match_length = c & NUM_PRIMARY_LENGTHS) == NUM_PRIMARY_LENGTHS)
			{
				DECODE_LEN_TREE_NOEOFCHECK(match_length);
			}

	 		/*
	  		 * Get match position slot
	  		 */
			m = c >> NL_SHIFT;

			if (m > 2)
			{
				if (dec_extra_bits[(uint)m] >= 3)
				{
                    if ((dec_extra_bits[(uint)m]-3) != 0)
					{
						/* no need to getbits17 */
	    				GET_BITS_NOEOFCHECK(dec_extra_bits[(uint)m] - 3, temp_pos);
					}
					else
					{
						temp_pos = 0;
					}

	    			match_pos = MP_POS_minus2[(uint)m] + (temp_pos << 3);

	    			DECODE_ALIGNED_NOEOFCHECK(temp_pos);
	    			match_pos += temp_pos;
				}
				else
				{
					if (dec_extra_bits[(uint)m])
					{
	    				GET_BITS_NOEOFCHECK(dec_extra_bits[(uint)m], match_pos);

						match_pos += MP_POS_minus2[(uint)m];
					}
					else
					{
						match_pos = MP_POS_minus2[(uint)m];
					}
				}

				context->dec_last_matchpos_offset[2] = context->dec_last_matchpos_offset[1];
				context->dec_last_matchpos_offset[1] = context->dec_last_matchpos_offset[0];
				context->dec_last_matchpos_offset[0] = match_pos;
			}
			else
	 		{
				match_pos = context->dec_last_matchpos_offset[(uint)m];

				if (m)
				{
					context->dec_last_matchpos_offset[(uint)m] = context->dec_last_matchpos_offset[0];
					context->dec_last_matchpos_offset[0] = match_pos;
				}
	 		}

			match_length += MIN_MATCH;
			match_ptr = (bufpos - match_pos) & context->dec_window_mask;

			do
			{
				dec_mem_window[bufpos++] = dec_mem_window[match_ptr++];
			} while (--match_length > 0);
		}
	}

	context->dec_bitcount	  = dec_bitcount;
	context->dec_bitbuf		  = dec_bitbuf;
	context->dec_input_curpos = dec_input_curpos;

	/* should be zero */
	decode_residue = bufpos - bufpos_end;

    bufpos &= context->dec_window_mask;
	context->dec_bufpos = bufpos;

	return decode_residue;
}
#endif


int decode_aligned_offset_block(
    t_decoder_context * context, 
    int                BufPos, 
    int                 amount_to_decode
)
{
    /*
     * Special case code when BufPos is near the beginning of the window;
     * we must properly update our MAX_MATCH wrapper bytes.
     */
    if (BufPos < MAX_MATCH)
    {
        int    new_bufpos;
        int    amount_to_slowly_decode;

        amount_to_slowly_decode = min(MAX_MATCH-BufPos, amount_to_decode);

        /*
         * It's ok to end up decoding more than we wanted if we
         * restricted it to decoding only MAX_MATCH; there's
         * no guarantee a match doesn't straddle MAX_MATCH
         */
        new_bufpos = special_decode_aligned_block(
            context,
            BufPos,
            amount_to_slowly_decode
        );

        amount_to_decode -= (new_bufpos-BufPos);

        context->dec_bufpos = BufPos = new_bufpos;

        /*
         * Note: if amount_to_decode < 0 then we're in trouble
         */
        if (amount_to_decode <= 0)
            return amount_to_decode;
    }

    return fast_decode_aligned_offset_block(context, BufPos, amount_to_decode);
}
