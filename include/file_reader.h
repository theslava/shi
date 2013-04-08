/*
 *      file_reader.h
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
 
#ifndef __file_reader_h__
#define __file_reader_h__

#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

typedef struct _fr_fd {
	char* file_path;
	int file;
	int eof_flag;
	unsigned char* buffer;
	unsigned int buffer_size;
	unsigned int pos;	// position within the buffer
	unsigned int inbuf; // actual number of bytes in the buffer
} fr_fd;

// creates a new file descriptor and opens the file for reading
fr_fd* fr_new(char* file_path, unsigned int bsize) {
	// new file descriptor, create enough memory for stuff we need
	fr_fd* ret_fd = (fr_fd*) malloc(sizeof(fr_fd));
	ret_fd->file_path = (char*) malloc(sizeof(char)*strlen(file_path) + 1);
	ret_fd->file = open(file_path,O_RDONLY);
	ret_fd->buffer = (unsigned char*) malloc(sizeof(char) * bsize);

	// set up the file descriptor
	strcpy(ret_fd->file_path,file_path);
	ret_fd->buffer_size = bsize;
	ret_fd->pos = 0;
	ret_fd->inbuf = 0;
	ret_fd->eof_flag = 0;

	return ret_fd;
}

// returns the next byte or -1 on error
int fr_read(fr_fd *fd) {
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
	fd->eof_flag = 0;
	return;
}

// delete, cleanup, close the file related stuffs
void fr_done(fr_fd *fd) {
	close(fd->file);
	free(fd->buffer);
	free(fd->file_path);
	free(fd);
	return;
}

// output current info of the file
fr_info(fr_fd *fd) {
	fprintf(stderr, "File path = %s\n", fd->file_path);
	fprintf(stderr, "Descriptor = %d\n", fd->file);
	fprintf(stderr, "Current buffer = \n%s\n", fd->buffer);
	fprintf(stderr, "Buffer size = %d\n", fd->buffer_size);
	fprintf(stderr, "Current position in buffer = %d\n", fd->pos);	// position within the buffer
	fprintf(stderr, "Bytes in buffer = %d\n", fd->inbuf); // actual number of bytes in the buffer
	fprintf(stderr, "EOF flag = %d\n", fd->eof_flag);
}
#endif 
