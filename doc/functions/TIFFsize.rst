TIFFsize
========

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>

.. c:function:: tsize_t TIFFRasterScanlineSize(TIFF* tif)

.. c:function:: tsize_t TIFFScanlineSize(TIFF* tif)

Description
-----------

:c:func:`TIFFScanlineSize` returns the size in bytes of a row of data as
it would be returned in a call to :c:func:`TIFFReadScanline`, or as it
would be expected in a call to :c:func:`TIFFWriteScanline`.

:c:func:`TIFFRasterScanlineSize` returns the size in bytes of a complete
decoded and packed raster scanline. Note that this value may be different
from the value returned by :c:func:`TIFFScanlineSize` if data is stored
as separate planes.

Diagnostics
-----------

None.

See also
--------

:doc:`TIFFOpen` (3tiff),
:doc:`TIFFReadScanline` (3tiff),
:doc:`libtiff` (3tiff)
