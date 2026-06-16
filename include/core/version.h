/* Released to the public domain — No rights reserved. */

#ifndef __version_h__
#define __version_h__

#include "io/file_io.h"

/* ---------------------------------------------------------------------------
 * Version constants
 * --------------------------------------------------------------------------- */

/* Current default version */
#define SHI_CURRENT_VERSION 0

/* Maximum supported version (for bounds checking) */
#define SHI_MAX_VERSION 0

/* ---------------------------------------------------------------------------
 * Magic bytes per version
 *
 * Format: SHI<version_byte>
 * - Bytes 0-2: "SHI" = 0x53 0x48 0x49
 * - Byte 3: version number (distinguishes file format versions)
 *
 * Version 0 magic: 0x53 0x48 0x49 0x00
 * --------------------------------------------------------------------------- */

#define SHI_MAGIC_V0 "\x53\x48\x49\x00"
#define SHI_MAGIC_V0_LEN 4

/* File extension for compressed files */
#define SHI_COMPRESSED_EXT ".shi"

/* Magic byte arrays for direct comparison */
extern const unsigned char shi_magic_v0[4];

/* ---------------------------------------------------------------------------
 * Version detection
 * --------------------------------------------------------------------------- */

/* Read the 4th byte (version byte) from an already-opened file reader.
 * The reader should be positioned at the start of the file.
 * Returns the version number, or -1 on error. */
int shi_detect_version(fr_fd* input_fd);

/* ---------------------------------------------------------------------------
 * Compression entry points per version
 * --------------------------------------------------------------------------- */

/* Compress a file using the specified version format.
 * Returns 0 on success, -1 on error. */
int shi_compress_v0(const char* input_file, const char* output_file);

/* ---------------------------------------------------------------------------
 * Decompression entry points per version
 * --------------------------------------------------------------------------- */

/* Decompress a file compressed with the specified version format.
 * Returns 0 on success, -1 on error. */
int shi_decompress_v0(const char* input_file, const char* output_file);

#endif
