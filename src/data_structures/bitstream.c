/*
 *      bitstream.c
 *
 *      Sequential bit-stream reader/writer implementation.
 *      Wraps an existing fr_fd to provide bit-level access to a byte stream.
 *      Reads/Writes MSB-first (bit 7, then 6, ..., then 0 of each byte).
 */

#include "data_structures/bitstream.h"
#include <stdlib.h>

/* ==========================================================================
 * READER API
 * ========================================================================== */

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

/* ==========================================================================
 * WRITER API
 * ========================================================================== */

/*
 * Create a new bitstream writer from an existing file descriptor.
 * The writer takes ownership of writing to the fd but does NOT
 * close it — you must still call fr_done(fd) separately.
 */
bitstream_writer* bsw_new(fr_wd *fd) {
    if (!fd) return NULL;

    bitstream_writer *bsw = (bitstream_writer*) malloc(sizeof(bitstream_writer));
    if (!bsw) return NULL;

    bsw->fd = fd;
    bsw->current_byte = 0;
    bsw->bit_offset = 0;   /* Start at MSB (bit 7) */
    bsw->bits_written = 0;

    return bsw;
}

/*
 * Write a single bit to the stream.
 * Bits are written MSB-first.
 */
void bsw_write_bit(bitstream_writer *bsw, int bit) {
    if (!bsw || bit < 0 || bit > 1) return;

    /* Set the bit at current position (MSB first) */
    unsigned char mask = (unsigned char)(1 << (7 - bsw->bit_offset));
    if (bit) {
        bsw->current_byte |= mask;
    } else {
        bsw->current_byte &= ~mask;
    }

    bsw->bit_offset++;
    bsw->bits_written++;

    /* If byte is full, write it out */
    if (bsw->bit_offset > 7) {
        fw_write_byte(bsw->fd, bsw->current_byte);
        bsw->current_byte = 0;
        bsw->bit_offset = 0;
    }
}

/*
 * Write n bits from the given value to the stream.
 * Bits are written MSB-first (most significant bit first).
 *
 * Example: if value = 0xCA (11001010) and n = 8,
 *          writes bits in order: 1, 1, 0, 0, 1, 0, 1, 0
 */
void bsw_write_bits(bitstream_writer *bsw, unsigned int value, int n) {
    if (!bsw || n <= 0 || n > 32) return;

    /* Write each bit from MSB to LSB */
    for (int i = n - 1; i >= 0; i--) {
        int bit = (value >> i) & 1;
        bsw_write_bit(bsw, bit);
    }
}

/*
 * Flush any remaining partial byte to the output file.
 * This should be called before closing the writer to ensure all bits are written.
 */
void bsw_flush(bitstream_writer *bsw) {
    if (!bsw || bsw->bit_offset == 0) return;

    /* Pad with zeros and write the remaining byte */
    fw_write_byte(bsw->fd, bsw->current_byte);
    bsw->current_byte = 0;
    bsw->bit_offset = 0;
}

/*
 * Clean up the bitstream writer.
 * Flushes any remaining data and frees memory. Does NOT close the underlying fr_fd.
 */
void bsw_done(bitstream_writer *bsw) {
    if (bsw) {
        bsw_flush(bsw);
        free(bsw);
    }
}
