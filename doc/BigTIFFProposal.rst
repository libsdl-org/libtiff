Proposal to Implement BigTIFF Support in LibTiff
================================================

**NEWS:** We have located all four required sponsors. There are no more
open slots. We will issue appropriate press release shortly.

0. Summary
----------

BigTIFF logically extends the original TIFF file format (referred to as 
'ClassicTIFF' from this point on), breaking the 4 gigabyte boundary, in theory
allowing files up to 18,000 petabytes in size. The BigTIFF specification is the
result of work by a variety of parties on the LibTiff mailing list, including
the current LibTiff maintainers, Joris Van Damme and Adobe staff. The BigTIFF
specification has not yet been officially approved by the TIFF specification
owner (Adobe) but implementation within LibTiff could accelerate that
process.

For more information on the BigTIFF file format, we recommend 
`AWare Systems' BigTIFF page <http://www.awaresystems.be/imaging/tiff/bigtiff.html>`_.

BigTIFF is expected to be especially useful for people and vendors that are 
confronted with unusually large images, and still seek to use an open, simple,
and extendable format. This requirement is frequently seen in the geospatial
field, but also affects large format scanners, medical imaging and other
fields.

1. Sponsoring
-------------

It is planned that LibTiff 4.0, the BigTIFF upgrade to LibTiff, would start
March 1, 2007. A preliminary version (LibTiff 4.0alpha1) will be operational by
June 15, 2007. Testing and final release improvements for LibTiff 4.0 will be
completed by July 30, 2007.

The LibTiff BigTIFF upgrade team is seeking USD 24,000 from four sponsors (USD 
6,000 each) to fund the project. Sponsorship payment will be invoiced upon
delivery of the LibTiff 4.0alpha1 release (on or before June 15) with a 30 day
payable, giving time for some technical review.

In addition to the benefits of open interchange of BigTIFF data that comes from 
support in a public and free open source codec, sponsors will be given proper
public acknowledgment. During development, they will be mentioned in the
appropriate sections of the LibTiff and AWare Systems site.

When LibTiff 4.0 with BigTIFF support is released, they will additionally 
receive public acknowledgment in a formal press release, distributed widely to
news venues in the imaging and geospatial industries.

A 'migration guide' document or any functional equivalent thereof will be 
included in the standard free distribution, but sponsors will additionally
receive up to 20 hours of consulting and support on any BigTIFF migration issues
that they may be facing, and/or any additional related requests they may have.

2. Development and Testing
--------------------------

The work will primarily be done by Joris Van Damme on behalf of his company 
AWare Systems. Joris was closely involved in the drafting of the BigTIFF file
format proposal, and has also developed the proprietary AWare Systems TIFF
codec that already supports BigTIFF. Joris Van Damme has a deep understanding
of the TIFF format, and has been acting as a LibTiff co-maintainer for over a
year, contributing the upgraded OJPEG codec as well as several other
improvements.

Joris and AWare Systems have agreed to crosstest their proprietary codec and 
LibTiff's support for BigTIFF extensively. Joris will also extend the test image
library included with LibTiff, with a number of varying BigTIFF test files.
These files will primarily include test files with issues that relate closely
to the BigTIFF file format and the difference with ClassicTIFF. For instance,
in BigTIFF the datatypes TIFF_LONG, TIFF_LONG8, TIFF_IFD and TIFF_IFD8 are all
valid for tags pointing to additional IFDs, and thus files will be included to
illustrate and enable testing all of these. Additionally, Joris will build a
tool to enable creating test files that exceed 4 gigabyte, as these can of
course not be included in the test suite in a more direct manner, and a tool to
convert files from ClassicTIFF to BigTIFF, as well as the other way around for
source BigTIFF files that do not exceed 4 gigabyte in size.

Frank Warmerdam has also agreed to be involved in a consulting and validation 
role. Frank has been LibTiff's primary maintainer since approximately the year
2000, develops large-image exploitation software for the geospatial industry
(GDAL) and is currently President of the Open Source Geospatial Foundation
(OSGeo). Frank will also be ensuring that BigTIFF improvements are made
accessible to all users of the GDAL library.

3. Implementation Strategy
--------------------------

Many people who have a need for BigTIFF, use LibTiff already. We thus plan to
support BigTIFF in LibTiff, applying a minimum change strategy much like the
one that was applied in the creation of the new file format in the first
place.

To support BigTIFF, we will use a 64bit integer datatype. It is anticipated 
that some older platforms will not be able to comply with this need, and for
that reason we will encapsulate all required changes inside a conditional
compilation switch.

While it is anticipated that there will be ABI (application binary interface) 
changes as part of a BigTIFF support upgrade to LibTiff, and possibly some
more to support other major upgrades to LibTiff, the plan is such that
the source level API will remain compatible for most LibTiff applications.
Applications using specialized interfaces may require some minor source code
changes, but we will provide backwards compatibility to support the majority
of existing application level code.

The resulting LibTiff version would transparently support reading traditional 
32bit ClassicTIFF files, and 64bit BigTIFF files, as well as writing either
ClassicTIFF or BigTIFF files based on a flag in the TIFFOpen/TIFFClientOpen call.

4. Intellectual Property and Licensing
--------------------------------------

The licensing agreement of LibTiff will remain unchanged.

5. Contact information
----------------------
Please contact `Frank Warmerdam <warmerdam@pobox.com>`_ and/or
`Joris Van Damme <info@awaresystems.be>`_ for more information
on sponsorship agreements.
