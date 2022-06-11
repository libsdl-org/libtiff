TIFFError
=========

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>
    #include <stdarg.h>

.. c:function:: void TIFFError(const char * module, const char * fmt, ...)

.. c:type:: void (*TIFFErrorHandler)(const char * module, const char* fmt, va_list ap)

.. c:function:: TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler handler)

Description
-----------

:c:func:`TIFFError` invokes the library-wide error handling function to (normally) write an error
message to ``stderr``.  The *fmt* parameter is a :c:func:`printf` format string, and any number
arguments can be supplied. The *module* parameter, if non-zero, is printed before the message; it
typically is used to identify the software module in which an error is detected.

Applications that desire to capture control in the event of an error should
:c:func:`TIFFSetErrorHandler` to override the default error handler.
A :c:macro:`NULL` (0) error handling function may be installed to suppress error messages.

Return values
-------------

:c:func:`TIFFSetErrorHandler` returns a reference to the previous error handling function.

See also
--------

:doc:`TIFFWarning` (3tiff),
:doc:`libtiff` (3tiff),
printf (3)
