/* "$Header: /usr/people/sam/tiff/libtiff/RCS/mkversion.c,v 1.5 1996/05/21 18:19:48 sam Exp $ */

/*
 * Copyright (c) 1995-1996 Sam Leffler
 * Copyright (c) 1995-1996 Silicon Graphics, Inc.
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

/*
 * Generate a library version string.
 *
 * Syntax: mkversion major minor alpha type [outfile]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char* argv[])
{
    FILE* fd;

    if (argc != 5 && argc != 6) {
	fprintf(stderr, "usage: %s major minor alpha type [outfile]\n",
	    argv[0]);
	exit(-1);
    }
    if (argc == 6) {
	fd = fopen(argv[5], "w");
	if (fd == NULL) {
	    fprintf(stderr, "%s: %s: Could not open for writing.\n",
		argv[0], argv[5]);
	    exit(-1);
	}
    } else
	fd = stdout;
    if (strcmp(argv[4], "beta") == 0)
	fprintf(fd, "#define VERSION \"LIBTIFF, Version %s.%sbeta%s\\n",
	    argv[1], argv[2], argv[3]);
    else
	fprintf(fd, "#define VERSION \"LIBTIFF, Version %s.%s%s (alpha %s)\\n",
	    argv[1], argv[2], argv[4], argv[3]);
    fprintf(fd, "Copyright (c) 1988-1996 Sam Leffler\\n");
    fprintf(fd, "Copyright (c) 1991-1996 Silicon Graphics, Inc.\"\n");

    if (fd != stdout)
	fclose(fd);
    return (0);
}
