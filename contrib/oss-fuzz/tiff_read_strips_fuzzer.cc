// Copyright 2026 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////
//
// Fuzzer for libtiff raw strip and scanline decoding paths.
//
// The existing tiff_read_rgba_fuzzer.cc exercises TIFFReadRGBAImage(), which
// decodes into a 32-bit RGBA buffer via a high-level path.  It does NOT
// exercise the lower-level TIFFReadEncodedStrip / TIFFReadScanline /
// TIFFReadEncodedTile paths that go directly through each codec (LZW, ZIP,
// JPEG, PixarLog, LZMA, JBIG, WebP, ZStd, etc.).  Those paths share the same
// tag-parsing entry points but take a different branch once the actual
// decompression begins, so bugs in individual codec decode loops can be missed
// by the existing harness.
//
// This harness drives:
//   - TIFFReadScanline()        (row-by-row, strips)
//   - TIFFReadEncodedStrip()    (strip-at-a-time)
//   - TIFFReadEncodedTile()     (tile-at-a-time)
//   - TIFFReadRawStrip()        (raw, uncompressed view)
//   - TIFFReadRawTile()         (raw, uncompressed view)
//
// All memory is bounded by TIFF internal limits + our own cap so the fuzzer
// does not OOM.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <tiffio.h>
#include <tiffio.hxx>

// Suppress libtiff diagnostics so they don't pollute fuzzer output.
extern "C" void tiff_error_handler(const char *, const char *, va_list) {}
extern "C" void tiff_warn_handler(const char *, const char *, va_list) {}
extern "C" void tiff_error_handler_ext(void *, const char *, const char *, va_list) {}
extern "C" void tiff_warn_handler_ext(void *, const char *, const char *, va_list) {}

// Maximum number of bytes we'll allocate for a single read buffer.
// Keeps peak RSS sane even on adversarial inputs.
static constexpr uint64_t kMaxBufBytes = 64 * 1024 * 1024; // 64 MB

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    TIFFSetErrorHandler(tiff_error_handler);
    TIFFSetWarningHandler(tiff_warn_handler);
    TIFFSetErrorHandlerExt(tiff_error_handler_ext);
    TIFFSetWarningHandlerExt(tiff_warn_handler_ext);

    // Wrap the fuzz data in a std::istringstream so libtiff can read it via
    // the C++ stream interface (no temp file needed).
    std::istringstream iss(std::string(reinterpret_cast<const char *>(data), size));

    TIFF *tif = TIFFStreamOpen("fuzz_input", &iss);
    if (!tif) {
        return 0;
    }

    uint32_t width = 0, height = 0, rows_per_strip = 0;
    uint16_t samples_per_pixel = 1, bits_per_sample = 8;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);

    if (width == 0 || height == 0) {
        TIFFClose(tif);
        return 0;
    }

    const int is_tiled = TIFFIsTiled(tif);

    if (!is_tiled) {
        // ---- Strip-based image ----

        // TIFFReadScanline path
        {
            tsize_t scanline_size = TIFFScanlineSize(tif);
            if (scanline_size > 0 && (uint64_t)scanline_size <= kMaxBufBytes) {
                void *buf = _TIFFmalloc(scanline_size);
                if (buf) {
                    for (uint32_t row = 0; row < height; ++row) {
                        if (TIFFReadScanline(tif, buf, row, 0) < 0) {
                            break;
                        }
                    }
                    _TIFFfree(buf);
                }
            }
        }

        // TIFFReadEncodedStrip path
        {
            tstrip_t nstrips = TIFFNumberOfStrips(tif);
            tsize_t strip_size = TIFFStripSize(tif);
            if (strip_size > 0 && (uint64_t)strip_size <= kMaxBufBytes && nstrips > 0) {
                void *buf = _TIFFmalloc(strip_size);
                if (buf) {
                    for (tstrip_t strip = 0; strip < nstrips; ++strip) {
                        TIFFReadEncodedStrip(tif, strip, buf, strip_size);
                    }
                    _TIFFfree(buf);
                }
            }
        }

        // TIFFReadRawStrip path
        {
            tstrip_t nstrips = TIFFNumberOfStrips(tif);
            if (nstrips > 0) {
                // Find the maximum raw strip size across all strips so our
                // single buffer is large enough for any strip.
                tsize_t max_raw_size = 0;
                for (tstrip_t s = 0; s < nstrips; ++s) {
                    tsize_t sz = TIFFRawStripSize(tif, s);
                    if (sz > max_raw_size) max_raw_size = sz;
                }
                if (max_raw_size > 0 && (uint64_t)max_raw_size <= kMaxBufBytes) {
                    void *buf = _TIFFmalloc(max_raw_size);
                    if (buf) {
                        for (tstrip_t strip = 0; strip < nstrips; ++strip) {
                            tsize_t this_raw = TIFFRawStripSize(tif, strip);
                            if (this_raw > 0) {
                                TIFFReadRawStrip(tif, strip, buf, this_raw);
                            }
                        }
                        _TIFFfree(buf);
                    }
                }
            }
        }
    } else {
        // ---- Tile-based image ----

        // TIFFReadEncodedTile path
        {
            ttile_t ntiles = TIFFNumberOfTiles(tif);
            tsize_t tile_size = TIFFTileSize(tif);
            if (tile_size > 0 && (uint64_t)tile_size <= kMaxBufBytes && ntiles > 0) {
                void *buf = _TIFFmalloc(tile_size);
                if (buf) {
                    for (ttile_t tile = 0; tile < ntiles; ++tile) {
                        TIFFReadEncodedTile(tif, tile, buf, tile_size);
                    }
                    _TIFFfree(buf);
                }
            }
        }

        // TIFFReadRawTile path
        {
            ttile_t ntiles = TIFFNumberOfTiles(tif);
            tsize_t raw_tile_size = TIFFTileSize(tif); // upper bound
            if (raw_tile_size > 0 && (uint64_t)raw_tile_size <= kMaxBufBytes && ntiles > 0) {
                void *buf = _TIFFmalloc(raw_tile_size);
                if (buf) {
                    for (ttile_t tile = 0; tile < ntiles; ++tile) {
                        TIFFReadRawTile(tif, tile, buf, raw_tile_size);
                    }
                    _TIFFfree(buf);
                }
            }
        }
    }

    TIFFClose(tif);
    return 0;
}
