/*
 *      bitstream.c
 *
 *      Sequential bit-stream reader implementation.
 *      Wraps an existing fr_fd to provide bit-level access to a byte stream.
 *      Reads MSB-first (bit 7, then 6, ..., then 0 of each byte).
 */

#include "bitstream.h"
#include <stdlib.h>

/*
 * Create a new bitstream from an existing file descriptor.
 * The bitstream takes ownership of reading from the fd but does NOT
 * close it — you must still call fr_done(fd) separately.
 */
bitstream* bs_new(fr_fd *fd) {
    if (!fd) return NULL;

    bitstream *bs = (bitstream*) malloc(sizeof(bitstream));
    if (!bs) return NULL;

    bs->fd = fd;
    bs->current_byte = 0;
    bs->bit_offset = 0;  /* Start at MSB (bit 7) */
    bs->eof = 0;

    return bs;
}

/*
 * Load the next byte from the file descriptor.
 * Returns 0 on EOF.
 */
static int bs_load_byte(bitstream *bs) {
    int byte = fr_read(bs->fd);
    if (byte == EOF) {
        bs->eof = 1;
        return 0;
    }
    bs->current_byte = (unsigned char)byte;
    bs->bit_offset = 0;  /* Reset to MSB */
    return 1;
}

/*
 * Read a single bit (0 or 1) from the stream.
 * Returns -1 on EOF/error.
 */
int bs_read_bit(bitstream *bs) {
    if (!bs || bs->eof) return -1;

    /* Load a new byte if we've exhausted the current one */
    if (bs->bit_offset > 7) {
        if (!bs_load_byte(bs)) return -1;
    }

    /* Extract the bit at current offset (MSB first) */
    int bit = (bs->current_byte >> (7 - bs->bit_offset)) & 1;
    bs->bit_offset++;

    return bit;
}

/*
 * Read n bits from the stream, returned as an unsigned int.
 * Bits are read MSB-first.
 *
 * Example: if we read 8 bits 11001010, result = 0xCA = 202
 *
 * Returns:
 *   - The accumulated value if at least 1 bit was read
 *   - 0 if EOF was reached before any bits
 *   - Partial result if EOF hit mid-read (n bits not available)
 */
unsigned int bs_read_bits(bitstream *bs, int n) {
    if (!bs || n <= 0 || n > 32) return 0;
    if (bs->eof) return 0;

    unsigned int result = 0;
    for (int i = 0; i < n; i++) {
        int bit = bs_read_bit(bs);
        if (bit == -1) {
            /* EOF reached — return partial result */
            return result;
        }
        /* Shift accumulated result left and add new bit */
        result = (result << 1) | (unsigned int)bit;
    }

    return result;
}

/*
 * Check if the bitstream has reached EOF.
 */
int bs_eof(bitstream *bs) {
    return bs ? bs->eof : 1;
}

/*
 * Clean up the bitstream. Does NOT close the underlying fr_fd.
 */
void bs_done(bitstream *bs) {
    if (bs) {
        free(bs);
    }
}
