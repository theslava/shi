#ifndef METRIC_H
#define METRIC_H

#include <stdlib.h>
#include <stdio.h>
#include "file_reader.h"

typedef struct _metric {
	long long unsigned int characters[256];
} metric;

static inline metric * new_metric() {
	metric * met = (metric *)malloc(sizeof(metric));
	if (!met) return NULL;
	for (int i = 0; i < 256; i++) {
		met->characters[i] = 0;
	}
	return met;
}

metric * new_metric_from_file(fr_fd *file) {
	if (!file) return NULL;

	metric * met = new_metric();
	if (!met) return NULL;
	int i;
	while ((i = fr_read(file)) != EOF) {
		met->characters[i]++;
	}
	return met;
}

void fill_metric(metric *met, fr_fd *file) {
	if (!met || !file) return;
	int i;
	while ((i = fr_read(file)) != EOF) {
		met->characters[i]++;
	}
}

void delete_metric(metric *met) {
	if (met) free(met);
}

#endif /* METRIC_H */

