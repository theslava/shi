#include <stdio.h>
#include "utils/metric.h"
#include "io/file_io.h"

metric * new_metric() {
	int i = 0;
	metric * met = (metric*)malloc(sizeof(struct _metric));
	if (met == NULL) {
		return NULL;
	}
	while (i < 256) met->characters[i++] = 0;
	return met;
}

metric * new_metric_from_file(fr_fd *file) {
	int i;
	metric * met = new_metric();
	while ((i = fr_read(file)) != EOF) {
		met->characters[i]++;
	}
	return met;
}

void fill_metric(metric* met, fr_fd *file) {
	int i;
	while ((i = fr_read(file)) != EOF) {
		met->characters[i]++;
	}
	return;
}

void delete_metric (metric* met) {
	free(met);
	return;
}