/*
 *      bitstream.h
 *
 *      A sequential bit-stream reader that wraps an existing file_reader.
 *      Reads bits one at a time from a byte stream without random access.
 */

#ifndef __bitstream_h__
#define __bitstream_h__

#include "io/file_io.h"

/*
 * Bitstream state (READER):
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

/* --- Reader API (existing) --- */
bitstream* bs_new(fr_fd *fd);
int bs_read_bit(bitstream *bs);
unsigned int bs_read_bits(bitstream *bs, int n);
int bs_eof(bitstream *bs);
void bs_done(bitstream *bs);

/*
 * Bitstream state (WRITER):
 * - fd: the underlying file writer
 * - current_byte: the byte currently being written to
 * - bit_offset: next bit position to write (0-7, MSB first)
 * - bits_written: total number of bits written so far
 */
typedef struct _bitstream_writer {
    fr_wd *fd;
    unsigned char current_byte;
    int bit_offset;   /* 0 = most significant bit, 7 = least significant bit */
    int bits_written;
} bitstream_writer;

/* --- Writer API (new) --- */
bitstream_writer* bsw_new(fr_wd *fd);
void bsw_write_bit(bitstream_writer *bsw, int bit);
void bsw_write_bits(bitstream_writer *bsw, unsigned int value, int n);
void bsw_flush(bitstream_writer *bsw);
void bsw_done(bitstream_writer *bsw);

#endif /* __bitstream_h__ */