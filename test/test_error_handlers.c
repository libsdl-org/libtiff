/*
 * Copyright (c) 2022, Even Rouault <even.rouault at spatialys.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * TIFF Library
 *
 * Test error handlers
 */

#include "tif_config.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "tiffio.h"

#define ERROR_STRING_SIZE 1024

typedef struct MyErrorHandlerUserDataStruct
{
    char*  buffer;
    size_t buffer_size;
    TIFF*  tif_got_from_callback;
    char   module[64];
} MyErrorHandlerUserDataStruct;

static int myErrorHandler(TIFF* tiff, void* user_data, const char* module, const char* fmt, va_list ap)
{
    MyErrorHandlerUserDataStruct* errorhandler_user_data = (MyErrorHandlerUserDataStruct*)user_data;
    vsnprintf(errorhandler_user_data->buffer,
              errorhandler_user_data->buffer_size,
              fmt,
              ap);
    errorhandler_user_data->tif_got_from_callback = tiff;
    snprintf(errorhandler_user_data->module, sizeof(errorhandler_user_data->module), "%s", module);
    return 1;
}

int test_open_ext(int handlers_set_in_open)
{
    int ret = 0;
    char error_buffer[ERROR_STRING_SIZE] = {0};
    char warn_buffer[ERROR_STRING_SIZE] = {0};
    MyErrorHandlerUserDataStruct errorhandler_user_data =
    {
        .buffer = error_buffer,
        .buffer_size = ERROR_STRING_SIZE
    };
    MyErrorHandlerUserDataStruct warnhandler_user_data =
    {
        .buffer = warn_buffer,
        .buffer_size = ERROR_STRING_SIZE
    };
    TIFF* tif;
    if( handlers_set_in_open )
    {
        TIFFOpenExtStruct arguments = {
            .version = 1,
            .errorhandler = myErrorHandler,
            .errorhandler_user_data = &errorhandler_user_data,
            .warnhandler = myErrorHandler,
            .warnhandler_user_data = &warnhandler_user_data
        };
        tif = TIFFOpenExt("test_error_handler.tif", "w", &arguments);
    }
    else
    {
        tif = TIFFOpen("test_error_handler.tif", "w");
        TIFFSetErrorHandlerExtR(tif, myErrorHandler, &errorhandler_user_data);
        TIFFSetWarningHandlerExtR(tif, myErrorHandler, &warnhandler_user_data);
    }
    if( tif == NULL )
    {
        fprintf(stderr, "Cannot create test_error_handler.tif");
        exit(1);
    }

    // Simulate an error emitted by libtiff
    TIFFErrorExtR(tif, "my_error_module", "%s", "some error message");
    if( strcmp(error_buffer, "some error message") != 0 )
    {
        fprintf(stderr, "Did not get expected error message\n");
        ret = 1;
    }
    if( strcmp(errorhandler_user_data.module, "my_error_module") != 0 )
    {
        fprintf(stderr, "Did not get expected error module\n");
        ret = 1;
    }
    if( errorhandler_user_data.tif_got_from_callback != tif)
    {
        fprintf(stderr, "errorhandler_user_data.tif_got_from_callback != tif\n");
        ret = 1;
    }

    // Simulate a warning emitted by libtiff
    TIFFWarningExtR(tif, "my_warning_module", "%s", "some warning message");
    if( strcmp(warn_buffer, "some warning message") != 0 )
    {
        fprintf(stderr, "Did not get expected warning message\n");
        ret = 1;
    }
    if( strcmp(warnhandler_user_data.module, "my_warning_module") != 0 )
    {
        fprintf(stderr, "Did not get expected warning module\n");
        ret = 1;
    }
    if( warnhandler_user_data.tif_got_from_callback != tif)
    {
        fprintf(stderr, "warnhandler_user_data.tif_got_from_callback != tif\n");
        ret = 1;
    }

    TIFFClose(tif);
    unlink("test_error_handler.tif");
    return ret;
}

int main()
{
    int ret = 0;
    ret += test_open_ext(1);
    ret += test_open_ext(0);
    return ret;
}
