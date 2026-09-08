/*
 * Copyright (c) 2026 Can Bolayir
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice and this permission notice appear in
 * all copies of the software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DAMAGES ARISING OUT OF
 * THE USE OF OR INABILITY TO USE THIS SOFTWARE.
 */

/* Regression test for https://gitlab.com/libtiff/libtiff/-/issues/900.
 * Like fax2ps, a custom fax fill function may consume runs without an
 * output buffer. Every row must receive NULL, without pointer arithmetic.
 */

#include "tiffio.h"
#include <stdio.h>
#include <string.h>

static unsigned int filled_rows;
static int fill_failed;

static void check_runs(unsigned char *buf, uint32_t *runs, uint32_t *end,
                       uint32_t width)
{
    uint32_t pixels = 0;
    if (buf != NULL || width != 16)
        fill_failed = 1;
    while (runs < end)
        pixels += *runs++;
    if (pixels != width)
        fill_failed = 1;
    filled_rows++;
}

static int test_codec(uint16_t compression, uint32_t options)
{
    const char *filename = "o-test_fax_fill.tif";
    unsigned char pixels[] = {0x00, 0x00, 0xff, 0xff, 0x55, 0xaa, 0xf0, 0x0f};
    unsigned char decoded[sizeof(pixels)];
    TIFF *tif = TIFFOpen(filename, "w");
    int result = 1;

    if (!tif)
        return 1;
    if (!TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, 16) ||
        !TIFFSetField(tif, TIFFTAG_IMAGELENGTH, 4) ||
        !TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 1) ||
        !TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1) ||
        !TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 4) ||
        !TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG) ||
        !TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE) ||
        !TIFFSetField(tif, TIFFTAG_COMPRESSION, compression))
        goto done;
    if (compression == COMPRESSION_CCITTFAX3 &&
        !TIFFSetField(tif, TIFFTAG_GROUP3OPTIONS, options))
        goto done;
    if (TIFFWriteEncodedStrip(tif, 0, pixels, sizeof(pixels)) != sizeof(pixels))
        goto done;
    TIFFClose(tif);
    tif = NULL;

    /* The default fill function must still advance through real buffers. */
    tif = TIFFOpen(filename, "r");
    if (!tif)
        goto done;
    memset(decoded, 0, sizeof(decoded));
    if (TIFFReadEncodedStrip(tif, 0, decoded, -1) != sizeof(decoded) ||
        memcmp(decoded, pixels, sizeof(pixels)) != 0)
        goto done;
    TIFFClose(tif);
    tif = NULL;

    tif = TIFFOpen(filename, "r");
    if (!tif)
        goto done;
    filled_rows = 0;
    fill_failed = 0;
    if (!TIFFSetField(tif, TIFFTAG_FAXFILLFUNC, check_runs) ||
        TIFFReadEncodedStrip(tif, 0, NULL, -1) != sizeof(pixels) ||
        filled_rows != 4 || fill_failed)
        goto done;
    result = 0;

done:
    if (tif)
        TIFFClose(tif);
    remove(filename);
    if (result)
        fprintf(stderr, "Fax fill failed for compression %u, options %u\n",
                (unsigned int)compression, (unsigned int)options);
    return result;
}

int main(void)
{
    int result = 0;
    if (!TIFFIsCODECConfigured(COMPRESSION_CCITTFAX3))
        return 0;
    result |= test_codec(COMPRESSION_CCITTFAX3, 0);
    result |= test_codec(COMPRESSION_CCITTFAX3, GROUP3OPT_2DENCODING);
    result |= test_codec(COMPRESSION_CCITTFAX4, 0);
    result |= test_codec(COMPRESSION_CCITTRLE, 0);
    result |= test_codec(COMPRESSION_CCITTRLEW, 0);
    return result;
}
