#ifndef FILE_READER_H
#define FILE_READER_H
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

typedef struct _fr_fd {
	char *file_path;
	int file;
	int eof_flag;
	unsigned char *buffer;
	unsigned int buffer_size;
	unsigned int pos;
	unsigned int inbuf;
} fr_fd;

// creates a new file descriptor and opens the file for reading
fr_fd *fr_new(const char *file_path, unsigned int bsize) {
	if (!file_path) return NULL;
	fr_fd *ret_fd = (fr_fd *)malloc(sizeof(fr_fd));
	if (!ret_fd) return NULL;
	ret_fd->file_path = (char *)malloc(strlen(file_path) + 1);
	if (!ret_fd->file_path) {
		free(ret_fd);
		return NULL;
	}
	ret_fd->file = open(file_path, O_RDONLY);
	if (ret_fd->file < 0) {
		free(ret_fd->file_path);
		free(ret_fd);
		return NULL;
	}
	ret_fd->buffer = (unsigned char *)malloc(bsize);
	if (!ret_fd->buffer) {
		close(ret_fd->file);
		free(ret_fd->file_path);
		free(ret_fd);
		return NULL;
	}

	strcpy(ret_fd->file_path, file_path);
	ret_fd->buffer_size = bsize;
	ret_fd->pos = 0;
	ret_fd->inbuf = 0;
	ret_fd->eof_flag = 0;

	return ret_fd;
}

// returns the next byte or -1 on error
int fr_read(fr_fd *fd) {
	int t;
	if (!fd) return EOF;
	if (fd->eof_flag != 1) {
		if (fd->pos >= fd->inbuf) {
			t = read(fd->file, fd->buffer, fd->buffer_size);
			fd->pos = 0;
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
	if (!fd) return;
	lseek(fd->file, 0, SEEK_SET);
	fd->pos = 0;
	fd->eof_flag = 0;
}

// delete, cleanup, close the file related stuffs
void fr_done(fr_fd *fd) {
	if (!fd) return;
	if (fd->file >= 0) close(fd->file);
	free(fd->buffer);
	free(fd->file_path);
	free(fd);
}

#endif /* FILE_READER_H */

