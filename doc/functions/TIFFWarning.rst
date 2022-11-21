TIFFWarning
===========

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>
    #include <stdarg.h>

.. c:function:: void TIFFWarning(const char* module, const char* fmt, ...)

.. c:function:: void TIFFWarningExt(thandle_t fd, const char* module, const char* fmt, ...)

.. c:type:: void (*TIFFWarningHandler)(const char* module, const char* fmt, va_list ap);

.. c:type:: void (*TIFFWarningHandlerExt)(thandle_t fd, const char* module, const char* fmt, va_list ap);

.. c:function:: TIFFWarningHandler TIFFSetWarningHandler(TIFFWarningHandler handler)

.. c:function:: TIFFWarningHandlerExt TIFFSetWarningHandlerExt(TIFFWarningHandlerExt handler)

Description
-----------

:c:func:`TIFFWarning` invokes the library-wide warning handler function
to (normally) write a warning message to the ``stderr``.
The *fmt* parameter is a :c:func:`printf` format string, and any number
arguments can be supplied. The *module* parameter is interpreted as a
string that, if non-zero, should be printed before the message; it
typically is used to identify the software module in which a warning is
detected.

Applications that desire to capture control in the event of a warning
should use :c:func:`TIFFSetWarningHandler` to override the default
warning handler. A :c:macro:`NULL` (0) warning handler function may be
installed to suppress warning messages.

The function :c:func:`TIFFWarningExt` provides a file handle.
Within ``libtiff`` :c:func:`TIFFWarningExt` is called passing ``tif->tif_clientdata``
as *fd*, which represents the TIFF file handle (file descriptor).

.. TODO: Check description, how to setup a TIFFWarningExt handler and
   its file handle.

With :c:func:`TIFFSetWarningHandlerExt` an extra warning handler can be
setup up.

Note that, starting with libtiff 4.5, a per-TIFF handler may also be installed
with :c:func:`TIFFOpenExt` or :c:func:`TIFFClientOpenExt`

Note
----

Both functions :c:func:`TIFFWarning` and :c:func:`TIFFWarningExt`
each attempt to call both handler functions if they are defined.
First :c:func:`TIFFWarningHandler` is called and then :c:func:`TIFFWarningHandlerExt`.
However, :c:func:`TIFFWarning` passes a "0" as a file handle to :c:func:`TIFFWarningHandlerExt`. 

Return values
-------------

:c:func:`TIFFSetWarningHandler` and :c:func:`TIFFSetWarningHandlerExt`
returns a reference to the previous warning handling function.

See also
--------

:doc:`TIFFError` (3tiff),
:doc:`libtiff` (3tiff),
printf (3)
