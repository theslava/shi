/*
 *      list.h
 *
 *      Copyright 2007 Vyacheslav Goltser <slavikg@gmail.com>
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __bitarray_h__
#define  __bitarray_h__

#include <malloc.h>
#include "io/file_io.h"

extern unsigned int ba_mask[32];
typedef struct _array {
	unsigned int size;
	unsigned int last;
	unsigned int *data;
} bitarray;

bitarray *ba_new (int size);
bitarray *ba_new_from_existing (bitarray *src);
void ba_destroy(bitarray *ba);
int ba_get_bit(bitarray *ba, int pos);
int ba_set_bit(bitarray *ba, int pos);
int ba_unset_bit(bitarray *ba, int pos);
int ba_flip_bit(bitarray *ba, int pos);

/* Write the bitarray to a file descriptor (byte-aligned). Returns 0 on success. */
int ba_write_to_file(bitarray *ba, fr_fd *fd);

#endif

