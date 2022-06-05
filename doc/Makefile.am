#
# Tag Image File Format (TIFF) Software
#
# Copyright (C) 2004, Andrey Kiselev <dron@ak4719.spb.edu>
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

# Process this file with automake to produce Makefile.in.

docdir = $(LIBTIFF_DOCDIR)/manual

if SPHINX
doc-html:
	$(SPHINX_BUILD) \
	-D "release=$(LIBTIFF_VERSION)" \
	-D "version=$(LIBTIFF_MAJOR_VERSION).$(LIBTIFF_MINOR_VERSION)" \
	-d "$(builddir)/cache" \
	-b html \
	"$(srcdir)" "$(builddir)/html"
endif

html/index.html: doc-html

all-local: html/index.html

clean-local:
	rm -r html
	rm -r cache

install-data-local:
	find html -type d | sed -e 's;^html;;' | while read dir; do \
	  echo " $(MKDIR_P) '$(DESTDIR)$(docdir)$$dir'"; \
	  $(MKDIR_P) '$(DESTDIR)$(docdir)$$dir' || exit 1; \
	done
	find html -type f | sed -e 's;^html;;' | while read docfile; do \
	  docsubdir=$$(dirname $$docfile); \
	  echo "$(INSTALL_DATA) 'html$$docfile' '$(DESTDIR)$(docdir)$$docsubdir'"; \
	  $(INSTALL_DATA) "html$$docfile" "$(DESTDIR)$(docdir)$$docsubdir"; \
	done

uninstall-local:
	echo "rm -r $(DESTDIR)$(docdir)"
	rm -r $(DESTDIR)$(docdir)

rst_sources = \
	./index.rst \
	./build.rst \
	./internals.rst \
	./manpages.rst \
	./project/license.rst \
	./project/index.rst \
	./project/acknowledgements.rst \
	./project/bugs.rst \
	./project/mailinglist.rst \
	./addingtags.rst \
	./specification/index.rst \
	./specification/bigtiff.rst \
	./specification/coverage.rst \
	./specification/technote2.rst \
	./specification/coverage-bigtiff.rst \
	./releases/v3.4beta028.rst \
	./releases/v4.0.9.rst \
	./releases/v3.9.0.rst \
	./releases/v4.0.7.rst \
	./releases/v3.5.7.rst \
	./releases/v4.0.0.rst \
	./releases/v3.7.0beta2.rst \
	./releases/v3.7.1.rst \
	./releases/index.rst \
	./releases/v3.8.0.rst \
	./releases/v4.1.0.rst \
	./releases/historical.rst \
	./releases/v3.5.6beta.rst \
	./releases/v3.6.1.rst \
	./releases/v4.2.0.rst \
	./releases/v3.7.0.rst \
	./releases/v4.0.1.rst \
	./releases/v3.5.1.rst \
	./releases/v4.0.10.rst \
	./releases/v4.0.6.rst \
	./releases/v4.0.8.rst \
	./releases/v3.9.1.rst \
	./releases/v3.4beta029.rst \
	./releases/v4.3.0.rst \
	./releases/v3.4beta031.rst \
	./releases/v3.7.0alpha.rst \
	./releases/v3.6.0.rst \
	./releases/v3.4beta036.rst \
	./releases/v4.0.4beta.rst \
	./releases/v3.8.1.rst \
	./releases/v4.4.0.rst \
	./releases/v3.4beta035.rst \
	./releases/v3.4beta032.rst \
	./releases/v3.4beta007.rst \
	./releases/v3.9.0beta.rst \
	./releases/v3.8.2.rst \
	./releases/v3.4beta018.rst \
	./releases/v3.7.3.rst \
	./releases/v3.4beta016.rst \
	./releases/v3.7.4.rst \
	./releases/v3.5.2.rst \
	./releases/v4.0.5.rst \
	./releases/v3.4beta024.rst \
	./releases/v3.5.5.rst \
	./releases/v4.0.2.rst \
	./releases/v3.9.5.rst \
	./releases/v3.9.2.rst \
	./releases/v3.4beta033.rst \
	./releases/v3.4beta034.rst \
	./releases/v3.9.3.rst \
	./releases/v3.9.4.rst \
	./releases/v3.7.0beta.rst \
	./releases/v3.5.4.rst \
	./releases/v4.0.3.rst \
	./releases/v3.5.3.rst \
	./releases/v4.0.4.rst \
	./releases/v3.7.2.rst \
	./tools.rst \
	./terms.rst \
	./contrib.rst \
	./libtiff.rst \
	./images.rst

EXTRA_DIST = \
	CMakeLists.txt \
	HtmlDoc.cmake \
	conf.py \
	bigtiffpr_images \
	images \
	static \
	$(rst_sources)

.PHONY: doc-html
