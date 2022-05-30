# CMake build for libtiff
# Run "cmake" to generate the build files for your platform
#
# Copyright © 2015 Open Microscopy Environment / University of Dundee
# Copyright © 2021-2022 Roger Leigh <rleigh@codelibre.net>
# Written by Roger Leigh <rleigh@codelibre.net>
#
# Permission to use, copy, modify, distribute, and sell this software and
# its documentation for any purpose is hereby granted without fee, provided
# that (i) the above copyright notices and this permission notice appear in
# all copies of the software and related documentation, and (ii) the names of
# Sam Leffler and Silicon Graphics may not be used in any advertising or
# publicity relating to the software without the specific, prior written
# permission of Sam Leffler and Silicon Graphics.
#
# THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
# WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
#
# IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
# ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
# LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
# OF THIS SOFTWARE.

cmake_minimum_required(VERSION 3.2.0)

include(Sphinx)

if (BUILD_SPHINX AND SPHINX_BUILD)
  message(STATUS "Checking manual page dependencies")

  # Create build directory and conf.py
  set(sphinx_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
  set(sphinx_builddir "${CMAKE_CURRENT_BINARY_DIR}")
  file(MAKE_DIRECTORY "${sphinx_builddir}")

  sphinx_manpages("${sphinx_srcdir}" "${sphinx_builddir}"
                  "${PROJECT_BINARY_DIR}/man" MAN_PAGES)
  sphinx_manpage_dependencies("${sphinx_srcdir}" "${sphinx_builddir}"
                              MAN_PAGE_DEPENDENCIES)
  sphinx_dependencies("${sphinx_srcdir}" SPHINX_DEPENDENCIES)

  # Generate and install man pages

  unset(man_target)
  if(MAN_PAGES)
    add_custom_command(OUTPUT ${MAN_PAGES}
                       COMMAND ${CMAKE_COMMAND} -E make_directory "${sphinx_builddir}/cache"
                       COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/man"
                       COMMAND ${SPHINX_BUILD}
                               -D "release=${LIBTIFF_VERSION_FULL}"
                               -D "version=${tiff_VERSION_MAJOR}.${tiff_VERSION_MINOR}"
                               -D "srcdir=${sphinx_srcdir}"
                               -D "builddir=${sphinx_builddir}"
                               -c "${sphinx_builddir}"
                               -d "${sphinx_builddir}/cache"
                               -b man
                               "${sphinx_srcdir}" "${PROJECT_BINARY_DIR}/man"
                       WORKING_DIRECTORY "${sphinx_srcdir}"
                       DEPENDS ${MAN_PAGE_DEPENDENCIES})

    if(NOT TARGET man)
      add_custom_target(man ALL)
    endif()
    add_custom_target(${PROJECT_NAME}-man DEPENDS ${MAN_PAGES})
    add_dependencies(man ${PROJECT_NAME}-man)
    set(man_target ${PROJECT_NAME}-man)

    foreach (man ${MAN_PAGES})
      string(REGEX REPLACE ".*(.)\$" "\\1" man_section "${man}")
      install(FILES "${man}"
              DESTINATION "${CMAKE_INSTALL_MANDIR}/man${man_section}"
              COMPONENT "runtime")
    endforeach()
    endif()

  # Generate and install HTML manual
  # Depends on man to allow sharing of cache with parallel build.

  add_custom_command(OUTPUT "${sphinx_builddir}/html/index.html"
                     COMMAND ${CMAKE_COMMAND} -E make_directory "${sphinx_builddir}/cache"
                     COMMAND ${CMAKE_COMMAND} -E make_directory "${sphinx_builddir}/html"
                     COMMAND ${SPHINX_BUILD}
                             -D "release=${tiff_VERSION}"
                             -D "version=${tiff_VERSION_MAJOR}.${tiff_VERSION_MINOR}"
                             -D "srcdir=${sphinx_srcdir}"
                             -D "builddir=${sphinx_builddir}"
                             -d "${sphinx_builddir}/cache"
                             -b html
                             "${sphinx_srcdir}" "${sphinx_builddir}/html"
                     WORKING_DIRECTORY "${sphinx_srcdir}"
                     DEPENDS ${man_target} ${SPHINX_DEPENDENCIES})

  if(NOT TARGET doc-html)
    add_custom_target(doc-html ALL)
  endif()
  add_custom_target(${PROJECT_NAME}-doc-html DEPENDS "${sphinx_builddir}/html/index.html")
  add_dependencies(doc-html ${PROJECT_NAME}-doc-html)

  install(DIRECTORY "${sphinx_builddir}/html"
          DESTINATION "${CMAKE_INSTALL_DOCDIR}/manual"
          COMPONENT "runtime")

  if(sphinx-linkcheck)
    set(sphinx_linkcheck_all ALL)
  endif()

  if(NOT TARGET doc-linkcheck)
    add_custom_target(doc-linkcheck)
  endif()
  add_custom_target(${PROJECT_NAME}-doc-linkcheck ${sphinx_linkcheck_all}
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${sphinx_builddir}/cache"
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${sphinx_builddir}/linkcheck"
                    COMMAND ${SPHINX_BUILD}
                            -D "release=${tiff_VERSION}"
                            -D "version=${tiff_VERSION_MAJOR}.${tiff_VERSION_MINOR}"
                            -c "${sphinx_builddir}"
                            -d "${sphinx_builddir}/cache"
                            -b linkcheck
                            "${sphinx_srcdir}" "${sphinx_builddir}/linkcheck"
                     COMMENT "Checking remote links in sphinx documentation"
                     WORKING_DIRECTORY "${sphinx_srcdir}"
                     DEPENDS doc-api ${man_target} ${SPHINX_DEPENDENCIES})
  add_dependencies(doc-linkcheck ${PROJECT_NAME}-doc-linkcheck)

  if(NOT TARGET doc-locallinkcheck)
    add_custom_target(doc-locallinkcheck)
  endif()
  add_custom_target(${PROJECT_NAME}-doc-locallinkcheck ${sphinx_linkcheck_all}
                    COMMAND ${CMAKE_COMMAND}
                            "-DOUTPUT=${sphinx_builddir}/linkcheck/output.txt"
                            "-DSPHINX_INSTALL_PATH=${PROJECT_NAME}/manual/html"
                            "-DINTERNAL_REFERENCE=${sphinx_builddir}/html"
                            "-DEXTERNAL_REFERENCE=${CMAKE_INSTALL_FULL_DATAROOTDIR}/doc"
                            "-DDOXYGEN_REFERENCE=${PROJECT_BINARY_DIR}/docs/doxygen/api"
                            "-DDOXYGEN_INSTALL_PATH=${PROJECT_NAME}/api"
                            -P "${PROJECT_SOURCE_DIR}/cmake/SphinxCheckInternalLinks.cmake"
                     COMMENT "Checking local links in sphinx documentation"
                     WORKING_DIRECTORY "${sphinx_srcdir}"
                     DEPENDS doc-linkcheck)
  add_dependencies(doc-locallinkcheck ${PROJECT_NAME}-doc-locallinkcheck)
else()
  message(WARNING "Manual pages and HTML manual will not be generated or installed")
endif()

set(html_manpages
        libtiff.3tiff.html
        TIFFbuffer.3tiff.html
        TIFFClose.3tiff.html
        TIFFcodec.3tiff.html
        TIFFcolor.3tiff.html
        TIFFDataWidth.3tiff.html
        TIFFError.3tiff.html
        TIFFFieldDataType.3tiff.html
        TIFFFieldName.3tiff.html
        TIFFFieldPassCount.3tiff.html
        TIFFFieldReadCount.3tiff.html
        TIFFFieldTag.3tiff.html
        TIFFFieldWriteCount.3tiff.html
        TIFFFlush.3tiff.html
        TIFFGetField.3tiff.html
        TIFFmemory.3tiff.html
        TIFFOpen.3tiff.html
        TIFFPrintDirectory.3tiff.html
        TIFFquery.3tiff.html
        TIFFReadDirectory.3tiff.html
        TIFFReadEncodedStrip.3tiff.html
        TIFFReadEncodedTile.3tiff.html
        TIFFReadRawStrip.3tiff.html
        TIFFReadRawTile.3tiff.html
        TIFFReadRGBAImage.3tiff.html
        TIFFReadRGBAStrip.3tiff.html
        TIFFReadRGBATile.3tiff.html
        TIFFReadScanline.3tiff.html
        TIFFReadTile.3tiff.html
        TIFFRGBAImage.3tiff.html
        TIFFSetDirectory.3tiff.html
        TIFFSetField.3tiff.html
        TIFFsize.3tiff.html
        TIFFstrip.3tiff.html
        TIFFswab.3tiff.html
        TIFFtile.3tiff.html
        TIFFWarning.3tiff.html
        TIFFWriteDirectory.3tiff.html
        TIFFWriteEncodedStrip.3tiff.html
        TIFFWriteEncodedTile.3tiff.html
        TIFFWriteRawStrip.3tiff.html
        TIFFWriteRawTile.3tiff.html
        TIFFWriteScanline.3tiff.html
        TIFFWriteTile.3tiff.html
        fax2ps.1.html
        fax2tiff.1.html
        pal2rgb.1.html
        ppm2tiff.1.html
        raw2tiff.1.html
        tiff2bw.1.html
        tiff2pdf.1.html
        tiff2ps.1.html
        tiff2rgba.1.html
        tiffcmp.1.html
        tiffcp.1.html
        tiffcrop.1.html
        tiffdither.1.html
        tiffdump.1.html
        tiffgt.1.html
        tiffinfo.1.html
        tiffmedian.1.html
        tiffset.1.html
        tiffsplit.1.html)

# htmldoc target to regenerate HTML files
string(REPLACE ";" "^" escaped_html_manpages "${html_manpages}")
add_custom_target(htmldoc
        COMMAND "${CMAKE_COMMAND}"
        "-DMANSRCDIR=${PROJECT_SOURCE_DIR}/man"
        "-DHTMLMANDIR=${PROJECT_SOURCE_DIR}/doc/static/man"
        "-DINDEXFILE=${PROJECT_SOURCE_DIR}/doc/manpages.rst"
        "-DDOCFILES=${escaped_html_manpages}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/HtmlDoc.cmake")
