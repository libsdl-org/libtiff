TIFFquery
=========

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>

.. c:function:: uint32_t TIFFCurrentRow(TIFF* tif)

.. c:function:: tstrip_t TIFFCurrentStrip(TIFF* tif)

.. c:function:: ttile_t TIFFCurrentTile(TIFF* tif)

.. c:function:: tdir_t TIFFCurrentDirectory(TIFF* tif)

.. c:function:: int TIFFLastDirectory(TIFF* tif)

.. c:function:: int TIFFFileno(TIFF* tif)

.. c:function:: char* TIFFFileName(TIFF* tif)

.. c:function:: int TIFFGetMode(TIFF* tif)

.. c:function:: int TIFFIsTiled(TIFF* tif)

.. c:function:: int TIFFIsByteSwapped(TIFF* tif)

.. c:function:: int TIFFIsUpSampled(TIFF* tif)

.. c:function:: int TIFFIsMSB2LSB(TIFF* tif)

.. c:function:: const char* TIFFGetVersion(void)

Description
-----------

The following routines return status information about an open TIFF file.

:c:func:`TIFFCurrentDirectory` returns the index of the current directory
(directories are numbered starting at 0). This number is suitable for use
with the :c:func:`TIFFSetDirectory` routine.

:c:func:`TIFFLastDirectory` returns a non-zero value if the current
directory is the last directory in the file; otherwise zero is returned.

:c:func:`TIFFCurrentRow`, :c:func:`TIFFCurrentStrip`, and
:c:func:`TIFFCurrentTile` return the current row, strip, and tile,
respectively, that is being read or written. These values are updated each
time a read or write is done.

:c:func:`TIFFFileno` returns the underlying file descriptor used to access the
TIFF image in the filesystem.

:c:func:`TIFFFileName` returns the pathname argument passed to :c:func:`TIFFOpen`
or :c:func:`TIFFFdOpen`.

:c:func:`TIFFGetMode` returns the mode with which the underlying file was opened.
On UNIX systems, this is the value passed to the :c:func:`open` (2) system call.

:c:func:`TIFFIsTiled` returns a non-zero value if the image data has a tiled
organization. Zero is returned if the image data is organized in strips.

:c:func:`TIFFIsByteSwapped` returns a non-zero value if the image data was in a
different byte-order than the host machine. Zero is returned if the TIFF file and
local host byte-orders are the same.  Note that :c:func:`TIFFReadTile`,
:c:func:`TIFFReadEncodedStrip` and :c:func:`TIFFReadScanline` functions already
normally perform byte swapping to local host order if needed.

:c:func:`TIFFIsUpSampled` returns a non-zero value if image data returned through
the read interface routines is being up-sampled. This can be useful to applications
that want to calculate I/O buffer sizes to reflect this usage (though the usual
strip and tile size routines already do this).

:c:func:`TIFFIsMSB2LSB` returns a non-zero value if the image data is being returned
with bit 0 as the most significant bit.

:c:func:`TIFFGetVersion` returns an ``ASCII`` string that has a version stamp for the 
TIFF library software.

Diagnostics
-----------

None.

See also
--------

:doc:`libtiff` (3tiff),
:doc:`TIFFOpen` (3tiff),
:doc:`TIFFFdOpen` (3tiff)
