/*
 *      file_writer.h
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

#ifndef __file_writer_h__
#define __file_writer_h__

#include "file_reader.h"

/* Writer API (re-exported from file_reader.h for convenience) */
fr_wd* fw_new(const char* file_path, unsigned int bsize);
int fw_write_byte(fr_wd *wd, unsigned char byte);
int fw_write_bytes(fr_wd *wd, const unsigned char *data, unsigned int len);
void fw_flush(fr_wd *wd);
void fw_done(fr_wd *wd);

#endif /* __file_writer_h__ */
