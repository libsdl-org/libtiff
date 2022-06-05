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
	echo " $(MKDIR_P) '$(DESTDIR)$(docdir)'"; \
	$(MKDIR_P) '$(DESTDIR)$(docdir)' || exit 1; \
	echo "$(MKDIR_P) '$(DESTDIR)$(docdir)/_images'"; \
	$(MKDIR_P) '$(DESTDIR)$(docdir)/_images' || exit 1; \
	echo "$(MKDIR_P) '$(DESTDIR)$(docdir)/_static'"; \
	$(MKDIR_P) '$(DESTDIR)$(docdir)/_static' || exit 1; \
	echo "$(MKDIR_P) '$(DESTDIR)$(docdir)/_sources'"; \
	$(MKDIR_P) '$(DESTDIR)$(docdir)/_sources' || exit 1; \
	echo "$(MKDIR_P) '$(DESTDIR)$(docdir)/man'"; \
	$(MKDIR_P) '$(DESTDIR)$(docdir)/man' || exit 1; \
	echo "$(INSTALL_DATA) html/*.html html/objects.inv html/searchindex.js '$(DESTDIR)$(docdir)'"; \
	$(INSTALL_DATA) html/*.html html/objects.inv html/searchindex.js '$(DESTDIR)$(docdir)'; \
	echo "$(INSTALL_DATA) html/man/*.html '$(DESTDIR)$(docdir)/man'"; \
	$(INSTALL_DATA) html/man/*.html '$(DESTDIR)$(docdir)/man'; \
	echo "$(INSTALL_DATA) html/_images/* '$(DESTDIR)$(docdir)/_images'"; \
	$(INSTALL_DATA) html/_images/* '$(DESTDIR)$(docdir)/_images'; \
	echo "$(INSTALL_DATA) html/_static/* '$(DESTDIR)$(docdir)/_static'"; \
	$(INSTALL_DATA) html/_static/* '$(DESTDIR)$(docdir)/_static'; \
	echo "$(INSTALL_DATA) html/_sources/* '$(DESTDIR)$(docdir)/_sources'"; \
	$(INSTALL_DATA) html/_sources/* '$(DESTDIR)$(docdir)/_sources'

uninstall-local:
	echo "rm -r $(DESTDIR)$(docdir)"
	rm -r $(DESTDIR)$(docdir)

EXTRA_DIST = \
	CMakeLists.txt \
	HtmlDoc.cmake \
	conf.py \
	bigtiffpr_images \
	images \
	static \
	addingtags.rst \
	bigtiffdesign.rst \
	bigtiffpr.rst \
	BigTIFFProposal.rst \
	bugs.rst \
	build.rst \
	contrib.rst \
	document.rst \
	images.rst \
	index.rst \
	internals.rst \
	intro.rst \
	libtiff.rst \
	manpages.rst \
	misc.rst \
	releases.rst \
	support.rst \
	TIFFTechNote2.rst \
	tools.rst \
	v3.4beta007.rst \
	v3.4beta016.rst \
	v3.4beta018.rst \
	v3.4beta024.rst \
	v3.4beta028.rst \
	v3.4beta029.rst \
	v3.4beta031.rst \
	v3.4beta032.rst \
	v3.4beta033.rst \
	v3.4beta034.rst \
	v3.4beta035.rst \
	v3.4beta036.rst \
	v3.5.1.rst \
	v3.5.2.rst \
	v3.5.3.rst \
	v3.5.4.rst \
	v3.5.5.rst \
	v3.5.6beta.rst \
	v3.5.7.rst \
	v3.6.0.rst \
	v3.6.1.rst \
	v3.7.0.rst \
	v3.7.0alpha.rst \
	v3.7.0beta.rst \
	v3.7.0beta2.rst \
	v3.7.1.rst \
	v3.7.2.rst \
	v3.7.3.rst \
	v3.7.4.rst \
	v3.8.0.rst \
	v3.8.1.rst \
	v3.8.2.rst \
	v3.9.0.rst \
	v3.9.0beta.rst \
	v3.9.1.rst \
	v3.9.2.rst \
	v3.9.3.rst \
	v3.9.4.rst \
	v3.9.5.rst \
	v4.0.0.rst \
	v4.0.1.rst \
	v4.0.10.rst \
	v4.0.2.rst \
	v4.0.3.rst \
	v4.0.4.rst \
	v4.0.4beta.rst \
	v4.0.5.rst \
	v4.0.6.rst \
	v4.0.7.rst \
	v4.0.8.rst \
	v4.0.9.rst \
	v4.1.0.rst \
	v4.2.0.rst \
	v4.3.0.rst \
	v4.4.0.rst

.PHONY: doc-html
