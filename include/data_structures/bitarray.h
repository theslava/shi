/* Released to the public domain — No rights reserved. */

#ifndef __bitarray_h__
#define __bitarray_h__

#include <malloc.h>
#include "io/file_io.h"

extern unsigned int ba_mask[32];
typedef struct _array {
    unsigned int size;
    unsigned int last;
    unsigned int* data;
} bitarray;

bitarray* ba_new(unsigned int size);
bitarray* ba_new_from_existing(bitarray* src);
void ba_destroy(bitarray* ba);
int ba_get_bit(bitarray* ba, unsigned int pos);
int ba_set_bit(bitarray* ba, unsigned int pos);
int ba_unset_bit(bitarray* ba, unsigned int pos);
int ba_flip_bit(bitarray* ba, unsigned int pos);

/* Write the bitarray to a file descriptor (byte-aligned). Returns 0 on success. */
int ba_write_to_file(bitarray* ba, fw_fd* fd);

#endif
