# CMake build for libtiff
#
# Copyright © 2015 Open Microscopy Environment / University of Dundee
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

extra_dist(Makefile.vc)

set (TOOL_INCLUDES
        ${PROJECT_SOURCE_DIR}/libtiff
        ${PROJECT_BINARY_DIR}/libtiff
        ${PROJECT_SOURCE_DIR}/port
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(fax2ps fax2ps.c)
target_link_libraries(fax2ps tiff port)
target_include_directories(fax2ps PRIVATE ${TOOL_INCLUDES})

add_executable(fax2tiff fax2tiff.c)
target_link_libraries(fax2tiff tiff port)
target_include_directories(fax2tiff PRIVATE ${TOOL_INCLUDES})

add_executable(pal2rgb pal2rgb.c)
target_link_libraries(pal2rgb tiff port)
target_include_directories(pal2rgb PRIVATE ${TOOL_INCLUDES})

add_executable(ppm2tiff ppm2tiff.c)
target_link_libraries(ppm2tiff tiff port)
target_include_directories(ppm2tiff PRIVATE ${TOOL_INCLUDES})

add_executable(raw2tiff raw2tiff.c)
target_link_libraries(raw2tiff tiff port)
target_include_directories(raw2tiff PRIVATE ${TOOL_INCLUDES})

add_executable(rgb2ycbcr rgb2ycbcr.c)
target_link_libraries(rgb2ycbcr tiff port)
target_include_directories(rgb2ycbcr PRIVATE ${TOOL_INCLUDES})

add_executable(thumbnail thumbnail.c)
target_link_libraries(thumbnail tiff port)
target_include_directories(thumbnail PRIVATE ${TOOL_INCLUDES})

add_executable(tiff2bw tiff2bw.c)
target_link_libraries(tiff2bw tiff port)
target_include_directories(tiff2bw PRIVATE ${TOOL_INCLUDES})

add_executable(tiff2pdf tiff2pdf.c)
target_link_libraries(tiff2pdf tiff port)
target_include_directories(tiff2pdf PRIVATE ${TOOL_INCLUDES})

add_executable(tiff2ps tiff2ps.c)
target_link_libraries(tiff2ps tiff port)
target_include_directories(tiff2ps PRIVATE ${TOOL_INCLUDES})

add_executable(tiff2rgba tiff2rgba.c)
target_link_libraries(tiff2rgba tiff port)
target_include_directories(tiff2rgba PRIVATE ${TOOL_INCLUDES})

add_executable(tiffcmp tiffcmp.c)
target_link_libraries(tiffcmp tiff port)
target_include_directories(tiffcmp PRIVATE ${TOOL_INCLUDES})

add_executable(tiffcp tiffcp.c)
target_link_libraries(tiffcp tiff port)
target_include_directories(tiffcp PRIVATE ${TOOL_INCLUDES})

add_executable(tiffcrop tiffcrop.c)
target_link_libraries(tiffcrop tiff port)
target_include_directories(tiffcrop PRIVATE ${TOOL_INCLUDES})

add_executable(tiffdither tiffdither.c)
target_link_libraries(tiffdither tiff port)
target_include_directories(tiffdither PRIVATE ${TOOL_INCLUDES})

add_executable(tiffdump tiffdump.c)
target_link_libraries(tiffdump tiff port)
target_include_directories(tiffdump PRIVATE ${TOOL_INCLUDES})

add_executable(tiffinfo tiffinfo.c)
target_link_libraries(tiffinfo tiff port)
target_include_directories(tiffinfo PRIVATE ${TOOL_INCLUDES})

add_executable(tiffmedian tiffmedian.c)
target_link_libraries(tiffmedian tiff port)
target_include_directories(tiffmedian PRIVATE ${TOOL_INCLUDES})

add_executable(tiffset tiffset.c)
target_link_libraries(tiffset tiff port)
target_include_directories(tiffset PRIVATE ${TOOL_INCLUDES})

add_executable(tiffsplit tiffsplit.c)
target_link_libraries(tiffsplit tiff port)
target_include_directories(tiffsplit PRIVATE ${TOOL_INCLUDES})

# rgb2ycbcr and thumbnail are intended to *NOT* be installed. They are for
# testing purposes only.
install(TARGETS fax2ps
                fax2tiff
                pal2rgb
                ppm2tiff
                raw2tiff
                tiff2bw
                tiff2pdf
                tiff2ps
                tiff2rgba
                tiffcmp
                tiffcp
                tiffcrop
                tiffdither
                tiffdump
                tiffinfo
                tiffmedian
                tiffset
                tiffsplit
        RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}")

if(HAVE_OPENGL)
  add_executable(tiffgt tiffgt.c)
  target_include_directories(tiffgt PRIVATE
          ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
  target_link_libraries(tiffgt tiff port ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

  install(TARGETS tiffgt
          RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}")
endif()

if(WEBP_SUPPORT AND EMSCRIPTEN)
  # Emscripten is pretty finnicky about linker flags.
  # It needs --shared-memory if and only if atomics or bulk-memory is used.
  foreach(target fax2ps
                 fax2tiff
                 pal2rgb
                 ppm2tiff
                 raw2tiff
                 rgb2ycbcr
                 thumbnail
                 tiff2bw
                 tiff2pdf
                 tiff2ps
                 tiff2rgba
                 tiffcmp
                 tiffcp
                 tiffcrop
                 tiffdither
                 tiffdump
                 tiffinfo
                 tiffmedian
                 tiffset
                 tiffsplit)
    target_link_options(${target} PUBLIC "-Wl,--shared-memory")
  endforeach()
endif()
