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

unsigned int ba_mask[32] = {
	0x00000001, 0x00000002, 0x00000004, 0x00000008,
	0x00000010, 0x00000020, 0x00000040, 0x00000080,
	0x00000100, 0x00000200, 0x00000400, 0x00000800,
	0x00001000, 0x00002000, 0x00004000, 0x00008000,
	0x00010000, 0x00020000, 0x00040000, 0x00080000,
	0x00100000, 0x00200000, 0x00400000, 0x00800000,
	0x01000000, 0x02000000, 0x04000000, 0x08000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000
};

typedef struct _array {
	unsigned int size;
	unsigned int last;
	unsigned int *data;
} bitarray;

bitarray *ba_new (int size) {
	bitarray *created = (bitarray *)malloc(sizeof(bitarray));
	int size_in_int = 0;
	int i = 0;

	if (size%32 > 0) { size_in_int = size/32 + 1; }
	else { size_in_int = size/32; }
	if(!created) {
		return NULL;
	}

	created->data = (unsigned int *)malloc(sizeof(unsigned int)*(size_in_int));
	if(!created->data) {
		free(created);
		return NULL;
	}

	for (i = 0; i < size_in_int; i++) {
		created->data[i] = 0;
	}

	created->size = size;
	created->last = 0;

	return created;
}

bitarray *ba_new_from_existing (bitarray *src) {
	int i = 0;
	int size_in_int = 0;

	if (src->last%32 > 0) { size_in_int = src->last/32 + 1; }
	else { size_in_int = src->last/32; }

	bitarray *created = (bitarray *)malloc(sizeof(bitarray));
	if(!created) {
		return NULL;
	}

	created->data = (unsigned int *)malloc(sizeof(unsigned int)*(size_in_int));
	if(!created->data) {
		free(created);
		return NULL;
	}

	created->size = src->size;

	for (i = 0; i < src->size; i++) {
		created->data[i] = src->data[i];
	}

	return created;
}

void ba_destroy(bitarray *ba) {
	free(ba->data);
	free(ba);
	return;
}

int ba_get_bit(bitarray *ba, int pos) {
	if (pos >= ba->size || pos < 0) {
		return -1;
	}
	return (ba->data[(pos)/32] & ba_mask[(pos)%32])>>((pos)%32);
}

int ba_set_bit(bitarray *ba, int pos) {
	if (pos >= ba->size || pos < 0) {
		return -1;
	}
	(pos > ba->last) ? ba->last = pos : 1 ;
	return (ba->data[(pos)/32] |= ba_mask[(pos)%32]);
}

int ba_unset_bit(bitarray *ba, int pos) {
	if (pos >= ba->size || pos < 0) {
		return -1;
	}
	(pos > ba->last) ? ba->last = pos : 1 ;
	return (ba->data[(pos)/32] &= ~ba_mask[(pos)%32]);
}

int ba_flip_bit(bitarray *ba, int pos) {
	if (pos >= ba->size || pos < 0) {
		return -1;
	}
	(pos > ba->last) ? ba->last = pos : 1 ;
	return (ba->data[(pos)/32] ^= ba_mask[(pos)%32]);
}

#endif
