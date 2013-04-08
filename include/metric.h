/*
 *      metric.h
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

#ifndef __metric_h__
#define __metric_h__

#include <malloc.h>
#include "file_reader.h"

typedef struct _metric {
	long long unsigned int characters[256];
} metric;

metric * new_metric() {
	int i = 0;
	metric * met = (metric*)malloc(sizeof(struct _metric));
	while (i < 256) met->characters[i++] = 0;
	return met;
}

metric * new_metric_from_file(fr_fd *file) {
	unsigned int i;
	metric * met = new_metric();
	while ((i = fr_read(file)) != EOF) {
		met->characters[i]++;
	}
	return met;
}

void fill_metric(metric* met, fr_fd *file) {
	unsigned int i;
	while ((i = fr_read(file)) != EOF) {
		met->characters[i]++;
	}
	return;
}

void delete_metric (metric* met) {
	free(met);
	return;
}

#endif
