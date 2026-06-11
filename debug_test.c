#include <stdio.h>
#include <stdlib.h>
#include "core/compress.h"
#include "core/decompress.h"

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("Creating input file...\n");
    FILE *fp = fopen("debug_input.txt", "wb");
    if (fp) {
        fprintf(fp, "Hello, world!");
        fclose(fp);
    }
    printf("Done.\n");

    printf("Compressing...\n");
    int result = compress_file("debug_input.txt", "debug_compressed.bin");
    printf("Compress result: %d\n", result);

    printf("Decompressing...\n");
    printf("About to call decompress_file...\n");
    result = decompress_file("debug_compressed.bin", "debug_output.txt");
    printf("After decompress_file, result=%d\n", result);
    printf("Decompress result: %d\n", result);

    return 0;
}