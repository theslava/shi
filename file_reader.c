#include "file_reader.h"

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
void fr_info(fr_fd *fd) {
	fprintf(stderr, "File path = %s\n", fd->file_path);
	fprintf(stderr, "Descriptor = %d\n", fd->file);
	fprintf(stderr, "Current buffer = \n%s\n", fd->buffer);
	fprintf(stderr, "Buffer size = %d\n", fd->buffer_size);
	fprintf(stderr, "Current position in buffer = %d\n", fd->pos);	// position within the buffer
	fprintf(stderr, "Bytes in buffer = %d\n", fd->inbuf); // actual number of bytes in the buffer
	fprintf(stderr, "EOF flag = %d\n", fd->eof_flag);
}