/* $Header: /usr/people/sam/tiff/libtiff/RCS/tiffcomp.h,v 1.44 1995/06/06 17:00:43 sam Exp $ */

/*
 * Copyright (c) 1990-1995 Sam Leffler
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

#ifndef _COMPAT_
#define	_COMPAT_
/*
 * This file contains a hodgepodge of definitions and
 * declarations that are needed to provide compatibility
 * between the native system and the base implementation
 * that the library assumes.
 *
 * NB: This file is a mess.
 */

/*
 * Setup basic type definitions and function declaratations.
 */
#if defined(__MWERKS__) || defined(THINK_C)
#include <unix.h>
#include <math.h>
#endif
#include <stdio.h>
#ifdef applec
#include <types.h>
#else
#if !defined(__MWERKS__) && !defined(THINK_C)
#include <sys/types.h>
#endif
#endif
#ifdef VMS
#include <file.h>
#include <unixio.h>
#else
#include <fcntl.h>
#endif
#if defined(__MWERKS__) || defined(THINK_C) || defined(applec)
#include <stdlib.h>
#define	BSDTYPES
#endif
#if defined(_WINDOWS) || defined(__WIN32__) || defined(_Windows)
#define	BSDTYPES
#endif
#if defined(OS2_16) || defined(OS2_32)
#define	BSDTYPES
#endif

/*
 * The library uses the ANSI C/POSIX SEEK_*
 * definitions that should be defined in unistd.h
 * (except on VMS or the Mac, where they are in stdio.h and
 * there is no unistd.h).
 */
#ifndef SEEK_SET
#if !defined(VMS) && !defined (applec) && !defined(THINK_C) && !defined(__MWERKS__)
#include <unistd.h>
#endif
#endif

/*
 * The library uses memset, memcpy, and memcmp.
 * ANSI C and System V define these in string.h.
 */
#include <string.h>

/*
 * The BSD typedefs are used throughout the library.
 * If your system doesn't have them in <sys/types.h>,
 * then define BSDTYPES in your Makefile.
 */
#ifdef BSDTYPES
typedef	unsigned char u_char;
typedef	unsigned short u_short;
typedef	unsigned int u_int;
typedef	unsigned long u_long;
#endif

/*
 * dblparam_t is the type that a double precision
 * floating point value will have on the parameter
 * stack (when coerced by the compiler).
 */
#ifdef applec
typedef extended dblparam_t;
#else
typedef double dblparam_t;
#endif

/*
 * If your compiler supports inline functions, then
 * set INLINE appropriately to get the known hotspots
 * in the library expanded inline.
 */
#ifdef __GNUC__
#ifdef __STRICT_ANSI__
#define	INLINE	__inline__
#else
#define	INLINE	inline
#endif
#else /* !__GNUC__ */
#define	INLINE
#endif

/*
 * GLOBALDATA is a macro that is used to define global variables
 * private to the library.  We use this indirection to hide
 * brain-damage in VAXC (and GCC) under VAX/VMS.  In these
 * environments the macro places the variable in a non-shareable
 * program section, which ought to be done by default (sigh!)
 *
 * Apparently DEC are aware of the problem as this behaviour is the
 * default under VMS on AXP.
 *
 * The GNU C variant is untested.
 */
#if defined(VAX) && defined(VMS)
#ifdef VAXC
#define GLOBALDATA(TYPE,NAME)	extern noshare TYPE NAME
#endif
#ifdef __GNUC__
#define GLOBALDATA(TYPE,NAME)	extern TYPE NAME \
	asm("_$$PsectAttributes_NOSHR$$" #NAME)
#endif
#else	/* !VAX/VMS */
#define GLOBALDATA(TYPE,NAME)	extern TYPE NAME
#endif
#endif /* _COMPAT_ */
