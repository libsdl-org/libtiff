/* $Header: /usr/people/sam/tiff/libtiff/RCS/tif_zip.c,v 1.4 1995/07/17 14:38:12 sam Exp $ */

/*
 * Copyright (c) 1995 Sam Leffler
 * Copyright (c) 1995 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#include "tiffiop.h"
#ifdef ZIP_SUPPORT
/*
 * TIFF Library.
 *
 * ZIP (i.e. Deflate) Compression Support
 *
 * This file is simply an interface to the libgz library written by
 * Jean-loup Gailly and Mark Adler.  Use version 0.92 or later of
 * the library.  The data format used by the zlib library is described
 * in the files zlib-3.1.doc, deflate-1.1.doc and gzip-4.1.doc, available
 * in the directory ftp://ftp.uu.net/pub/archiving/zip/doc.  The library
 * was last found at ftp://ftp.uu.net/graphics/png/code/zlib-0.93.tar.gz.
 */
#include "tif_predict.h"
#include "zlib.h"
#include "zutil.h"

#include <stdio.h>
#include <assert.h>

/*
 * State block for each open TIFF
 * file using ZIP compression/decompression.
 */
typedef	struct {
	TIFFPredictorState	predict;
	z_stream		stream;
} ZIPState;

#define	DecoderState(tif)	((ZIPState*) (tif)->tif_data)
#define	EncoderState(tif)	((ZIPState*) (tif)->tif_data)

static	int ZIPEncode(TIFF*, tidata_t, tsize_t, tsample_t);
static	int ZIPDecode(TIFF*, tidata_t, tsize_t, tsample_t);

static int
ZIPSetupDecode(TIFF* tif)
{
	ZIPState* sp = DecoderState(tif);
	static char module[] = "ZIPSetupDecode";

	assert(sp != NULL);
	if (inflateInit2(&sp->stream, -DEF_WBITS) != Z_OK) {
		TIFFError(module, "%s: %s", tif->tif_name, sp->stream.msg);
		return (0);
	} else
		return (1);
}

/*
 * Setup state for decoding a strip.
 */
static int
ZIPPreDecode(TIFF* tif, tsample_t s)
{
	ZIPState* sp = DecoderState(tif);

	(void) s;
	assert(sp != NULL);
	sp->stream.next_in = tif->tif_rawdata;
	sp->stream.avail_in = tif->tif_rawcc;
	return (inflateReset(&sp->stream) == Z_OK);
}

static int
ZIPDecode(TIFF* tif, tidata_t op, tsize_t occ, tsample_t s)
{
	ZIPState* sp = DecoderState(tif);
	static char module[] = "ZIPDecode";

	(void) s;
	assert(sp != NULL);
	sp->stream.next_out = op;
	sp->stream.avail_out = occ;
	do {
		int state = inflate(&sp->stream, Z_PARTIAL_FLUSH);
		if (state == Z_STREAM_END)
			break;
		if (state == Z_DATA_ERROR) {
			TIFFError(module,
			    "%s: Decoding error at scanline %d, %s",
			    tif->tif_name, tif->tif_row, sp->stream.msg);
			if (inflateSync(&sp->stream) != Z_OK)
				return (0);
			continue;
		}
		if (state != Z_OK) {
			TIFFError(module, "%s: libgz error: %s",
			    tif->tif_name, sp->stream.msg);
			return (0);
		}
	} while (sp->stream.avail_out > 0);
	if (sp->stream.avail_out != 0) {
		TIFFError(module,
		    "%s: Not enough data at scanline %d (short %d bytes)",
		    tif->tif_name, tif->tif_row, sp->stream.avail_out);
		return (0);
	}
	return (1);
}

static int
ZIPSetupEncode(TIFF* tif)
{
	ZIPState* sp = EncoderState(tif);
	static char module[] = "ZIPSetupEncode";

	assert(sp != NULL);
	/*
	 * We use the undocumented feature of a negiatve window
	 * bits to suppress writing the header in the output
	 * stream.  This is necessary when the resulting image
	 * is made up of multiple strips or tiles as otherwise
	 * libgz will not write a header for each strip/tile and
	 * the decoder will fail.
	 */
	if (deflateInit2(&sp->stream, Z_DEFAULT_COMPRESSION,
	    DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, 0) != Z_OK) {
		TIFFError(module, "%s: %s", tif->tif_name, sp->stream.msg);
		return (0);
	} else
		return (1);
}

/*
 * Reset encoding state at the start of a strip.
 */
static int
ZIPPreEncode(TIFF* tif, tsample_t s)
{
	ZIPState *sp = EncoderState(tif);

	(void) s;
	assert(sp != NULL);
	sp->stream.next_out = tif->tif_rawdata;
	sp->stream.avail_out = tif->tif_rawdatasize;
	return (deflateReset(&sp->stream) == Z_OK);
}

/*
 * Encode a chunk of pixels.
 */
static int
ZIPEncode(TIFF* tif, tidata_t bp, tsize_t cc, tsample_t s)
{
	ZIPState *sp = EncoderState(tif);
	static char module[] = "ZIPEncode";

	(void) s;
	sp->stream.next_in = bp;
	sp->stream.avail_in = cc;
	do {
		if (deflate(&sp->stream, Z_NO_FLUSH) != Z_OK) {
			TIFFError(module, "%s: Encoder error: %s",
			    tif->tif_name, sp->stream.msg);
			return (0);
		}
		if (sp->stream.avail_out == 0) {
			tif->tif_rawcc = tif->tif_rawdatasize;
			TIFFFlushData1(tif);
			sp->stream.next_out = tif->tif_rawdata;
			sp->stream.avail_out = tif->tif_rawdatasize;
		}
	} while (sp->stream.avail_in > 0);
	return (1);
}

/*
 * Finish off an encoded strip by flushing the last
 * string and tacking on an End Of Information code.
 */
static int
ZIPPostEncode(TIFF* tif)
{
	ZIPState *sp = EncoderState(tif);
	static char module[] = "ZIPPostEncode";
	int state;

	sp->stream.avail_in = 0;
	do {
		state = deflate(&sp->stream, Z_FINISH);
		switch (state) {
		case Z_STREAM_END:
		case Z_OK:
		    if (sp->stream.avail_out != tif->tif_rawdatasize) {
			    tif->tif_rawcc =
				tif->tif_rawdatasize - sp->stream.avail_out;
			    TIFFFlushData1(tif);
			    sp->stream.next_out = tif->tif_rawdata;
			    sp->stream.avail_out = tif->tif_rawdatasize;
		    }
		    break;
		default:
		    TIFFError(module, "%s: libgz error: %s",
			tif->tif_name, sp->stream.msg);
		    return (0);
		}
	} while (state != Z_STREAM_END);
	return (1);
}

static void
ZIPCleanup(TIFF* tif)
{
	ZIPState* sp = (ZIPState*) tif->tif_data;
	if (sp) {
		if (tif->tif_mode == O_RDONLY)
			inflateEnd(&sp->stream);
		else
			deflateEnd(&sp->stream);
		_TIFFfree(sp);
		tif->tif_data = NULL;
	}
}

int
TIFFInitZIP(TIFF* tif, int scheme)
{
	ZIPState* sp;

	assert(scheme == COMPRESSION_DEFLATE);

	/*
	 * Allocate state block so tag methods have storage to record values.
	 */
	tif->tif_data = (tidata_t) _TIFFmalloc(sizeof (ZIPState));
	if (tif->tif_data == NULL)
		goto bad;
	sp = (ZIPState*) tif->tif_data;
	sp->stream.zalloc = NULL;
	sp->stream.zfree = NULL;
	sp->stream.opaque = NULL;
	sp->stream.data_type = Z_BINARY;

	/*
	 * Install codec methods.
	 */
	tif->tif_setupdecode = ZIPSetupDecode;
	tif->tif_predecode = ZIPPreDecode;
	tif->tif_decoderow = ZIPDecode;
	tif->tif_decodestrip = ZIPDecode;
	tif->tif_decodetile = ZIPDecode;
	tif->tif_setupencode = ZIPSetupEncode;
	tif->tif_preencode = ZIPPreEncode;
	tif->tif_postencode = ZIPPostEncode;
	tif->tif_encoderow = ZIPEncode;
	tif->tif_encodestrip = ZIPEncode;
	tif->tif_encodetile = ZIPEncode;
	tif->tif_cleanup = ZIPCleanup;
	/*
	 * Setup predictor setup.
	 */
	(void) TIFFPredictorInit(tif);
	return (1);
bad:
	TIFFError("TIFFInitZIP", "No space for ZIP state block");
	return (0);
}
#endif /* ZIP_SUPORT */
