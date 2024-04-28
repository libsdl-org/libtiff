.. _rfc2_restoring_needed_tools:

=====================================
RFC 2: Restoring Needed LibTIFF Tools
=====================================

Author: Su Laus

Contact: (@Su_Laus)

Status: Proposed

Summary
-------

The purpose of this RFC is to clarify if and which tools that were moved
to the archive in libtiff 4.6.0 should be reactivated.

Prehistory
----------

The tools in libtiff caused many vulnerabilities
and CVEs that were attributed to the libtiff library itself.
Trying to fix the security holes in the tools turned out to be a
Sisyphean task (can never be done). 
Therefore, most of the tools in libtiff 4.6.0 were moved to the archive
and the existing problems were closed with "wontfix-unmaintained".

It was later understood that some users depend on some of these archived tools.

Some problems with the tools have now been fixed
(see e.g. https://gitlab.com/libtiff/libtiff/-/merge_requests/569).

Proposed procedure
------------------

* All tools as of libtiff 4.5.1 shall be restored.
* Bugfixes in MR !569 are applied in single merge requests for traceability
  and selectively as some changes might not be applicable.
* Remove “wontfix-unmaintained” from closed issues, when fixed.
* All issues related to utilities / tools shall get label “utility”.
* The documentation and other references shall point to
  https://libtiff.gitlab.io/libtiff/.
* After an initial merge has been applied for restoring the tools,
  the http://www.libtiff.org page shall be reset as a mirror of
  https://libtiff.gitlab.io/libtiff/.
* Finally release as 4.7.0 when all known issues of the tools are closed.

References to previous contributions to the discussion
------------------------------------------------------
https://gitlab.com/libtiff/libtiff/-/issues/580 and related merge request, 
https://www.asmail.be/msg0054917226.html, 
https://www.asmail.be/msg0055015786.html, 
https://gitlab.com/libtiff/libtiff/-/merge_requests/569,
and discussion in https://gitlab.com/libtiff/libtiff/-/merge_requests/581

Voting history
--------------

### to be filled after comments and votes ###
