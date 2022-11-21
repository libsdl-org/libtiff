TIFFError
=========

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>
    #include <stdarg.h>

.. c:function:: void TIFFError(const char * module, const char * fmt, ...)

.. c:function:: void TIFFErrorExt(thandle_t fd, const char* module, const char* fmt, ...)

.. c:type:: void (*TIFFErrorHandler)(const char * module, const char* fmt, va_list ap)

.. c:type:: void (*TIFFErrorHandlerExt)(thandle_t fd, const char * module, const char* fmt, va_list ap)

.. c:function:: TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler handler)

.. c:function:: TIFFErrorHandlerExt TIFFSetErrorHandlerExt(TIFFErrorHandlerExt handler)

Description
-----------

:c:func:`TIFFError` invokes the library-wide error handling function to
(normally) write an error message to ``stderr``.  The *fmt* parameter is
a :c:func:`printf` format string, and any number arguments can be supplied.
The *module* parameter, if non-zero, is printed before the message; it
typically is used to identify the software module in which an error is detected.

Applications that desire to capture control in the event of an error should use
:c:func:`TIFFSetErrorHandler` to override the default error handler. A :c:macro:`NULL`
(0) error handling function may be installed to suppress error messages.

The function :c:func:`TIFFErrorExt` provides a file handle.
Within ``libtiff`` :c:func:`TIFFErrorExt` is called passing ``tif->tif_clientdata``
as *fd*, which represents the TIFF file handle (file descriptor).


.. TODO: Check description, how to setup a TIFFErrorExt handler and its file handle.

With :c:func:`TIFFSetErrorHandlerExt` an extra error handler can be setup
in order to write to a file. The file handle needs to be stored in
``tif->tif_clientdata`` if the ``libtiff`` internal errors shall also
be written to that file.

Note that, starting with libtiff 4.5, a per-TIFF handler may also be installed
with :c:func:`TIFFOpenExt` or :c:func:`TIFFClientOpenExt`

Note
----

Both functions :c:func:`TIFFError` and :c:func:`TIFFErrorExt`
each attempt to call both handler functions if they are defined.
First :c:func:`TIFFErrorHandler` is called and then :c:func:`TIFFErrorHandlerExt`.
However, :c:func:`TIFFError` passes a "0" as a file handle to :c:func:`TIFFErrorHandlerExt`. 

Return values
-------------

:c:func:`TIFFSetErrorHandler` and :c:func:`TIFFSetErrorHandlerExt` returns
a reference to the previous error handling function.

See also
--------

:doc:`TIFFWarning` (3tiff),
:doc:`libtiff` (3tiff),
printf (3)
