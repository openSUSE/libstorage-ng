#
# Makefile.am for libstorage
#

SUBDIRS = storage doc po utils examples testsuite bindings integration-tests

AUTOMAKE_OPTIONS = foreign dist-bzip2 no-dist-gzip subdir-objects

doc_DATA = AUTHORS LICENSE

EXTRA_DIST = $(doc_DATA) VERSION LIBVERSION

libstorage-ng-$(VERSION).tar.bz2: dist-bzip2

package: libstorage-ng-$(VERSION).tar.bz2
	rm -f package/libstorage-ng-*.tar.bz2
	mv libstorage-ng-$(VERSION).tar.bz2 package/

changes:
	utils/git2log --changelog --format obs package/libstorage-ng.changes

archive:
	utils/make_package --name libstorage-ng
