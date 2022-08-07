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

Applications that desire to capture control in the event of an error should
:c:func:`TIFFSetErrorHandler` to override the default error handler. A :c:macro:`NULL`
(0) error handling function may be installed to suppress error messages.

The function :c:func:`TIFFErrorExt` provides a file handle in order
to write the error message to a file. Within ``libtiff`` :c:func:`TIFFErrorExt`
is called using ``tif->tif_clientdata`` as file handle.


.. TODO: Check description, how to setup a TIFFErrorExt handler and its file handle.

With :c:func:`TIFFSetErrorHandlerExt` an extra error handler can be setup
in order to write to a file. The file handle needs to be stored in
``tif->tif_clientdata`` if the ``libtiff`` internal errors shall also
be written to that file.


Note
----

In ``libtiff`` only a default error handler is defined, writing the message
to ``stderr``. For writing error messages to file, an extra *TIFFErrorHandlerExt*
function has to be set. :c:func:`TIFFError` and :c:func:`TIFFErrorExt`
will try to call both handler functions if defined. However, :c:func:`TIFFErrorExt`
will pass "0" as file handle to the extended error handler.

Return values
-------------

:c:func:`TIFFSetErrorHandler` and :c:func:`TIFFSetErrorHandlerExt` returns
a reference to the previous error handling function.

See also
--------

:doc:`TIFFWarning` (3tiff),
:doc:`libtiff` (3tiff),
printf (3)
