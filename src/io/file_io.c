#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "io/file_io.h"

// creates a new file descriptor and opens the file for reading
fr_fd* fr_new(char* file_path, unsigned int bsize) {
	if (!file_path) return NULL;

	// new file descriptor, create enough memory for stuff we need
	fr_fd* ret_fd = (fr_fd*) malloc(sizeof(fr_fd));
	if (!ret_fd) return NULL;

	ret_fd->file_path = (char*) malloc(strlen(file_path) + 1);
	if (!ret_fd->file_path) {
		free(ret_fd);
		return NULL;
	}
	strcpy(ret_fd->file_path, file_path);

	#ifdef _WIN32
	ret_fd->file = open(file_path, O_RDONLY | O_BINARY);
#else
	ret_fd->file = open(file_path, O_RDONLY);
#endif
	if (ret_fd->file < 0) {
		free(ret_fd->file_path);
		free(ret_fd);
		return NULL;
	}

	ret_fd->buffer = (unsigned char*) malloc(bsize);
	if (!ret_fd->buffer) {
		close(ret_fd->file);
		free(ret_fd->file_path);
		free(ret_fd);
		return NULL;
	}

	// set up the file descriptor
	ret_fd->buffer_size = bsize;
	ret_fd->pos = 0;
	ret_fd->inbuf = 0;
	ret_fd->eof_flag = 0;

	return ret_fd;
}

// returns the next byte or -1 on error
int fr_read(fr_fd *fd) {
	if (!fd) return EOF;
	int t = 0;
	if(fd->eof_flag != 1) {
		if(fd->pos >= fd->inbuf) {
			t = read(fd->file, fd->buffer, fd->buffer_size);
			fd->pos=0;
			if (t == 0) {
				fd->eof_flag = 1;
				return EOF;
			}
			else {
				fd->inbuf = t;
			}
		}
		return fd->buffer[fd->pos++];
	}
	else return EOF;
}

// goto beginning of the file
void fr_rewind(fr_fd *fd) {
	lseek(fd->file,0,SEEK_SET);
	fd->pos = 0;
	fd->inbuf = 0;
	fd->eof_flag = 0;
	return;
}

// delete, cleanup, close the file related stuffs
void fr_done(fr_fd *fd) {
	if (!fd) return;
	close(fd->file);
	free(fd->buffer);
	free(fd->file_path);
	free(fd);
	return;
}

// output current info of the file
void fr_info(fr_fd *fd) {
	fprintf(stderr, "File path = %s\n", fd->file_path);
	fprintf(stderr, "Descriptor = %d\n", fd->file);
	fprintf(stderr, "Current buffer = \n%s\n", fd->buffer);
	fprintf(stderr, "Buffer size = %d\n", fd->buffer_size);
	fprintf(stderr, "Current position in buffer = %d\n", fd->pos);	// position within the buffer
	fprintf(stderr, "Bytes in buffer = %d\n", fd->inbuf); // actual number of bytes in the buffer
	fprintf(stderr, "EOF flag = %d\n", fd->eof_flag);
}

/* ==========================================================================
 * Writer API Implementation
 * ========================================================================== */

/* creates a new file writer and opens the file for writing */
fw_fd* fw_new(const char* file_path, unsigned int bsize) {
	if (!file_path || !bsize) return NULL;

	fw_fd* ret = (fw_fd*) malloc(sizeof(fw_fd));
	if (!ret) return NULL;

	ret->file_path = (char*) malloc(strlen(file_path) + 1);
	strcpy(ret->file_path, file_path);

	/* Open for writing/truncating; create if doesn't exist */
#ifdef _WIN32
	ret->file = open(file_path, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0644);
#else
	ret->file = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif


	if (ret->file < 0) {
		free(ret->file_path);
		free(ret);
		return NULL;
	}

	ret->buffer = (unsigned char*) malloc(bsize);
	if (!ret->buffer) {
		close(ret->file);
		free(ret->file_path);
		free(ret);
		return NULL;
	}

	ret->buffer_size = bsize;
	ret->pos = 0;   /* next write position within buffer */

	return ret;
}

// writes a single byte to the file (with buffering)
int fw_write_byte(fw_fd *wd, unsigned char byte) {
	if (!wd) return -1;

	/* If buffer is full, flush it first */
	if (wd->pos >= wd->buffer_size) {
		fw_flush(wd);
	}

	wd->buffer[wd->pos++] = byte;
	return 0;
}

// writes an array of bytes to the file
int fw_write_bytes(fw_fd *wd, const unsigned char *data, unsigned int len) {
	if (!wd || !data || len == 0) return -1;

	/* If data is larger than buffer, write directly */
	if (len >= wd->buffer_size) {
		fw_flush(wd);
		write(wd->file, data, len);
		return 0;
	}

	/* Otherwise, fill the buffer and flush when full */
	for (unsigned int i = 0; i < len; i++) {
		if (wd->pos >= wd->buffer_size) {
			fw_flush(wd);
		}
		wd->buffer[wd->pos++] = data[i];
	}

    return 0;
}

// flushes the buffer to disk
void fw_flush(fw_fd *wd) {
	if (!wd || wd->pos == 0) return;

	write(wd->file, wd->buffer, wd->pos);
	wd->pos = 0;   /* reset position for next batch */
}

// delete, cleanup, close the file writer
void fw_done(fw_fd *wd) {
	if (!wd) return;

	/* Flush any remaining data in buffer */
	fw_flush(wd);

	close(wd->file);
	free(wd->buffer);
	free(wd->file_path);
	free(wd);
}