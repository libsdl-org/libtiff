TIFFSetDirectory
================

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>

.. c:function:: int TIFFSetDirectory(TIFF* tif, tdir_t dirnum)

.. c:function:: int TIFFSetSubDirectory(TIFF* tif, uint64_t diroff)

Description
-----------

:c:func:`TIFFSetDirectory` changes the current directory and reads its
contents with :c:func:`TIFFReadDirectory`.  The parameter *dirnum*
specifies the subfile/directory as an integer number, with the first
directory numbered zero.

:c:func:`TIFFSetSubDirectory` acts like :c:func:`TIFFSetDirectory`,
except the directory is specified as a file offset instead of an index;
this is required for accessing subdirectories linked through a
``SubIFD`` tag.

Return values
-------------

On successful return 1 is returned. Otherwise, 0 is returned if *dirnum*
or *diroff* specifies a non-existent directory, or if an error was
encountered while reading the directory's contents.

Diagnostics
-----------

All error messages are directed to the :c:func:`TIFFError` routine.

``"%s: Error fetching directory count"``:

  An error was encountered while reading the "directory count" field.

``"%s: Error fetching directory link"``:

  An error was encountered while reading the "link value" that points to the
  next directory in a file.

See also
--------

:doc:`TIFFquery` (3tiff),
:doc:`TIFFOpen` (3tiff),
:doc:`TIFFReadDirectory` (3tiff),
:doc:`TIFFWriteDirectory` (3tiff),
:doc:`libtiff` (3tiff),
