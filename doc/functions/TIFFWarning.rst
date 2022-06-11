TIFFWarning
===========

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>
    #include <stdarg.h>

.. c:function:: void TIFFWarning(const char* module, const char* fmt, ...)

.. c:type:: void (*TIFFWarningHandler)(const char* module, const char* fmt, va_list ap);

.. c:function:: TIFFWarningHandler TIFFSetWarningHandler(TIFFWarningHandler handler)

Description
-----------

:c:func:`TIFFWarning` invokes the library-wide warning handler function
to (normally) write a warning message to the ``stderr``.
The *fmt* parameter is a :c:func:`printf` format string, and any number
arguments can be supplied. The *module* parameter is interpreted as a
string that, if non-zero, should be printed before the message; it
typically is used to identify the software module in which a warning is
detected.

Applications that desire to capture control in the event of a warning should
use :c:func:`TIFFSetWarningHandler` to override the default warning handler.
A :c:macro:`NULL` (0) warning handler function may be installed to suppress
error messages.

Return values
-------------

:c:func:`TIFFSetWarningHandler` returns a reference to the previous error
handling function.

See also
--------

:doc:`TIFFError` (3tiff),
:doc:`libtiff` (3tiff),
printf (3)
