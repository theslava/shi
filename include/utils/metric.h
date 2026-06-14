/* Released to the public domain — No rights reserved. */

#ifndef __metric_h__
#define __metric_h__

#include <malloc.h>
#include "io/file_io.h"

typedef struct _metric {
    long long unsigned int characters[256];
} metric;

metric* new_metric(void);
metric* new_metric_from_file(fr_fd* file, unsigned int* file_size_out);
void fill_metric(metric* met, fr_fd* file);
void delete_metric(metric* met);
#endif
