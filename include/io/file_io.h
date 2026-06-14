/* Released to the public domain — No rights reserved. */

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
    unsigned int pos;    // position within the buffer
    unsigned int inbuf;  // actual number of bytes in the buffer
} fr_fd;

/* --- Writer struct --- */
typedef struct _fw_fd {
    char* file_path;
    int file;
    unsigned char* buffer;
    unsigned int buffer_size;
    unsigned int pos;  // current write position within buffer
} fw_fd;

/* Reader API */
fr_fd* fr_new(const char* file_path, unsigned int bsize);
int fr_read(fr_fd* fd);
void fr_rewind(fr_fd* fd);
void fr_done(fr_fd* fd);
void fr_info(fr_fd* fd);

/* Writer API */
fw_fd* fw_new(const char* file_path, unsigned int bsize);
int fw_write_byte(fw_fd* wd, unsigned char byte);
int fw_write_bytes(fw_fd* wd, const unsigned char* data, unsigned int len);
void fw_flush(fw_fd* wd);
void fw_done(fw_fd* wd);

#endif
