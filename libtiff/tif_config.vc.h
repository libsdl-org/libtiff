#ifndef _TIF_CONFIG_H_
#define _TIF_CONFIG_H_

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define as 0 or 1 according to the floating point format supported by the
   machine */
#define HAVE_IEEEFP 1

/* Define to 1 if you have the `jbg_newlen' function. */
#define HAVE_JBG_NEWLEN 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <io.h> header file. */
#define HAVE_IO_H 1

/* Define to 1 if you have the `setmode' function. */
#define HAVE_SETMODE 1

/* Define to 1 if you have the declaration of `optarg', and to 0 if you don't. */
#define HAVE_DECL_OPTARG 0

#if _WIN64
/*
  Windows 64-bit build
*/

/* The size of `size_t', as computed by sizeof. */
#  define SIZEOF_SIZE_T 8

/* Signed size type formatter */
#  define TIFF_SSIZE_FORMAT PRId64

/* Signed size type */
#  define TIFF_SSIZE_T int64_t

#else
/*
  Windows 32-bit build
*/

/* The size of `size_t', as computed by sizeof. */
#  define SIZEOF_SIZE_T 4

/* Signed size type formatter */
#  define TIFF_SSIZE_FORMAT PRId32

/* Signed size type */
#  define TIFF_SSIZE_T int32_t

#endif

/* Set the native cpu bit order */
#define HOST_FILLORDER FILLORDER_LSB2MSB

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

#define lfind _lfind

#pragma warning(disable : 4996) /* function deprecation warnings */

#endif /* _TIF_CONFIG_H_ */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 8
 * fill-column: 78
 * End:
 */
