TIFFClose
==========

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>


.. c:function:: void TIFFClose(TIFF* tif)

Description
-----------

:c:func:`TIFFClose` closes a file that was previously opened with
:c:func:`TIFFOpen`.  Any buffered data are flushed to the file, including
the contents of the current directory (if modified); and all resources
are reclaimed.

Diagnostics
-----------

All error messages are directed to the :c:func:`TIFFError` routine.
Likewise, warning messages are directed to the :c:func:`TIFFWarning` routine.

See also
--------

:doc:`libtiff` (3tiff),
:doc:`TIFFOpen`  (3tiff),
:doc:`TIFFError` (3tiff)
