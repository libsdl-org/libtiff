Extending LibTiff library with support for the new BigTIFF format
=================================================================

LibTiff maintainers have started work on LibTiff 4.0,
the next major upgrade of the time-proven `TIFF <http://www.awaresystems.be/imaging/tiff.html>`_
codec. This upgrade will include support for the new BigTIFF file format. It is planned a preliminary
version (LibTiff 4.0alpha1) will be operational by June 15, 2007. Testing and final release improvements
for LibTiff 4.0 will be completed by July 30, 2007.

BigTIFF logically extends the original TIFF file format (referred to as 'ClassicTIFF' from now on),
BigTIFF specification is the result of work by a variety of parties on the LibTiff mailing list, including
the current LibTiff maintainers, Joris Van Damme and Adobe staff. The BigTIFF specification has not yet
been officially approved by the TIFF specification owner (Adobe), but implementation within LibTiff
could accelerate that process. For more information on the BigTIFF file format, we recommend AWare Systems'
`BigTIFF page <http://www.awaresystems.be/imaging/tiff/bigtiff.html>`_.

BigTIFF will be especially useful for people and vendors that are confronted with very large images, and
still seek to use an open, simple, and extendable format. This requirement is frequently encountered in the
geospatial field, but also concerns large format scanners, medical imaging and other fields.

The LibTiff BigTIFF upgrade is made possible by four sponsors funding the project. The programming work will
primarily be done by `Joris Van Damme (AWare Systems) <http://www.awaresystems.be/index.html>`_.
LibTiff's licensing agreement will remain unchanged. More details on the project itself, are available from
the :ref:`BigTIFFProposal` page.

The sponsors are, in alphabetical order:

.. list-table:: Sponsors
    :widths: 5 20
    :header-rows: 0

    * - .. image:: bigtiffpr_images/esri.png
            :width: 100%
            :alt: ESRI
      - `ESRI <http://www.esri.com/>`_ has been giving customers around the world the power to
        think and plan geographically since 1969.  As the leader in GIS, ESRI applies innovative technologies to help
        organizations create, analyze, and visualize information for more informed decisions.  Running on more than
        a million desktops and thousands of servers, ESRI applications are the foundation of the world's mapping and
        spatial analysis infrastructure.
    * - .. image:: bigtiffpr_images/leica.png
            :width: 100%
            :alt: Leica Geosystems Geospatial Imaging
      - `Leica Geosystems Geospatial Imaging <http://gi.leica-geosystems.com/default.aspx>`_ offers a
        range of workflow solutions for photogrammetry, mapping, remote sensing, catalog management and exploitation
        of geospatial imagery. Enterprise organizations use this imagery as the basis for generating information for
        both education and decision making processes. Those who use Leica Geosystems products every day trust them
        for their precision, seamless integration, interoperability and superior customer support.
    * - .. image:: bigtiffpr_images/safe.png
            :width: 100%
            :alt: Safe Software
      - `Safe Software <http://www.safe.com/>`_ is the maker of FME, a powerful spatial ETL (Extract,
        Transform and Load) tool that enables true data interoperability. FME manages the translation,
        transformation, integration and web-based distribution of geospatial data in 200 GIS, CAD, raster and
        database formats. Safe Software's FME technology is also embedded in numerous market-leading GIS and
        ETL applications.
    * - .. image:: bigtiffpr_images/weogeo.png
            :width: 100%
            :alt: WeoGeo
      - `WeoGeo <http://www.weogeo.com/>`_ is a web-based data management resource for the geospatial
        industry that allows the free market exchange of mapping related imagery products, featuring an innovative
        solution that efficiently manages digital mapping files of any size. With an intuitive user interface
        and the scalable power of Amazon Web Services (AWS), geospatial professionals can view, sort, search,
        and share complex, high volume maps quickly and effectively.
