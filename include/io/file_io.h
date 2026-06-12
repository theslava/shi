/*
 *      file_io.h
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
 
#ifndef __file_io_h__
#define __file_io_h__

#include <sys/stat.h>
#include <fcntl.h>


#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

/* --- Reader struct --- */
typedef struct _fr_fd {
	char* file_path;
	int file;
	int eof_flag;
	unsigned char* buffer;
	unsigned int buffer_size;
	unsigned int pos;	// position within the buffer
	unsigned int inbuf; // actual number of bytes in the buffer
} fr_fd;

/* --- Writer struct --- */
typedef struct _fw_fd {
	char* file_path;
	int file;
	unsigned char* buffer;
	unsigned int buffer_size;
	unsigned int pos;	// current write position within buffer
} fw_fd;

/* Reader API */
fr_fd* fr_new(char* file_path, unsigned int bsize);
int fr_read(fr_fd *fd);
void fr_rewind(fr_fd *fd);
void fr_done(fr_fd *fd);
void fr_info(fr_fd *fd);

/* Writer API */
fw_fd* fw_new(const char* file_path, unsigned int bsize);
int fw_write_byte(fw_fd *wd, unsigned char byte);
int fw_write_bytes(fw_fd *wd, const unsigned char *data, unsigned int len);
void fw_flush(fw_fd *wd);
void fw_done(fw_fd *wd);

#endif

