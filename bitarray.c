#include "bitarray.h"

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

	for (i = 0; i < size_in_int; i++) {
		created->data[i] = src->data[i];
	}

	return created;
}

void ba_destroy(bitarray *ba) {
	if (ba != NULL) {
	free(ba->data);
	free(ba);
}
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

/* Write the bitarray to a file descriptor (byte-aligned). Returns 0 on success. */
int ba_write_to_file(bitarray *ba, fr_fd *fd) {
	/* TODO: Implement writing bitarray data to file
	 *
	 * Algorithm:
	 * 1. Calculate number of full bytes needed: (last + 8) / 8
	 * 2. Iterate through each byte position
	 * 3. Extract bits from the bitarray and pack them into bytes
	 * 4. Write each packed byte using fr_write_byte() or similar */
	return -1;
}
