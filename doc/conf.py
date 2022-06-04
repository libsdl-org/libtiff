# -*- coding: utf-8 -*-
# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# Substitutions from external build system.
srcdir = None
builddir = None
ext_source_branch = None
ext_source_user = None


# -- Project information -----------------------------------------------------

project = 'LibTIFF'
copyright = '1988-2022, LibTIFF contributors'
author = 'LibTIFF contributors'

# The full version, including alpha/beta/rc tags
release = 'UNDEFINED'
version = 'UNDEFINED'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.extlinks'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

extlinks = {
    # GitLab links
    'issue' : ('https://gitlab.com/libtiff/libtiff/-/issues/%s', 'issue #%s'),
    'merge' : ('https://gitlab.com/libtiff/libtiff/-/merge_requests/%s', 'merge request #%s'),
    'commit' : ('https://gitlab.com/libtiff/libtiff/-/commit/%s', 'commit %s'),
    'branch' : ('https://gitlab.com/libtiff/libtiff/-/tree/%s', 'branch %s'),
    'tag' : ('https://gitlab.com/libtiff/libtiff/-/tags/%s', 'tag %s'),

    # Old Bugzilla
    'bugzilla' : ('http://bugzilla.maptools.org/show_bug.cgi?id=%s', 'MapTools bugzilla #%s'),
    'bugzilla-rs' : ('http://bugzilla.remotesensing.org/show_bug.cgi?id=%s', 'Remote Sensing bugzilla #%s [no longer available]'),

    # GDAL
    'gdal-trac' : ('http://trac.osgeo.org/gdal/ticket/%s', 'GDAL trac #%s'),
    'oss-fuzz' : ('https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=%s', 'OSS-Fuzz #%s'),

    # Security
    'cve' : ('https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-%s', 'CVE-%s'),
}


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_extra_path = ['static']
