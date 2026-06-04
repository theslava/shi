/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 *      main.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"
#include "decompress.h"

int main(int argc, char* argv[]) {
    // Check if we have the right number of arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <compress|decompress> <input_file> <output_file>\n", argv[0]);
        fprintf(stderr, "Example: %s compress input.txt output.txt\n", argv[0]);
        fprintf(stderr, "Example: %s decompress input.txt output.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Get the command
    const char* command = argv[1];
    const char* input_file = argv[2];
    const char* output_file = argv[3];

    // Check which command was given
    if (strcmp(command, "compress") == 0) {
        printf("Compressing '%s' to '%s'\n", input_file, output_file);
        compress_file(input_file, output_file);
        return EXIT_SUCCESS;
    }
    else if (strcmp(command, "decompress") == 0) {
        printf("Decompressing '%s' to '%s'\n", input_file, output_file);
        decompress_file(input_file, output_file);
        return EXIT_SUCCESS;
    }
    else {
        fprintf(stderr, "Error: Unknown command '%s'. Use 'compress' or 'decompress'.\n", command);
        return EXIT_FAILURE;
    }
}

