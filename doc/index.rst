####################################
LibTIFF - TIFF Library and Utilities
####################################

.. table:: References
    :widths: auto

    =====================  =====================================
    Name                   Reference
    =====================  =====================================
    Home Page #1           http://www.simplesystems.org/libtiff/
    Home Page #2           https://libtiff.gitlab.io/libtiff/
    Home Page #3           http://libtiff.maptools.org/
    Latest Stable Release  v4.4.0
    Master Download Site   `download.osgeo.org <https://download.osgeo.org/libtiff/>`_
    Mailing List           `tiff@lists.osgeo.org <tiff@lists.osgeo.org>`_
    List subscription      http://lists.osgeo.org/mailman/listinfo/tiff/
    List archive           http://www.awaresystems.be/imaging/tiff/tml.html
    Source repository      https://gitlab.com/libtiff/libtiff
    =====================  =====================================

This software provides support for the *Tag Image File Format* (TIFF),
a widely used format for storing image data.  The latest version of
the TIFF specification is :doc:`document`
available on-line in several different formats.

Included in this software distribution is a library, libtiff, for
reading and writing TIFF, a small collection of tools for doing
simple manipulations of TIFF images, and documentation on the
library and tools.  Libtiff is a portable software, it was built
and tested on various systems: UNIX flavors (Linux, BSD, Solaris,
MacOS X), Windows, and OpenVMS.  It should be possible to port
libtiff and additional tools on other OSes.

The library, along with associated tool programs, should handle most of
your needs for reading and writing TIFF images on 32- and 64-bit
machines.

The software was originally authored and maintained by Sam Leffler
but has been maintained by a cast of others since 1999.

Questions should be sent to the TIFF mailing list:
`tiff@lists.osgeo.org`_, with
a subscription interface at
http://lists.osgeo.org/mailman/listinfo/tiff.
The persons currently actively maintaining and releasing libtiff
are Even Rouault and
`Bob Friesenhahn <bfriesen@GraphicsMagick.org>`_.
Note from Even Rouault: do not email me personally, especially when asking
"when is the next release ?", unless you are willing to
pay a LOT of money for my services, and I may even not be interested.

Significant maintainers in the past (since the 3.5.1 release) are
`Frank Warmerdam <http://pobox.com/~warmerdam>`_,
`Andrey Kiselev <dron@ak4719.spb.edu>`_,
Joris Van Damme, and Lee Howard.

The following sections are included in this documentation:

.. toctree::
    :maxdepth: 1
    :titlesonly:

    intro
    support
    libtiff
    internals
    addingtags
    tools
    contrib
    document
    TIFFTechNote2
    build
    bugs
    images
    misc
    manpages
    releases
    BigTIFFProposal
    bigtiffdesign
    bigtiffpr
