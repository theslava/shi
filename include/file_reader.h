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

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>

/* --- Reader struct (existing) --- */
typedef struct _fr_fd {
	char* file_path;
	int file;
	int eof_flag;
	unsigned char* buffer;
	unsigned int buffer_size;
	unsigned int pos;	// position within the buffer
	unsigned int inbuf; // actual number of bytes in the buffer
} fr_fd;

/* --- Writer struct (moved to file_writer.h) --- */
typedef struct _fr_wd {
	char* file_path;
	int file;
	unsigned char* buffer;
	unsigned int buffer_size;
	unsigned int pos;	// current write position within buffer
} fr_wd;

/* Reader API */
fr_fd* fr_new(char* file_path, unsigned int bsize);
int fr_read(fr_fd *fd);
void fr_rewind(fr_fd *fd);
void fr_done(fr_fd *fd);
void fr_info(fr_fd *fd);

/* Writer API (moved to file_writer.h) */
// Removed from here
#endif 

