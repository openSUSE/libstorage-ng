#
# Makefile.am for libstorage
#

VERSION = $(shell utils/git2log --version VERSION ; cat VERSION)

SUBDIRS = storage doc po utils examples testsuite bindings integration-tests

AUTOMAKE_OPTIONS = foreign dist-xz no-dist-gzip subdir-objects

doc_DATA = AUTHORS LICENSE

EXTRA_DIST = $(doc_DATA) VERSION LIBVERSION

libstorage-ng-$(VERSION).tar.xz: dist-xz

package: libstorage-ng-$(VERSION).tar.xz
	rm -f package/libstorage-ng-*.tar.xz
	mv libstorage-ng-$(VERSION).tar.xz package/

changes:
	utils/git2log --changelog --format obs package/libstorage-ng.changes

archive:
	utils/make_package --name libstorage-ng
