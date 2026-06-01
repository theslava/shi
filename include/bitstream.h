/*
 *      bitstream.h
 *
 *      A sequential bit-stream reader that wraps an existing file_reader.
 *      Reads bits one at a time from a byte stream without random access.
 */

#ifndef __bitstream_h__
#define __bitstream_h__

#include "file_reader.h"

/*
 * Bitstream state:
 * - fd: the underlying file reader
 * - current_byte: the byte currently being read from (0-255)
 * - bit_offset: which bit we're at within current_byte (0-7, MSB first)
 * - eof: whether we've reached the end of the stream
 */
typedef struct _bitstream {
    fr_fd *fd;
    unsigned char current_byte;
    int bit_offset;   /* 0 = most significant bit, 7 = least significant bit */
    int eof;
} bitstream;

/*
 * Create a new bitstream from an existing file descriptor.
 * Returns NULL on failure.
 */
bitstream* bs_new(fr_fd *fd);

/*
 * Read a single bit (0 or 1) from the stream.
 * Returns -1 on EOF or error.
 */
int bs_read_bit(bitstream *bs);

/*
 * Read n bits from the stream, returned as an unsigned int.
 * Bits are read MSB-first (most significant bit first).
 * Returns 0 on EOF before any bits read, or partial result if EOF mid-read.
 * If n > 32, behavior is undefined (caller should limit to 32).
 */
unsigned int bs_read_bits(bitstream *bs, int n);

/*
 * Check if the bitstream has reached EOF.
 */
int bs_eof(bitstream *bs);

/*
 * Clean up and free the bitstream. Does NOT close the underlying fr_fd.
 */
void bs_done(bitstream *bs);

#endif /* __bitstream_h__ */