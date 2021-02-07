# CMake build for libtiff
# Run "cmake" to generate the build files for your platform
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

cmake_minimum_required(VERSION 3.9.0)

# Default policy is from 3.9.0
cmake_policy(VERSION 3.9.0)

# Find CMake modules in cmake/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Read version information from configure.ac.
include(AutotoolsVersion)
message(STATUS "Building tiff version ${LIBTIFF_VERSION_FULL}")
message(STATUS "libtiff library version ${SO_VERSION}")

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

# Project definition
set(CMAKE_C_STANDARD 99)
project(tiff
        VERSION "${LIBTIFF_VERSION}"
        LANGUAGES C CXX)

include(AutotoolsCompat)
include(CompilerChecks)
include(LinkerChecks)
include(ProcessorChecks)
include(IncludeChecks)
include(SymbolChecks)
include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CheckIncludeFile)
include(CheckLibraryExists)
include(CheckTypeSize)
enable_testing()

macro(current_date var)
  if(UNIX)
    execute_process(COMMAND "date" +"%Y%m%d" OUTPUT_VARIABLE ${var})
  endif()
endmacro()

current_date(RELEASE_DATE)

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()


# Find libm, if available
if(NOT MINGW)
  find_library(M_LIBRARY m)
endif()



# Disable deprecated features to ensure clean build
add_definitions(-DTIFF_DISABLE_DEPRECATED)

include(TypeSizeChecks)

macro(report_values)
  foreach(val ${ARGV})
    message(STATUS "${val} set to ${${val}}")
  endforeach()
endmacro()

report_values(TIFF_SSIZE_T)




report_values(CMAKE_HOST_SYSTEM_PROCESSOR HOST_FILLORDER
              HOST_BIG_ENDIAN HAVE_IEEEFP)

include(LargeFileSupport)

# Documentation install directory (default to cmake project docdir)
set(LIBTIFF_DOCDIR "${CMAKE_INSTALL_FULL_DOCDIR}")

include(InternalCodecs)
include(DeflateCodec)

# Option for Pixar log-format algorithm
# Pixar log format
set(PIXARLOG_SUPPORT FALSE)
option(pixarlog "support for Pixar log-format algorithm (requires Zlib)" ${ZLIB_SUPPORT})
if(pixarlog AND ZLIB_SUPPORT)
  set(PIXARLOG_SUPPORT TRUE)
endif()

include(JPEGCodec)

# JBIG-KIT
set(JBIG_SUPPORT FALSE)
find_path(JBIG_INCLUDE_DIR jbig.h)
set(JBIG_NAMES ${JBIG_NAMES} jbig libjbig)
find_library(JBIG_LIBRARY NAMES ${JBIG_NAMES})
if (JBIG_INCLUDE_DIR AND JBIG_LIBRARY)
  set(JBIG_FOUND TRUE)
  set(JBIG_LIBRARIES ${JBIG_LIBRARY})
else()
  set(JBIG_FOUND FALSE)
endif()
option(jbig "use ISO JBIG compression (requires JBIT-KIT library)" ${JBIG_FOUND})
if (jbig AND JBIG_FOUND)
  set(JBIG_SUPPORT TRUE)
endif()

set(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${JBIG_INCLUDE_DIR})
check_symbol_exists(jbg_newlen "jbig.h" HAVE_JBG_NEWLEN)
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})

# liblzma2
set(LZMA_SUPPORT FALSE)
find_package(LibLZMA)
option(lzma "use liblzma (required for LZMA2 compression)" ${LZMA_FOUND})
if (lzma AND LIBLZMA_FOUND)
  set(LZMA_SUPPORT TRUE)
endif()

# libzstd
set(ZSTD_SUPPORT FALSE)
find_path(ZSTD_INCLUDE_DIR zstd.h)
find_library(ZSTD_LIBRARY NAMES zstd zstd_static)
if (ZSTD_INCLUDE_DIR AND ZSTD_LIBRARY)
  check_library_exists ("${ZSTD_LIBRARY}" ZSTD_decompressStream "" ZSTD_RECENT_ENOUGH)
  if (ZSTD_RECENT_ENOUGH)
    set(ZSTD_FOUND TRUE)
    set(ZSTD_LIBRARIES ${ZSTD_LIBRARY})
    message(STATUS "Found ZSTD library: ${ZSTD_LIBRARY}")
  else()
    message(WARNING "Found ZSTD library, but not recent enough. Use zstd >= 1.0")
  endif()
else()
  set(ZSTD_FOUND FALSE)
endif()
option(zstd "use libzstd (required for ZSTD compression)" ${ZSTD_FOUND})
if (zstd AND ZSTD_FOUND)
  set(ZSTD_SUPPORT TRUE)
endif()

# libwebp
set(WEBP_SUPPORT FALSE)
find_path(WEBP_INCLUDE_DIR webp/decode.h)
find_library(WEBP_LIBRARY NAMES webp)
if (WEBP_INCLUDE_DIR AND WEBP_LIBRARY)
  set(WEBP_LIBRARIES ${WEBP_LIBRARY})
  set(WEBP_FOUND TRUE)
  message(STATUS "Found WEBP library: ${WEBP_LIBRARY}")
else()
  set(WEBP_FOUND FALSE)
endif()
option(webp "use libwebp (required for WEBP compression)" ${WEBP_FOUND})
if (webp AND WEBP_FOUND)
  set(WEBP_SUPPORT TRUE)
endif()

# 8/12-bit jpeg mode
set(JPEG12_INCLUDE_DIR JPEG12_INCLUDE_DIR-NOTFOUND CACHE PATH "Include directory for 12-bit libjpeg")
set(JPEG12_LIBRARY JPEG12_LIBRARY-NOTFOUND CACHE FILEPATH "12-bit libjpeg library")
set(JPEG_DUAL_MODE_8_12 FALSE)
if (JPEG12_INCLUDE_DIR AND JPEG12_LIBRARY)
  set(JPEG12_LIBRARIES ${JPEG12_LIBRARY})
  set(JPEG12_FOUND TRUE)
else()
  set(JPEG12_FOUND FALSE)
endif()
option(jpeg12 "enable libjpeg 8/12-bit dual mode (requires separate 12-bit libjpeg build)" ${JPEG12_FOUND})
if (jpeg12 AND JPEG12_FOUND)
  set(JPEG_DUAL_MODE_8_12 TRUE)
  set(LIBJPEG_12_PATH "${JPEG12_INCLUDE_DIR}/jpeglib.h")
endif()

# C++ support
option(cxx "Enable C++ stream API building (requires C++ compiler)" ON)
set(CXX_SUPPORT FALSE)
if (cxx)
  enable_language(CXX)
  set(CXX_SUPPORT TRUE)
endif()

# OpenGL and GLUT
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL COMPONENTS OpenGL)
find_package(GLUT)
set(HAVE_OPENGL FALSE)
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND GLUT_FOUND)
  set(HAVE_OPENGL TRUE)
endif()
# Purely to satisfy the generated headers:
check_include_file(GL/gl.h HAVE_GL_GL_H)
check_include_file(GL/glu.h HAVE_GL_GLU_H)
check_include_file(GL/glut.h HAVE_GL_GLUT_H)
check_include_file(GLUT/glut.h HAVE_GLUT_GLUT_H)
check_include_file(OpenGL/gl.h HAVE_OPENGL_GL_H)
check_include_file(OpenGL/glu.h HAVE_OPENGL_GLU_H)

include(WindowsIOFeature)

# Orthogonal features

# Strip chopping
option(strip-chopping "strip chopping (whether or not to convert single-strip uncompressed images to mutiple strips of specified size to reduce memory usage)" ON)
set(TIFF_DEFAULT_STRIP_SIZE 8192 CACHE STRING "default size of the strip in bytes (when strip chopping is enabled)")

set(STRIPCHOP_DEFAULT)
if(strip-chopping)
  set(STRIPCHOP_DEFAULT TRUE)
  if(TIFF_DEFAULT_STRIP_SIZE)
    set(STRIP_SIZE_DEFAULT "${TIFF_DEFAULT_STRIP_SIZE}")
  endif()
endif()

# Defer loading of strip/tile offsets
option(defer-strile-load "enable deferred strip/tile offset/size loading (also available at runtime with the 'D' flag of TIFFOpen())" OFF)
set(DEFER_STRILE_LOAD ${defer-strile-load})

# CHUNKY_STRIP_READ_SUPPORT
option(chunky-strip-read "enable reading large strips in chunks for TIFFReadScanline() (experimental)" OFF)
set(CHUNKY_STRIP_READ_SUPPORT ${chunky-strip-read})

# SUBIFD support
set(SUBIFD_SUPPORT 1)

# Default handling of ASSOCALPHA support.
option(extrasample-as-alpha "the RGBA interface will treat a fourth sample with no EXTRASAMPLE_ value as being ASSOCALPHA. Many packages produce RGBA files but don't mark the alpha properly" ON)
if(extrasample-as-alpha)
  set(DEFAULT_EXTRASAMPLE_AS_ALPHA 1)
endif()

# Default handling of YCbCr subsampling support.
# See Bug 168 in Bugzilla, and JPEGFixupTestSubsampling() for details.
option(check-ycbcr-subsampling "enable picking up YCbCr subsampling info from the JPEG data stream to support files lacking the tag" ON)
if (check-ycbcr-subsampling)
  set(CHECK_JPEG_YCBCR_SUBSAMPLING 1)
endif()

# Generate pkg-config file
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "${CMAKE_INSTALL_PREFIX}")
set(libdir "${CMAKE_INSTALL_FULL_LIBDIR}")
set(includedir "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libtiff-4.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/libtiff-4.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtiff-4.pc
        DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig")

# Includes used by libtiff (and tests)
if(JPEG12_INCLUDE_DIR)
  list(APPEND TIFF_INCLUDES ${JPEG12_INCLUDE_DIR})
endif()
if(JBIG_INCLUDE_DIR)
  list(APPEND TIFF_INCLUDES ${JBIG_INCLUDE_DIR})
endif()
if(LIBLZMA_INCLUDE_DIRS)
  list(APPEND TIFF_INCLUDES ${LIBLZMA_INCLUDE_DIRS})
endif()
if(ZSTD_INCLUDE_DIR)
  list(APPEND TIFF_INCLUDES ${ZSTD_INCLUDE_DIR})
endif()
if(WEBP_INCLUDE_DIR)
  list(APPEND TIFF_INCLUDES ${WEBP_INCLUDE_DIR})
endif()

# Libraries required by libtiff
set(TIFF_LIBRARY_DEPS)
if(NOT MINGW AND M_LIBRARY)
  list(APPEND TIFF_LIBRARY_DEPS "m")
endif()
if(JPEG12_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${JPEG12_LIBRARIES})
endif()
if(JBIG_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${JBIG_LIBRARIES})
endif()
if(LIBLZMA_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${LIBLZMA_LIBRARIES})
endif()
if(ZSTD_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${ZSTD_LIBRARIES})
endif()
if(WEBP_LIBRARIES)
  list(APPEND TIFF_LIBRARY_DEPS ${WEBP_LIBRARIES})
endif()

report_values(TIFF_INCLUDES TIFF_LIBRARY_DEPS)

# Process subdirectories
add_subdirectory(port)
add_subdirectory(libtiff)
add_subdirectory(tools)
add_subdirectory(test)
add_subdirectory(contrib)
add_subdirectory(build)
add_subdirectory(man)
add_subdirectory(html)

#message(STATUS "EXTRA_DIST: ${EXTRA_DIST}")

message(STATUS "")
message(STATUS "Libtiff is now configured for ${CMAKE_SYSTEM}")
message(STATUS "")
message(STATUS "  Installation directory:             ${prefix}")
message(STATUS "  Documentation directory:            ${LIBTIFF_DOCDIR}")
message(STATUS "  C compiler:                         ${CMAKE_C_COMPILER}")
message(STATUS "  C++ compiler:                       ${CMAKE_CXX_COMPILER}")
message(STATUS "  Build shared libraries:             ${BUILD_SHARED_LIBS}")
message(STATUS "  Enable linker symbol versioning:    ${HAVE_LD_VERSION_SCRIPT}")
message(STATUS "  Support Microsoft Document Imaging: ${mdi}")
message(STATUS "  Use win32 IO:                       ${USE_WIN32_FILEIO}")
message(STATUS "")
message(STATUS " Support for internal codecs:")
message(STATUS "  CCITT Group 3 & 4 algorithms:       ${ccitt}")
message(STATUS "  Macintosh PackBits algorithm:       ${packbits}")
message(STATUS "  LZW algorithm:                      ${lzw}")
message(STATUS "  ThunderScan 4-bit RLE algorithm:    ${thunder}")
message(STATUS "  NeXT 2-bit RLE algorithm:           ${next}")
message(STATUS "  LogLuv high dynamic range encoding: ${logluv}")
message(STATUS "")
message(STATUS " Support for external codecs:")
message(STATUS "  ZLIB support:                       Requested:${zlib} Availability:${ZLIB_FOUND} Support:${ZLIB_SUPPORT}")
if(ZLIB_SUPPORT)
  message(STATUS "  libdeflate support:                 Requested:${libdeflate} Availability:${DEFLATE_FOUND} Support:${LIBDEFLATE_SUPPORT}")
else()
  message(STATUS "  libdeflate support:                 Requested:${libdeflate} Availability:${DEFLATE_FOUND} Support:${LIBDEFLATE_SUPPORT} (Depends on ZLIB Support)")
endif()
if(ZLIB_SUPPORT)
  message(STATUS "  Pixar log-format algorithm:         Requested:${pixarlog} Availability:${ZLIB_FOUND} Support:${PIXARLOG_SUPPORT}")
else()
  message(STATUS "  Pixar log-format algorithm:         Requested:${pixarlog} Availability:${ZLIB_FOUND} Support:${PIXARLOG_SUPPORT} (Depends on ZLIB Support)")
endif()
message(STATUS "  JPEG support:                       Requested:${jpeg} Availability:${JPEG_FOUND} Support:${JPEG_SUPPORT}")
if(JPEG_SUPPORT)
  message(STATUS "  Old JPEG support:                   Requested:${old-jpeg} Availability:${JPEG_SUPPORT} Support:${OJPEG_SUPPORT}")
else()
  message(STATUS "  Old JPEG support:                   Requested:${old-jpeg} Availability:${JPEG_SUPPORT} Support:${OJPEG_SUPPORT} (Depends on JPEG Support)")
endif()
message(STATUS "  JPEG 8/12 bit dual mode:            Requested:${jpeg12} Availability:${JPEG12_FOUND} Support:${JPEG_DUAL_MODE_8_12}")
message(STATUS "  ISO JBIG support:                   Requested:${jbig} Availability:${JBIG_FOUND} Support:${JBIG_SUPPORT}")
message(STATUS "  LZMA2 support:                      Requested:${lzma} Availability:${LIBLZMA_FOUND} Support:${LZMA_SUPPORT}")
message(STATUS "  ZSTD support:                       Requested:${zstd} Availability:${ZSTD_FOUND} Support:${ZSTD_SUPPORT}")
message(STATUS "  WEBP support:                       Requested:${webp} Availability:${WEBP_FOUND} Support:${WEBP_SUPPORT}")
message(STATUS "")
message(STATUS "  C++ support:                        ${cxx} (requested) ${CXX_SUPPORT} (availability)")
message(STATUS "")
# message(STATUS "  X Athena Widgets support:           ${HAVE_XAW}")
message(STATUS "  OpenGL support:                     ${HAVE_OPENGL}")
message(STATUS "")
