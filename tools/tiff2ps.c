/* $Header: /usr/people/sam/tiff/tools/RCS/tiff2ps.c,v 1.45 1995/06/06 23:45:26 sam Exp $ */

/*
 * Copyright (c) 1988-1995 Sam Leffler
 * Copyright (c) 1991-1995 Silicon Graphics, Inc.
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

#if defined(unix) || defined(__unix)
#include "port.h"
#else
#include <stdio.h>
#include <stdlib.h>			/* for atof */
#endif
#include <math.h>
#include <time.h>

#include "tiffio.h"

/*
 * NB: this code assumes uint32 works with printf's %l[ud].
 */
#define	TRUE	1
#define	FALSE	0

int     ascii85 = FALSE;		/* use ASCII85 encoding */
int	level2 = FALSE;			/* generate PostScript level 2 */
int	printAll = FALSE;		/* print all images in file */
int	generateEPSF = TRUE;		/* generate Encapsulated PostScript */
int 	PSduplex = FALSE;		/* enable duplex printing */
int	PStumble = FALSE;		/* enable top edge binding */
char	*filename;			/* input filename */

int	TIFF2PS(FILE*, TIFF*, float, float);
void	PSpage(FILE*, TIFF*, uint32, uint32);
void	PSColorContigPreamble(FILE*, uint32, uint32, int);
void	PSColorSeparatePreamble(FILE*, uint32, uint32, int);
void	PSDataColorContig(FILE*, TIFF*, uint32, uint32, int);
void	PSDataColorSeparate(FILE*, TIFF*, uint32, uint32, int);
void	PSDataPalette(FILE*, TIFF*, uint32, uint32);
void	PSDataBW(FILE*, TIFF*, uint32, uint32);
void	PSRawDataBW(FILE*, TIFF*, uint32, uint32);
void	Ascii85Init(void);
void	Ascii85Put(unsigned char code, FILE* fd);
void	Ascii85Flush(FILE* fd);
void    PSHead(FILE*, TIFF*, uint32, uint32, float, float, float, float);
void 	PSTail(FILE*, int);

static	void usage(int);

int
main(int argc, char* argv[])
{
	int dirnum = -1, c, np = 0;
	float pageWidth = 0;
	float pageHeight = 0;
	uint32 diroff = 0;
	extern char *optarg;
	extern int optind;
	FILE* output = stdout;

	while ((c = getopt(argc, argv, "h:w:d:o:O:aeps128DT")) != -1)
		switch (c) {
		case 'd':
			dirnum = atoi(optarg);
			break;
	        case 'D':
			PSduplex = TRUE;
			break;
		case 'T':
			PStumble = TRUE;
			break;
		case 'e':
			generateEPSF = TRUE;
			break;
		case 'h':
			pageHeight = atof(optarg);
			break;
		case 'o':
			diroff = (uint32) strtoul(optarg, NULL, 0);
			break;
		case 'O':		/* XXX too bad -o is already taken */
			output = fopen(optarg, "w");
			if (output == NULL) {
				fprintf(stderr,
				    "%s: %s: Cannot open output file.\n",
				    argv[0], optarg);
				exit(-2);
			}
			break;
		case 'a':
			printAll = TRUE;
			/* fall thru... */
		case 'p':
			generateEPSF = FALSE;
			break;
		case 's':
			printAll = FALSE;
			break;
		case 'w':
			pageWidth = atof(optarg);
			break;
		case '1':
			level2 = FALSE;
			ascii85 = FALSE;
			break;
		case '2':
			level2 = TRUE;
			ascii85 = TRUE;			/* default to yes */
			break;
		case '8':
			ascii85 = FALSE;
			break;
		case '?':
			usage(-1);
		}
	for (; argc - optind > 0; optind++) {
		TIFF* tif = TIFFOpen(filename = argv[optind], "r");
		if (tif != NULL) {
			if (dirnum != -1 && !TIFFSetDirectory(tif, dirnum))
				return (-1);
			else if (diroff != 0 &&
			    !TIFFSetSubDirectory(tif, diroff))
				return (-1);
			np = TIFF2PS(output, tif, pageWidth, pageHeight);
			TIFFClose(tif);
		}
	}
	if (np)
	        PSTail(output, np);
	else
	        usage(-1);
	if (output != stdout)
		fclose(output);
	return (0);
}

static	uint16 samplesperpixel;
static	uint16 bitspersample;
static	uint16 planarconfiguration;
static	uint16 photometric;
static	uint16 compression;
static	uint16 extrasamples;
static	int alpha;

static int
checkImage(TIFF* tif)
{
	switch (bitspersample) {
	case 1: case 2:
	case 4: case 8:
		break;
	default:
		TIFFError(filename, "Can not handle %d-bit/sample image",
		    bitspersample);
		return (0);
	}
	switch (photometric) {
	case PHOTOMETRIC_YCBCR:
		if (compression == COMPRESSION_JPEG &&
		    planarconfiguration == PLANARCONFIG_CONTIG) {
			/* can rely on libjpeg to convert to RGB */
			TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE,
				     JPEGCOLORMODE_RGB);
			photometric = PHOTOMETRIC_RGB;
		} else {
			TIFFError(filename,
				  "Can not handle image with PhotometricInterpretation=YCbCr");
			return (0);
		}
		/* fall thru... */
	case PHOTOMETRIC_RGB:
		if (alpha && bitspersample != 8) {
			TIFFError(filename,
			    "Can not handle %d-bit/sample RGB image with alpha",
			    bitspersample);
			return (0);
		}
		/* fall thru... */
	case PHOTOMETRIC_SEPARATED:
	case PHOTOMETRIC_PALETTE:
	case PHOTOMETRIC_MINISBLACK:
	case PHOTOMETRIC_MINISWHITE:
		break;
	default:
		TIFFError(filename,
		    "Can not handle image with PhotometricInterpretation=%d",
		    photometric);
		return (0);
	}
	if (planarconfiguration == PLANARCONFIG_SEPARATE && extrasamples > 0)
		TIFFWarning(filename, "Ignoring extra samples");
	return (1);
}

#define PS_UNIT_SIZE	72.0
#define	PSUNITS(npix,res)	((npix) * (PS_UNIT_SIZE / (res)))

static	char RGBcolorimage[] = "\
/bwproc {\n\
    rgbproc\n\
    dup length 3 idiv string 0 3 0\n\
    5 -1 roll {\n\
	add 2 1 roll 1 sub dup 0 eq {\n\
	    pop 3 idiv\n\
	    3 -1 roll\n\
	    dup 4 -1 roll\n\
	    dup 3 1 roll\n\
	    5 -1 roll put\n\
	    1 add 3 0\n\
	} { 2 1 roll } ifelse\n\
    } forall\n\
    pop pop pop\n\
} def\n\
/colorimage where {pop} {\n\
    /colorimage {pop pop /rgbproc exch def {bwproc} image} bind def\n\
} ifelse\n\
";

/*
 * Adobe Photoshop requires a comment line of the form:
 *
 * %ImageData: <cols> <rows> <depth>  <main channels> <pad channels>
 *	<block size> <1 for binary|2 for hex> "data start"
 *
 * It is claimed to be part of some future revision of the EPS spec.
 */
static void
PhotoshopBanner(FILE* fd, uint32 w, uint32 h, int bs, int nc, char* startline)
{
	fprintf(fd, "%%ImageData: %ld %ld %d %d 0 %d 2 \"",
	    w, h, bitspersample, nc, bs);
	fprintf(fd, startline, nc);
	fprintf(fd, "\"\n");
}

static void
setupPageState(TIFF* tif, uint32* pw, uint32* ph, float* pprw, float* pprh)
{
	uint16 res_unit;
	float xres, yres;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, pw);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, ph);
	TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
	/*
	 * Calculate printable area.
	 */
	if (!TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres))
		xres = PS_UNIT_SIZE;
	if (!TIFFGetField(tif, TIFFTAG_YRESOLUTION, &yres))
		yres = PS_UNIT_SIZE;
	switch (res_unit) {
	case RESUNIT_CENTIMETER:
		xres /= 2.54, yres /= 2.54;
		break;
	case RESUNIT_NONE:
		xres *= PS_UNIT_SIZE, yres *= PS_UNIT_SIZE;
		break;
	}
	*pprh = PSUNITS(*ph, yres);
	*pprw = PSUNITS(*pw, xres);
}

static	tsize_t tf_bytesperrow;
static	tsize_t ps_bytesperrow;
static 	tsize_t	tf_rowsperstrip;
static	tsize_t	tf_numberstrips;
static	char *hex = "0123456789abcdef";


/* returns the sequence number of the page processed */
int
TIFF2PS(FILE* fd, TIFF* tif, float pw, float ph)
{
	uint32 w, h;
	float ox, oy, prw, prh;
	uint32 subfiletype;
	uint16* sampleinfo;
	static int npages = 0;

	if (!TIFFGetField(tif, TIFFTAG_XPOSITION, &ox))
		ox = 0;
	if (!TIFFGetField(tif, TIFFTAG_YPOSITION, &oy))
		oy = 0;
	setupPageState(tif, &w, &h, &prw, &prh);

	do {
	        tf_numberstrips = TIFFNumberOfStrips(tif);
		TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &tf_rowsperstrip);
		setupPageState(tif, &w, &h, &prw, &prh);
		if (!npages)
		        PSHead(fd, tif, w, h, prw, prh, ox, oy);
		tf_bytesperrow = TIFFScanlineSize(tif);
		TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE,
		    &bitspersample);
		TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL,
		    &samplesperpixel);
		TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarconfiguration);
		TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
		TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression);
		TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES,
		    &extrasamples, &sampleinfo);
		alpha = (extrasamples == 1 &&
			 sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA);
		if (checkImage(tif)) {
			npages++;
			fprintf(fd, "%%%%Page: %d %d\n", npages, npages);
			fprintf(fd, "gsave\n");
			fprintf(fd, "100 dict begin\n");
			if (pw != 0 && ph != 0)
				fprintf(fd, "%f %f scale\n",
				    pw*PS_UNIT_SIZE, ph*PS_UNIT_SIZE);
			else
				fprintf(fd, "%f %f scale\n", prw, prh);
			PSpage(fd, tif, w, h);
			fprintf(fd, "end\n");
			fprintf(fd, "grestore\n");
			fprintf(fd, "showpage\n");
		}
		if (generateEPSF)
			break;
		TIFFGetFieldDefaulted(tif, TIFFTAG_SUBFILETYPE, &subfiletype);
	} while (((subfiletype & FILETYPE_PAGE) || printAll) &&
	    TIFFReadDirectory(tif));

	return(npages);
}


static char DuplexPreamble[] = "\
%%BeginFeature: *Duplex True\n\
systemdict begin\n\
  /languagelevel where { pop languagelevel } { 1 } ifelse\n\
  2 ge { 1 dict dup /Duplex true put setpagedevice }\n\
  { statusdict /setduplex known { statusdict begin setduplex true end } if\n\
  } ifelse\n\
end\n\
%%EndFeature\n\
";

static char TumblePreamble[] = "\
%%BeginFeature: *Tumble True\n\
systemdict begin\n\
  /languagelevel where { pop languagelevel } { 1 } ifelse\n\
  2 ge { 1 dict dup /Tumble true put setpagedevice }\n\
  { statusdict /settumble known { statusdict begin settumble true end } if\n\
  } ifelse\n\
end\n\
%%EndFeature\n\
";

void 
PSHead(FILE *fd, TIFF *tif, uint32 w, uint32 h, float pw, float ph, 
	float ox, float oy) 
{
        time_t t;

	(void) tif; (void) w; (void) h;
	t = time(0);
	fprintf(fd, "%%!PS-Adobe-3.0%s\n", generateEPSF ? " EPSF-3.0" : "");
	fprintf(fd, "%%%%Creator: tiff2ps\n");
	fprintf(fd, "%%%%Title: %s\n", filename);
	fprintf(fd, "%%%%CreationDate: %s", ctime(&t));
	fprintf(fd, "%%%%Origin: %ld %ld\n", (long) ox, (long) oy);
	/* NB: should use PageBoundingBox */
	fprintf(fd, "%%%%BoundingBox: 0 0 %ld %ld\n",
	    (long) ceil(pw), (long) ceil(ph));
	fprintf(fd, "%%%%Pages: (atend)\n");
	fprintf(fd, "%%%%EndComments\n");
	fprintf(fd, "%%%%BeginSetup\n");
	if (PSduplex)
	        fprintf(fd, "%s", DuplexPreamble);
	if (PStumble)
	        fprintf(fd, "%s", TumblePreamble);
	fprintf(fd, "%%%%EndSetup\n");
}

void
PSTail(FILE *fd, int npages)
{
        fprintf(fd, "%%%%Pages: %d\n", npages);
	fprintf(fd, "%%%%Trailer\n%%%%EOF\n");
}

static int
emitPSLevel2FilterFunction(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	uint32 group3opts;
	int K;

#define	P(a,b)	(((a)<<4)|((b)&0xf))
	switch (P(compression, photometric)) {
	case P(COMPRESSION_CCITTRLE, PHOTOMETRIC_MINISBLACK):
	case P(COMPRESSION_CCITTRLE, PHOTOMETRIC_MINISWHITE):
		K = 0;
		break;
	case P(COMPRESSION_CCITTFAX3, PHOTOMETRIC_MINISBLACK):
	case P(COMPRESSION_CCITTFAX3, PHOTOMETRIC_MINISWHITE):
		TIFFGetField(tif, TIFFTAG_GROUP3OPTIONS, &group3opts);
		K = group3opts&GROUP3OPT_2DENCODING;
		break;
	case P(COMPRESSION_CCITTFAX4, PHOTOMETRIC_MINISBLACK):
	case P(COMPRESSION_CCITTFAX4, PHOTOMETRIC_MINISWHITE):
		K = -1;
		break;
	case P(COMPRESSION_LZW, PHOTOMETRIC_MINISBLACK):
		fprintf(fd, "    /LZWDecode filter\n");
		return (TRUE);
	default:
		return (FALSE);
	}
#undef P
	fprintf(fd, "    <<");
	fprintf(fd, "/K %d", K);
	fprintf(fd, " /Columns %d /Rows %d", w, h);
	fprintf(fd, " /EndOfBlock false /BlackIs1 %s",
	    (photometric == PHOTOMETRIC_MINISBLACK) ? "true" : "false");
	fprintf(fd, ">>\n    /CCITTFaxDecode filter\n");
	return (TRUE);
}

void
PSpage(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	ps_bytesperrow = tf_bytesperrow;
	switch (photometric) {
	case PHOTOMETRIC_RGB:
		if (planarconfiguration == PLANARCONFIG_CONTIG) {
			fprintf(fd, "%s", RGBcolorimage);
			PSColorContigPreamble(fd, w, h, 3);
			PSDataColorContig(fd, tif, w, h, 3);
		} else {
			PSColorSeparatePreamble(fd, w, h, 3);
			PSDataColorSeparate(fd, tif, w, h, 3);
		}
		break;
	case PHOTOMETRIC_SEPARATED:
		/* XXX should emit CMYKcolorimage */
		if (planarconfiguration == PLANARCONFIG_CONTIG) {
			PSColorContigPreamble(fd, w, h, 4);
			PSDataColorContig(fd, tif, w, h, 4);
		} else {
			PSColorSeparatePreamble(fd, w, h, 4);
			PSDataColorSeparate(fd, tif, w, h, 4);
		}
		break;
	case PHOTOMETRIC_PALETTE:
		fprintf(fd, "%s", RGBcolorimage);
		PhotoshopBanner(fd, w, h, 1, 3, "false 3 colorimage");
		fprintf(fd, "/scanLine %d string def\n", ps_bytesperrow);
		fprintf(fd, "%lu %lu 8\n", w, h);
		fprintf(fd, "[%lu 0 0 -%lu 0 %lu]\n", w, h, h);
		fprintf(fd, "{currentfile scanLine readhexstring pop} bind\n");
		fprintf(fd, "false 3 colorimage\n");
		PSDataPalette(fd, tif, w, h);
		break;
	case PHOTOMETRIC_MINISBLACK:
	case PHOTOMETRIC_MINISWHITE:
		if (level2) {
			int rawdata;
			fprintf(fd, "{ %lu { %lu %lu %d\n", tf_numberstrips, 
				w, tf_rowsperstrip, bitspersample);
			fprintf(fd, "    [%lu 0 0 -%lu 0 %lu]\n", w, h, h);
			fprintf(fd, "    currentfile /ASCII%sDecode filter\n",
			    ascii85 ? "85" : "Hex");
			rawdata = emitPSLevel2FilterFunction(fd, tif, w, h);
			fprintf(fd, "    image\n");
			fprintf(fd, "    0 -%f translate\n", 
				(float)tf_rowsperstrip/(float)h);
			fprintf(fd, "  } repeat\n}\n");
			PhotoshopBanner(fd, w, h, 1, 1, "image");
			fprintf(fd, "%%%%BeginData\nexec\n");
			if (ascii85) {
				if (rawdata)
					PSRawDataBW(fd, tif, w, tf_rowsperstrip);
				else 
					PSDataBW(fd, tif, w, h);
			} else {
				if (rawdata)
					PSRawDataBW(fd, tif, w, h);
				else
					PSDataBW(fd, tif, w, h);
			}
			fprintf(fd, "%%%%EndData\n");
		} else {
		        PhotoshopBanner(fd, w, h, 1, 1, "image");
			fprintf(fd, "/scanLine %d string def\n",ps_bytesperrow);
			fprintf(fd, "%lu %lu %d\n", w, h, bitspersample);
			fprintf(fd, "[%lu 0 0 -%lu 0 %lu]\n", w, h, h);
			fprintf(fd,
			    "{currentfile scanLine readhexstring pop} bind\n");
			fprintf(fd, "image\n");
			PSDataBW(fd, tif, w, h);
		}
		break;
	}
	putc('\n', fd);
}

void
PSColorContigPreamble(FILE* fd, uint32 w, uint32 h, int nc)
{
	ps_bytesperrow = nc * (tf_bytesperrow / samplesperpixel);
	PhotoshopBanner(fd, w, h, 1, nc, "false %d colorimage");
	fprintf(fd, "/line %d string def\n", ps_bytesperrow);
	fprintf(fd, "%lu %lu %d\n", w, h, bitspersample);
	fprintf(fd, "[%lu 0 0 -%lu 0 %lu]\n", w, h, h);
	fprintf(fd, "{currentfile line readhexstring pop} bind\n");
	fprintf(fd, "false %d colorimage\n", nc);
}

void
PSColorSeparatePreamble(FILE* fd, uint32 w, uint32 h, int nc)
{
	int i;

	PhotoshopBanner(fd, w, h, ps_bytesperrow, nc, "true %d colorimage");
	for (i = 0; i < nc; i++)
		fprintf(fd, "/line%d %d string def\n", i, ps_bytesperrow);
	fprintf(fd, "%lu %lu %d\n", w, h, bitspersample);
	fprintf(fd, "[%lu 0 0 -%lu 0 %lu] \n", w, h, h);
	for (i = 0; i < nc; i++)
		fprintf(fd, "{currentfile line%d readhexstring pop}bind\n", i);
	fprintf(fd, "true %d colorimage\n", nc);
}

#define MAXLINE		36
#define	DOBREAK(len, howmany, fd) \
	if (((len) -= (howmany)) <= 0) {	\
		putc('\n', fd);			\
		(len) = MAXLINE-(howmany);	\
	}
#define	PUTHEX(c,fd)	putc(hex[((c)>>4)&0xf],fd); putc(hex[(c)&0xf],fd)

void
PSDataColorContig(FILE* fd, TIFF* tif, uint32 w, uint32 h, int nc)
{
	uint32 row;
	int breaklen = MAXLINE, cc, es = samplesperpixel - nc;
	unsigned char *tf_buf;
	unsigned char *cp, c;

	(void) w;
	tf_buf = (unsigned char *) _TIFFmalloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for scanline buffer");
		return;
	}
	for (row = 0; row < h; row++) {
		if (TIFFReadScanline(tif, tf_buf, row, 0) < 0)
			break;
		cp = tf_buf;
		if (alpha) {
			int adjust;
			cc = 0;
			for (; cc < tf_bytesperrow; cc += samplesperpixel) {
				DOBREAK(breaklen, nc, fd);
				/*
				 * For images with alpha, matte against
				 * a white background; i.e.
				 *    Cback * (1 - Aimage)
				 * where Cback = 1.
				 */
				adjust = 255 - cp[nc];
				switch (nc) {
				case 4: c = *cp++ + adjust; PUTHEX(c,fd);
				case 3: c = *cp++ + adjust; PUTHEX(c,fd);
				case 2: c = *cp++ + adjust; PUTHEX(c,fd);
				case 1: c = *cp++ + adjust; PUTHEX(c,fd);
				}
				cp += es;
			}
		} else {
			cc = 0;
			for (; cc < tf_bytesperrow; cc += samplesperpixel) {
				DOBREAK(breaklen, nc, fd);
				switch (nc) {
				case 4: c = *cp++; PUTHEX(c,fd);
				case 3: c = *cp++; PUTHEX(c,fd);
				case 2: c = *cp++; PUTHEX(c,fd);
				case 1: c = *cp++; PUTHEX(c,fd);
				}
				cp += es;
			}
		}
	}
	_TIFFfree((char *) tf_buf);
}

void
PSDataColorSeparate(FILE* fd, TIFF* tif, uint32 w, uint32 h, int nc)
{
	uint32 row;
	int breaklen = MAXLINE, cc, s, maxs;
	unsigned char *tf_buf;
	unsigned char *cp, c;

	(void) w;
	tf_buf = (unsigned char *) _TIFFmalloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for scanline buffer");
		return;
	}
	maxs = (samplesperpixel > nc ? nc : samplesperpixel);
	for (row = 0; row < h; row++) {
		for (s = 0; s < maxs; s++) {
			if (TIFFReadScanline(tif, tf_buf, row, s) < 0)
				break;
			for (cp = tf_buf, cc = 0; cc < tf_bytesperrow; cc++) {
				DOBREAK(breaklen, 1, fd);
				c = *cp++;
				PUTHEX(c,fd);
			}
		}
	}
	_TIFFfree((char *) tf_buf);
}

#define	PUTRGBHEX(c,fd) \
	PUTHEX(rmap[c],fd); PUTHEX(gmap[c],fd); PUTHEX(bmap[c],fd)

static int
checkcmap(TIFF* tif, int n, uint16* r, uint16* g, uint16* b)
{
	(void) tif;
	while (n-- > 0)
		if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
			return (16);
	TIFFWarning(filename, "Assuming 8-bit colormap");
	return (8);
}

void
PSDataPalette(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	uint16 *rmap, *gmap, *bmap;
	uint32 row;
	int breaklen = MAXLINE, cc, nc;
	unsigned char *tf_buf;
	unsigned char *cp, c;

	(void) w;
	if (!TIFFGetField(tif, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap)) {
		TIFFError(filename, "Palette image w/o \"Colormap\" tag");
		return;
	}
	switch (bitspersample) {
	case 8:	case 4: case 2: case 1:
		break;
	default:
		TIFFError(filename, "Depth %d not supported", bitspersample);
		return;
	}
	nc = 3 * (8 / bitspersample);
	tf_buf = (unsigned char *) _TIFFmalloc(tf_bytesperrow);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for scanline buffer");
		return;
	}
	if (checkcmap(tif, 1<<bitspersample, rmap, gmap, bmap) == 16) {
		int i;
#define	CVT(x)		(((x) * 255L) / ((1L<<16)-1))
		for (i = (1<<bitspersample)-1; i >= 0; i--) {
			rmap[i] = CVT(rmap[i]);
			gmap[i] = CVT(gmap[i]);
			bmap[i] = CVT(bmap[i]);
		}
#undef CVT
	}
	for (row = 0; row < h; row++) {
		if (TIFFReadScanline(tif, tf_buf, row, 0) < 0)
			break;
		for (cp = tf_buf, cc = 0; cc < tf_bytesperrow; cc++) {
			DOBREAK(breaklen, nc, fd);
			switch (bitspersample) {
			case 8:
				c = *cp++; PUTRGBHEX(c, fd);
				break;
			case 4:
				c = *cp++; PUTRGBHEX(c&0xf, fd);
				c >>= 4;   PUTRGBHEX(c, fd);
				break;
			case 2:
				c = *cp++; PUTRGBHEX(c&0x3, fd);
				c >>= 2;   PUTRGBHEX(c&0x3, fd);
				c >>= 2;   PUTRGBHEX(c&0x3, fd);
				c >>= 2;   PUTRGBHEX(c, fd);
				break;
			case 1:
				c = *cp++; PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c&0x1, fd);
				c >>= 1;   PUTRGBHEX(c, fd);
				break;
			}
		}
	}
	_TIFFfree((char *) tf_buf);
}

void
PSDataBW(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	int breaklen = MAXLINE;
	unsigned char* tf_buf;
	unsigned char* cp;
	tsize_t stripsize = TIFFStripSize(tif);
	tstrip_t s;

	(void) w; (void) h;
	tf_buf = (unsigned char *) _TIFFmalloc(stripsize);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for scanline buffer");
		return;
	}
	if (ascii85)
	        Ascii85Init();
	for (s = 0; s < TIFFNumberOfStrips(tif); s++) {
		int cc = TIFFReadEncodedStrip(tif, s, tf_buf, stripsize);
		if (cc < 0) {
			TIFFError(filename, "Can't read strip");
			break;
		}
		cp = tf_buf;
		if (photometric == PHOTOMETRIC_MINISWHITE) {
			for (cp += cc; --cp >= tf_buf;)
				*cp = ~*cp;
			cp++;
		}
		if (ascii85) {
			while (cc-- > 0)
				Ascii85Put(*cp++, fd);
		} else {
			while (cc-- > 0) {
				unsigned char c = *cp++;
				DOBREAK(breaklen, 1, fd);
				PUTHEX(c, fd);
			}
		}
	}
	if (ascii85)
	        Ascii85Flush(fd);
	else if (level2)
	        fputs(">\n", fd);
	_TIFFfree(tf_buf);
}

void
PSRawDataBW(FILE* fd, TIFF* tif, uint32 w, uint32 h)
{
	uint32 *bc;
	uint32 bufsize;
	int breaklen = MAXLINE, cc;
	uint16 fillorder;
	unsigned char *tf_buf;
	unsigned char *cp, c;
	tstrip_t s;

	(void) w; (void) h;
	TIFFGetField(tif, TIFFTAG_FILLORDER, &fillorder);
	TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &bc);
	bufsize = bc[0];
	tf_buf = (unsigned char*) _TIFFmalloc(bufsize);
	if (tf_buf == NULL) {
		TIFFError(filename, "No space for strip buffer");
		return;
	}
	for (s = 0; s < tf_numberstrips; s++) {
		if (bc[s] > bufsize) {
			tf_buf = (unsigned char *) _TIFFrealloc(tf_buf, bc[s]);
			if (tf_buf == NULL) {
				TIFFError(filename,
				    "No space for strip buffer");
				return;
			}
			bufsize = bc[s];
		}
		cc = TIFFReadRawStrip(tif, s, tf_buf, bc[s]);
		if (cc < 0) {
			TIFFError(filename, "Can't read strip");
			break;
		}
		if (fillorder == FILLORDER_LSB2MSB)
			TIFFReverseBits(tf_buf, cc);
		if (!ascii85) {
			for (cp = tf_buf; cc > 0; cc--) {
				DOBREAK(breaklen, 1, fd);
				c = *cp++;
				PUTHEX(c, fd);
			}
			fputs(">\n", fd);
			breaklen = MAXLINE;
		} else {
		        Ascii85Init();
			for (cp = tf_buf; cc > 0; cc--)
				Ascii85Put(*cp++, fd);
			Ascii85Flush(fd);
		}
	}
	_TIFFfree((char *) tf_buf);
}

/*
 * ASCII85 Encoding Support.
 */
unsigned char ascii85buf[10];
int	ascii85count;
int	ascii85breaklen;

void
Ascii85Init(void)
{
	ascii85breaklen = 2*MAXLINE;
	ascii85count = 0;
}

static char*
Ascii85Encode(unsigned char* raw)
{
	static char encoded[6];
	uint32 word;

	word = (((raw[0]<<8)+raw[1])<<16) + (raw[2]<<8) + raw[3];
	if (word != 0L) {
		uint32 q;
		uint16 w1;

		q = word / (85L*85*85*85);	/* actually only a byte */
		encoded[0] = q + '!';

		word -= q * (85L*85*85*85); q = word / (85L*85*85);
		encoded[1] = q + '!';

		word -= q * (85L*85*85); q = word / (85*85);
		encoded[2] = q + '!';

		w1 = (uint16) (word - q*(85L*85));
		encoded[3] = (w1 / 85) + '!';
		encoded[4] = (w1 % 85) + '!';
		encoded[5] = '\0';
	} else
		encoded[0] = 'z', encoded[1] = '\0';
	return (encoded);
}

void
Ascii85Put(unsigned char code, FILE* fd)
{
	ascii85buf[ascii85count++] = code;
	if (ascii85count >= 4) {
		unsigned char* p;
		int n;

		for (n = ascii85count, p = ascii85buf; n >= 4; n -= 4, p += 4) {
			char* cp;
			for (cp = Ascii85Encode(p); *cp; cp++) {
				putc(*cp, fd);
				if (--ascii85breaklen == 0) {
					putc('\n', fd);
					ascii85breaklen = 2*MAXLINE;
				}
			}
			p += 4;
		}
		_TIFFmemcpy(ascii85buf, p, n);
		ascii85count = n;
	}
}

void
Ascii85Flush(FILE* fd)
{
	if (ascii85count > 0) {
		char* res;
		_TIFFmemset(&ascii85buf[ascii85count], 0, 3);
		res = Ascii85Encode(ascii85buf);
		fwrite(res[0] == 'z' ? "!!!!" : res, ascii85count + 1, 1, fd);
	}
	fputs("~>\n", fd);
}

char* stuff[] = {
"usage: tiff2ps [options] input.tif ...",
"where options are:",
" -1            generate PostScript Level I (default)",
" -2            generate PostScript Level II",
" -8            disable use of ASCII85 encoding with PostScript Level II",
" -d #          convert directory number #",
" -D            enable duplex printing (two pages per sheet of paper)",
" -e            generate Encapsulated PostScript (EPS)",
" -h #          assume printed page height is # inches (default 11)",
" -o #          convert directory at file offset #",
" -O file       write PostScript to file instead of standard output",
" -a            convert all directories in file (default is first)",
" -p            generate regular PostScript",
" -s            generate PostScript for a single image",
" -T            print pages for top edge binding",
" -w #          assume printed page width is # inches (default 8.5)",
NULL
};

static void
usage(int code)
{
	char buf[BUFSIZ];
	int i;

	setbuf(stderr, buf);
	for (i = 0; stuff[i] != NULL; i++)
		fprintf(stderr, "%s\n", stuff[i]);
	exit(code);
}
