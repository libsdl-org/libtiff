TIFF File Format Specification
==============================

.. image:: ../images/jim.gif
    :width: 139
    :alt: jim

A copy of the 6.0 specification is available from the libtiff
ftp site at `<https://download.osgeo.org/libtiff/doc/TIFF6.pdf>`_.

Draft :doc:`technote2` covers problems
with the TIFF 6.0 design for embedding JPEG-compressed data in TIFF, and 
describes an alternative.

################## Adobe information on TIFF is missing. E.g. http://partners.adobe.com does not work.  ##########

################## Is there a site for of known tags and their descriptions?? ##########

A design for a TIFF variation supporting files larger than 4GB is detailed in :doc:`bigtiff`.

The LibTIFF coverage of the TIFF 6.0 specification is detailed in :doc:`coverage`.

.. toctree::
    :maxdepth: 1
    :titlesonly:

    technote2
    bigtiff
    coverage
    coverage-bigtiff
