#
# Makefile.am for libstorage
#

PACKAGE_NAME = libstorage-ng
VERSION = $(shell utils/git2log --version VERSION ; cat VERSION)
PACKAGE_PREFIX = $(PACKAGE_NAME)-$(VERSION)
BRANCH = $(shell [ -d .git ] && git branch | perl -ne 'print $$_ if s/^\*\s*//')

SUBDIRS = storage doc po utils examples testsuite bindings integration-tests

AUTOMAKE_OPTIONS = foreign dist-bzip2 no-dist-gzip subdir-objects

doc_DATA = AUTHORS LICENSE

EXTRA_DIST = $(doc_DATA) VERSION LIBVERSION

$(PACKAGE_PREFIX).tar.bz2: dist-bzip2

package: $(PACKAGE_PREFIX).tar.bz2
	rm -f package/$(PACKAGE_NAME)-*.tar.bz2
	mv $(PACKAGE_PREFIX).tar.bz2 package/

changes:
	utils/git2log --changelog --format obs package/$(PACKAGE_NAME).changes

archive:
	@if [ ! -d .git ] ; then echo no git repo ; false ; fi
	git archive --prefix=$(PACKAGE_PREFIX)/ $(BRANCH) > package/$(PACKAGE_PREFIX).tar
	tar -r -f package/$(PACKAGE_PREFIX).tar \
	  --mode=0664 --owner=root --group=root \
	  --mtime="`git show -s --format=%ci`" \
	  --transform='s:^:$(PACKAGE_PREFIX)/:' VERSION
	xz -f package/$(PACKAGE_PREFIX).tar
