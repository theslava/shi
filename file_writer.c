#include "file_writer.h"

/* creates a new file writer and opens the file for writing */
fr_wd* fw_new(const char* file_path, unsigned int bsize) {
	if (!file_path || !bsize) return NULL;

	fr_wd* ret = (fr_wd*) malloc(sizeof(fr_wd));
	if (!ret) return NULL;

	ret->file_path = (char*) malloc(strlen(file_path) + 1);
	strcpy(ret->file_path, file_path);

	/* Open for writing/truncating; create if doesn't exist */
#ifdef _WIN32
	ret->file = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
#else
	ret->file = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
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
int fw_write_byte(fr_wd *wd, unsigned char byte) {
	if (!wd) return -1;

	/* If buffer is full, flush it first */
	if (wd->pos >= wd->buffer_size) {
		fw_flush(wd);
	}

	wd->buffer[wd->pos++] = byte;
	return 0;
}

// writes an array of bytes to the file
int fw_write_bytes(fr_wd *wd, const unsigned char *data, unsigned int len) {
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
void fw_flush(fr_wd *wd) {
	if (!wd || wd->pos == 0) return;

	write(wd->file, wd->buffer, wd->pos);
	wd->pos = 0;   /* reset position for next batch */
}

// delete, cleanup, close the file writer
void fw_done(fr_wd *wd) {
	if (!wd) return;

	/* Flush any remaining data in buffer */
	fw_flush(wd);

	close(wd->file);
	free(wd->buffer);
	free(wd->file_path);
	free(wd);
}