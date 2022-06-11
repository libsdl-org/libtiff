TIFFswab
========

Synopsis
--------

.. highlight:: c

::

    #include <tiffio.h>

.. c:function:: const unsigned char* TIFFGetBitRevTable(int reversed)

.. c:function:: void TIFFReverseBits(u_char* data, unsigned long nbytes)

.. c:function:: void TIFFSwabShort(uint16_t* data)

.. c:function:: void TIFFSwabLong(uint32_t* data)

.. c:function:: void TIFFSwabArrayOfShort(uint16_t* data, unsigned long nshorts)

.. c:function:: void TIFFSwabArrayOfLong(uint32_t* data, unsigned long nlongs)

Description
-----------

The following routines are used by the library to swap 16- and 32-bit
data and to reverse the order of bits in bytes.
:c:func:`TIFFSwabShort` and :c:func:`TIFFSwabLong` swap the bytes in a
single 16-bit and 32-bit item, respectively.
:c:func:`TIFFSwabArrayOfShort` and :c:func:`TIFFSwabArrayOfLong`
swap the bytes in an array of 16-bit and 32-bit items, respectively.

:c:func:`TIFFReverseBits` replaces each byte in *data* with the
equivalent bit-reversed value. This operation is performed with a
lookup table, which is returned using the :c:func:`TIFFGetBitRevTable`
function.  The *reversed* parameter specifies which table should be
returned. Supply *1* if you want bit reversal table. Supply *0* to get
the table that do not reverse bit values. It is a lookup table that can
be used as an "identity function"; i.e. :c:expr:`TIFFNoBitRevTable[n] == n`.

Diagnostics
-----------

None.

See also
--------

:doc:`libtiff` (3tiff)
