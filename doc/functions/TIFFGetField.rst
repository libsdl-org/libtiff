TIFFGetField
============

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>
    #include <stdarg.h>

.. c:function:: int TIFFGetField(TIFF* tif, ttag_t tag, ...)

.. c:function:: int TIFFVGetField(TIFF* tif, ttag_t tag, va_list ap)

.. c:function:: int TIFFGetFieldDefaulted(TIFF* tif, ttag_t tag, ...)

.. c:function:: int TIFFVGetFieldDefaulted(TIFF* tif, ttag_t tag, va_list ap)

Description
-----------

:c:func:`TIFFGetField` returns the value of a tag or pseudo-tag associated with the
the current directory of the opened TIFF file *tif*.  (A *pseudo-tag* is a
parameter that is used to control the operation of the TIFF library but whose value
is not read or written to the underlying file.) The  file must have been previously
opened with :c:func:`TIFFOpen`.  The tag is identified by *tag*, one of the values
defined in the include file :file:`tiff.h` (see also the table below). The type and
number of values returned is dependent on the tag being requested. The programming
interface uses a variable argument list as prescribed by the :file:`stdarg.h`
interface. The returned values should only be interpreted if :c:func:`TIFFGetField`
returns 1.

:c:func:`TIFFVGetField` is functionally equivalent to :c:func:`TIFFGetField`
except that it takes a pointer to a variable argument list.  :c:func:`TIFFVGetField`
is useful for layering interfaces on top of the functionality provided by
:c:func:`TIFFGetField`.

:c:func:`TIFFGetFieldDefaulted` and :c:func:`TIFFVGetFieldDefaulted` are identical
to :c:func:`TIFFGetField` and :c:func:`TIFFVGetField`, respectively, except that if
a tag is not defined in the current directory and it has a default value, then the
default value is returned.

The tags understood by :program:`libtiff` the number of parameter values, and the
types for the returned values are shown below. The data types are specified as in C
and correspond to the types used to specify tag values to
:c:func:`TIFFSetField`.  Remember that :c:func:`TIFFGetField` returns parameter
values, so all the listed data types are pointers to storage where values should be
returned.  Consult the TIFF specification (or relevant industry specification) for
information on the meaning of each tag and their possible values.

  .. list-table:: Tag properties
    :widths: 5 3 5 10
    :header-rows: 1

    * - Tag name
      - Count
      - Types
      - Notes

    * - :c:macro:`TIFFTAG_ARTIST`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_BADFAXLINES`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_BITSPERSAMPLE`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_CLEANFAXDATA`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_COLORMAP`
      - 3
      - :c:type:`const uint16_t**`
      - :c:expr:`1<<BitsPerSample` arrays

    * - :c:macro:`TIFFTAG_COMPRESSION`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_CONSECUTIVEBADFAXLINES`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_COPYRIGHT`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_DATATYPE`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_DATETIME`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_DOCUMENTNAME`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_DOTRANGE`
      - 2
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_EXTRASAMPLES`
      - 2
      - :c:type:`uint16_t*`, :c:type:`const uint16_t**`
      - count, types array

    * - :c:macro:`TIFFTAG_FAXFILLFUNC`
      - 1
      - :c:type:`TIFFFaxFillFunc*`
      - G3/G4 compression pseudo-tag

    * - :c:macro:`TIFFTAG_FAXMODE`
      - 1
      - :c:type:`int*`
      - G3/G4 compression pseudo-tag

    * - :c:macro:`TIFFTAG_FILLORDER`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_GROUP3OPTIONS`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_GROUP4OPTIONS`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_HALFTONEHINTS`
      - 2
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_HOSTCOMPUTER`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_ICCPROFILE`
      - 2
      - :c:type:`const uint32_t*`, :c:type:`const void**`
      - count, profile data‡

    * - :c:macro:`TIFFTAG_IMAGEDEPTH`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_IMAGEDESCRIPTION`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_IMAGELENGTH`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_IMAGEWIDTH`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_INKNAMES`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_INKSET`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_JPEGCOLORMODE`
      - 1
      - :c:type:`int*`
      - JPEG pseudo-tag

    * - :c:macro:`TIFFTAG_JPEGQUALITY`
      - 1
      - :c:type:`int*`
      - JPEG pseudo-tag

    * - :c:macro:`TIFFTAG_JPEGTABLES`
      - 2
      - :c:type:`uint32_t*`, :c:type:`const void**`
      - count, tables

    * - :c:macro:`TIFFTAG_JPEGTABLESMODE`
      - 1
      - :c:type:`int*`
      - JPEG pseudo-tag

    * - :c:macro:`TIFFTAG_MAKE`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_MATTEING`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_MAXSAMPLEVALUE`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_MINSAMPLEVALUE`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_MODEL`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_ORIENTATION`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_PAGENAME`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_PAGENUMBER`
      - 2
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_PHOTOMETRIC`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_PHOTOSHOP`
      - 2
      - :c:type:`uint32_t*`, :c:type:`const void**`
      - count, data

    * - :c:macro:`TIFFTAG_PLANARCONFIG`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_PREDICTOR`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_PRIMARYCHROMATICITIES`
      - 1
      - :c:type:`const float**`
      - 6-entry array

    * - :c:macro:`TIFFTAG_REFERENCEBLACKWHITE`
      - 1
      - :c:type:`const float**`
      - 6-entry array

    * - :c:macro:`TIFFTAG_RESOLUTIONUNIT`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_RICHTIFFIPTC`
      - 2
      - :c:type:`uint32_t*`, :c:type:`const void**`
      - count, data

    * - :c:macro:`TIFFTAG_ROWSPERSTRIP`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_SAMPLEFORMAT`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_SAMPLESPERPIXEL`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_SMAXSAMPLEVALUE`
      - 1
      - :c:type:`double*`
      -

    * - :c:macro:`TIFFTAG_SMINSAMPLEVALUE`
      - 1
      - :c:type:`double*`
      -

    * - :c:macro:`TIFFTAG_SOFTWARE`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_STONITS`
      - 1
      - :c:type:`const double**`
      -

    * - :c:macro:`TIFFTAG_STRIPBYTECOUNTS`
      - 1
      - :c:type:`const uint64_t**`
      -

    * - :c:macro:`TIFFTAG_STRIPOFFSETS`
      - 1
      - :c:type:`const uint64_t**`
      -

    * - :c:macro:`TIFFTAG_SUBFILETYPE`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_SUBIFD`
      - 2
      - :c:type:`uint16_t*`, :c:type:`const uint64_t**`
      - count, offsets array

    * - :c:macro:`TIFFTAG_TARGETPRINTER`
      - 1
      - :c:type:`const char**`
      -

    * - :c:macro:`TIFFTAG_THRESHHOLDING`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_TILEBYTECOUNTS`
      - 1
      - :c:type:`const uint64_t**`
      -

    * - :c:macro:`TIFFTAG_TILEDEPTH`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_TILELENGTH`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_TILEOFFSETS`
      - 1
      - :c:type:`const uint64_t**`
      -

    * - :c:macro:`TIFFTAG_TILEWIDTH`
      - 1
      - :c:type:`uint32_t*`
      -

    * - :c:macro:`TIFFTAG_TRANSFERFUNCTION`
      - 1 or 3†
      - :c:type:`const uint16_t**`
      - :c:expr:`1<<BitsPerSample` entry arrays

    * - :c:macro:`TIFFTAG_WHITEPOINT`
      - 1
      - :c:type:`const float**`
      - 2-entry array

    * - :c:macro:`TIFFTAG_XMLPACKET`
      - 2
      - :c:type:`uint32_t*,const void**`
      - count, data

    * - :c:macro:`TIFFTAG_XPOSITION`
      - 1
      - :c:type:`float*`
      -

    * - :c:macro:`TIFFTAG_XRESOLUTION`
      - 1
      - :c:type:`float*`
      -

    * - :c:macro:`TIFFTAG_YCBCRCOEFFICIENTS`
      - 1
      - :c:type:`const float**`
      - 3-entry array

    * - :c:macro:`TIFFTAG_YCBCRPOSITIONING`
      - 1
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_YCBCRSUBSAMPLING`
      - 2
      - :c:type:`uint16_t*`
      -

    * - :c:macro:`TIFFTAG_YPOSITION`
      - 1
      - :c:type:`float*`
      -

    * - :c:macro:`TIFFTAG_YRESOLUTION`
      - 1
      - :c:type:`float*`
      -

†:
  If ``SamplesPerPixel`` is one, then a single array is returned; otherwise
  three arrays are returned.

‡:
  The contents of this field are quite complex.  See
  "The ICC Profile Format Specification",
  Annex B.3 "Embedding ICC Profiles in TIFF Files" (available at
  http://www.color.org) for an explanation.

Autoregistered tags
-------------------

If you can't find the tag in the table above that means this is an unsupported
tag and is not directly supported by the :program:`libtiff` library. You will
still be able to read it's value if you know the data type of that tag. For
example, if you want to read the ``LONG`` value from the tag 33424
and ``ASCII`` string from the tag 36867 you can use the following code:

::

    uint32_t  count;
    void    *data;

    TIFFGetField(tiff, 33424, &count, &data);
    printf("Tag %d: %d, count %d\n", 33424, *(uint32_t *)data, count);
    TIFFGetField(tiff, 36867, &count, &data);
    printf("Tag %d: %s, count %d\n", 36867, (char *)data, count);

Return values
-------------

1 is returned if the tag is defined in the current directory; otherwise a 0 is
returned.

Diagnostics
-----------

All error messages are directed to the :c:func:`TIFFError` routine.

``Unknown field, tag 0x%x``:

  An unknown tag was supplied.

See also
--------

:doc:`TIFFOpen` (3tiff),
:doc:`TIFFSetField` (3tiff),
:doc:`TIFFSetDirectory` (3tiff),
:doc:`TIFFReadDirectory` (3tiff),
:doc:`TIFFWriteDirectory` (3tiff)
:doc:`libtiff` (3tiff)
