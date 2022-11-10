/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
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
 * TIFF Library Win32-specific Routines.  Adapted from tif_unix.c 4/5/95 by
 * Scott Wagner (wagner@itek.com), Itek Graphix, Rochester, NY USA
 */

#include "tiffiop.h"
#include <stdlib.h>

#include <windows.h>

/*
  CreateFileA/CreateFileW return type 'HANDLE' while TIFFFdOpen() takes 'int',
  which is formally incompatible and can even seemingly be of different size:
  HANDLE is 64 bit under Win64, while int is still 32 bits there.

  However, only the lower 32 bits of a HANDLE are significant under Win64 as,
  for interoperability reasons, they must have the same values in 32- and
  64-bit programs running on the same system, see

  https://docs.microsoft.com/en-us/windows/win32/winprog64/interprocess-communication

  Because of this, it is safe to define the following trivial functions for
  casting between ints and HANDLEs, which are only really needed to avoid
  compiler warnings (and, perhaps, to make the code slightly more clear).
  Note that using the intermediate cast to "intptr_t" is crucial for warning
  avoidance, as this integer type has the same size as HANDLE in all builds.
*/

static inline thandle_t thandle_from_int(int ifd)
{
    return (thandle_t)(intptr_t)ifd;
}

static inline int thandle_to_int(thandle_t fd)
{
    return (int)(intptr_t)fd;
}

static tmsize_t
_tiffReadProc(thandle_t fd, void* buf, tmsize_t size)
{
	/* tmsize_t is 64bit on 64bit systems, but the WinAPI ReadFile takes
	 * 32bit sizes, so we loop through the data in suitable 32bit sized
	 * chunks */
	uint8_t* ma;
	uint64_t mb;
	DWORD n;
	DWORD o;
	tmsize_t p;
	ma=(uint8_t*)buf;
	mb=size;
	p=0;
	while (mb>0)
	{
		n=0x80000000UL;
		if ((uint64_t)n>mb)
			n=(DWORD)mb;
		if (!ReadFile(fd,(LPVOID)ma,n,&o,NULL))
			return(0);
		ma+=o;
		mb-=o;
		p+=o;
		if (o!=n)
			break;
	}
	return(p);
}

static tmsize_t
_tiffWriteProc(thandle_t fd, void* buf, tmsize_t size)
{
	/* tmsize_t is 64bit on 64bit systems, but the WinAPI WriteFile takes
	 * 32bit sizes, so we loop through the data in suitable 32bit sized
	 * chunks */
	uint8_t* ma;
	uint64_t mb;
	DWORD n;
	DWORD o;
	tmsize_t p;
	ma=(uint8_t*)buf;
	mb=size;
	p=0;
	while (mb>0)
	{
		n=0x80000000UL;
		if ((uint64_t)n>mb)
			n=(DWORD)mb;
		if (!WriteFile(fd,(LPVOID)ma,n,&o,NULL))
			return(0);
		ma+=o;
		mb-=o;
		p+=o;
		if (o!=n)
			break;
	}
	return(p);
}

static uint64_t
_tiffSeekProc(thandle_t fd, uint64_t off, int whence)
{
	LARGE_INTEGER offli;
	DWORD dwMoveMethod;
	offli.QuadPart = off;
	switch(whence)
	{
		case SEEK_SET:
			dwMoveMethod = FILE_BEGIN;
			break;
		case SEEK_CUR:
			dwMoveMethod = FILE_CURRENT;
			break;
		case SEEK_END:
			dwMoveMethod = FILE_END;
			break;
		default:
			dwMoveMethod = FILE_BEGIN;
			break;
	}
	offli.LowPart=SetFilePointer(fd,offli.LowPart,&offli.HighPart,dwMoveMethod);
	if ((offli.LowPart==INVALID_SET_FILE_POINTER)&&(GetLastError()!=NO_ERROR))
		offli.QuadPart=0;
	return(offli.QuadPart);
}

static int
_tiffCloseProc(thandle_t fd)
{
	return (CloseHandle(fd) ? 0 : -1);
}

static uint64_t
_tiffSizeProc(thandle_t fd)
{
	LARGE_INTEGER m;
	if (GetFileSizeEx(fd,&m))
		return(m.QuadPart);
	else
		return(0);
}

static int
_tiffDummyMapProc(thandle_t fd, void** pbase, toff_t* psize)
{
	(void) fd;
	(void) pbase;
	(void) psize;
	return (0);
}

/*
 * From "Hermann Josef Hill" <lhill@rhein-zeitung.de>:
 *
 * Windows uses both a handle and a pointer for file mapping,
 * but according to the SDK documentation and Richter's book
 * "Advanced Windows Programming" it is safe to free the handle
 * after obtaining the file mapping pointer
 *
 * This removes a nasty OS dependency and cures a problem
 * with Visual C++ 5.0
 */
static int
_tiffMapProc(thandle_t fd, void** pbase, toff_t* psize)
{
	uint64_t size;
	tmsize_t sizem;
	HANDLE hMapFile;

	size = _tiffSizeProc(fd);
	sizem = (tmsize_t)size;
	if (!size || (uint64_t)sizem!=size)
		return (0);

	/* By passing in 0 for the maximum file size, it specifies that we
	   create a file mapping object for the full file size. */
	hMapFile = CreateFileMapping(fd, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMapFile == NULL)
		return (0);
	*pbase = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hMapFile);
	if (*pbase == NULL)
		return (0);
	*psize = size;
	return(1);
}

static void
_tiffDummyUnmapProc(thandle_t fd, void* base, toff_t size)
{
	(void) fd;
	(void) base;
	(void) size;
}

static void
_tiffUnmapProc(thandle_t fd, void* base, toff_t size)
{
	(void) fd;
	(void) size;
	UnmapViewOfFile(base);
}

/*
 * Open a TIFF file descriptor for read/writing.
 * Note that TIFFFdOpen and TIFFOpen recognise the character 'u' in the mode
 * string, which forces the file to be opened unmapped.
 */
TIFF*
TIFFFdOpen(int ifd, const char* name, const char* mode)
{
    TIFFOpenExtStruct arguments;
    arguments.version = 1;
    arguments.errorhandler = NULL;
    arguments.warnhandler = NULL;
    return TIFFFdOpenExt(ifd, name, mode, &arguments);
}

TIFF*
TIFFFdOpenExt(int ifd, const char* name, const char* mode, TIFFOpenExtStruct* arguments)
{
    static const char module[] = "TIFFFdOpenExt";
	TIFF* tif;
	int fSuppressMap;
	int m;

    if( arguments == NULL )
    {
        TIFFErrorExt(0, module, "arguments should NOT be NULL");
        return NULL;
    }
    if( arguments->version < 1 )
    {
        TIFFErrorExt(0, module, "arguments->version should be >= 1");
        return NULL;
    }

	fSuppressMap=0;
	for (m=0; mode[m]!=0; m++)
	{
		if (mode[m]=='u')
		{
			fSuppressMap=1;
			break;
		}
	}

    TIFFClientOpenExtStruct client_arguments;
    client_arguments.version = 1;
    client_arguments.readproc = _tiffReadProc;
    client_arguments.writeproc = _tiffWriteProc;
    client_arguments.seekproc = _tiffSeekProc;
    client_arguments.closeproc = _tiffCloseProc;
    client_arguments.sizeproc = _tiffSizeProc;
    client_arguments.mapproc = fSuppressMap ? _tiffDummyMapProc : _tiffMapProc;
    client_arguments.unmapproc = fSuppressMap ? _tiffDummyUnmapProc : _tiffUnmapProc;
    client_arguments.errorhandler = arguments->errorhandler;
    client_arguments.warnhandler = arguments->warnhandler;

	tif = TIFFClientOpenExt(name, mode, thandle_from_int(ifd),
			&client_arguments);
	if (tif)
		tif->tif_fd = ifd;
	return (tif);
}

#ifndef _WIN32_WCE

/*
 * Open a TIFF file for read/writing.
 */
TIFF*
TIFFOpen(const char* name, const char* mode)
{
    TIFFOpenExtStruct arguments = {
        .version = 1,
        .errorhandler = NULL,
        .errorhandler_user_data = NULL,
        .warnhandler = NULL,
        .warnhandler_user_data = NULL
    };
    return TIFFOpenExt(name, mode, &arguments);
}

TIFF*
TIFFOpenExt(const char* name, const char* mode, TIFFOpenExtStruct* arguments)
{
	static const char module[] = "TIFFOpen";
	thandle_t fd;
	int m;
	DWORD dwMode;
	TIFF* tif;

    if (arguments == NULL)
    {
        TIFFErrorExt(0, module, "arguments should NOT be NULL");
        return NULL;
    }
    if (arguments->version < 1)
    {
        TIFFErrorExt(0, module, "arguments->version should be >= 1");
        return NULL;
    }

	m = _TIFFgetMode(mode, module);

	switch(m) {
		case O_RDONLY:			dwMode = OPEN_EXISTING; break;
		case O_RDWR:			dwMode = OPEN_ALWAYS;   break;
		case O_RDWR|O_CREAT:		dwMode = OPEN_ALWAYS;   break;
		case O_RDWR|O_TRUNC:		dwMode = CREATE_ALWAYS; break;
		case O_RDWR|O_CREAT|O_TRUNC:	dwMode = CREATE_ALWAYS; break;
		default:			return ((TIFF*)0);
	}
        
	fd = (thandle_t)CreateFileA(name,
		(m == O_RDONLY)?GENERIC_READ:(GENERIC_READ | GENERIC_WRITE),
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, dwMode,
		(m == O_RDONLY)?FILE_ATTRIBUTE_READONLY:FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (fd == INVALID_HANDLE_VALUE) {
		TIFFErrorExt(0, module, "%s: Cannot open", name);
		return ((TIFF *)0);
	}

	tif = TIFFFdOpenExt(thandle_to_int(fd), name, mode, arguments);
	if(!tif)
		CloseHandle(fd);
	return tif;
}

/*
 * Open a TIFF file with a Unicode filename, for read/writing.
 */
TIFF*
TIFFOpenW(const wchar_t* name, const char* mode)
{
    TIFFOpenExtStruct arguments = {
        .version = 1,
        .errorhandler = NULL,
        .errorhandler_user_data = NULL,
        .warnhandler = NULL,
        .warnhandler_user_data = NULL
    };
    return TIFFOpenWExt(name, mode, &arguments);
}

TIFF*
TIFFOpenWExt(const wchar_t* name, const char* mode, TIFFOpenExtStruct* arguments)
{
	static const char module[] = "TIFFOpenW";
	thandle_t fd;
	int m;
	DWORD dwMode;
	int mbsize;
	char *mbname;
	TIFF *tif;

    if (arguments == NULL)
    {
        TIFFErrorExt(0, module, "arguments should NOT be NULL");
        return NULL;
    }
    if (arguments->version < 1)
    {
        TIFFErrorExt(0, module, "arguments->version should be >= 1");
        return NULL;
    }

	m = _TIFFgetMode(mode, module);

	switch(m) {
		case O_RDONLY:			dwMode = OPEN_EXISTING; break;
		case O_RDWR:			dwMode = OPEN_ALWAYS;   break;
		case O_RDWR|O_CREAT:		dwMode = OPEN_ALWAYS;   break;
		case O_RDWR|O_TRUNC:		dwMode = CREATE_ALWAYS; break;
		case O_RDWR|O_CREAT|O_TRUNC:	dwMode = CREATE_ALWAYS; break;
		default:			return ((TIFF*)0);
	}

	fd = (thandle_t)CreateFileW(name,
		(m == O_RDONLY)?GENERIC_READ:(GENERIC_READ|GENERIC_WRITE),
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, dwMode,
		(m == O_RDONLY)?FILE_ATTRIBUTE_READONLY:FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (fd == INVALID_HANDLE_VALUE) {
		TIFFErrorExt(0, module, "%S: Cannot open", name);
		return ((TIFF *)0);
	}

	mbname = NULL;
	mbsize = WideCharToMultiByte(CP_ACP, 0, name, -1, NULL, 0, NULL, NULL);
	if (mbsize > 0) {
		mbname = (char *)_TIFFmalloc(mbsize);
		if (!mbname) {
			TIFFErrorExt(0, module,
			"Can't allocate space for filename conversion buffer");
			return ((TIFF*)0);
		}

		WideCharToMultiByte(CP_ACP, 0, name, -1, mbname, mbsize,
				    NULL, NULL);
	}

	tif = TIFFFdOpenExt(thandle_to_int(fd),
			 (mbname != NULL) ? mbname : "<unknown>", mode,
             arguments);
	if(!tif)
		CloseHandle(fd);

	_TIFFfree(mbname);

	return tif;
}

#endif /* ndef _WIN32_WCE */

void*
_TIFFmalloc(tmsize_t s)
{
        if (s == 0)
                return ((void *) NULL);

	return (malloc((size_t) s));
}

void* _TIFFcalloc(tmsize_t nmemb, tmsize_t siz)
{
    if( nmemb == 0 || siz == 0 )
        return ((void *) NULL);

    return calloc((size_t) nmemb, (size_t)siz);
}

void
_TIFFfree(void* p)
{
	free(p);
}

void*
_TIFFrealloc(void* p, tmsize_t s)
{
	return (realloc(p, (size_t) s));
}

void
_TIFFmemset(void* p, int v, tmsize_t c)
{
	memset(p, v, (size_t) c);
}

void
_TIFFmemcpy(void* d, const void* s, tmsize_t c)
{
	memcpy(d, s, (size_t) c);
}

int
_TIFFmemcmp(const void* p1, const void* p2, tmsize_t c)
{
	return (memcmp(p1, p2, (size_t) c));
}

#ifndef _WIN32_WCE

static void
Win32WarningHandler(const char* module, const char* fmt, va_list ap)
{
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	fprintf(stderr, "Warning, ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ".\n");
}
TIFFErrorHandler _TIFFwarningHandler = Win32WarningHandler;

static void
Win32ErrorHandler(const char* module, const char* fmt, va_list ap)
{
	if (module != NULL)
		fprintf(stderr, "%s: ", module);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ".\n");
}
TIFFErrorHandler _TIFFerrorHandler = Win32ErrorHandler;

#endif /* ndef _WIN32_WCE */

/* vim: set ts=8 sts=8 sw=8 noet: */
/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 8
 * fill-column: 78
 * End:
 */
