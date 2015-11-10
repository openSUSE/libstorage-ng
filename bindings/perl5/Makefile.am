#
# Makefile.am for libstorage/bindings/perl5
#

SUBDIRS = . examples testsuite

EXTRA_DIST = storage-perl5.i

MAINTAINERCLEANFILES = storage_wrap.cxx

BUILT_SOURCES = storage_wrap.cxx

perlextensiondirdir = /usr/lib/perl5/vendor_perl/${PERL5_VERSION}

AM_CXXFLAGS = -fPIC -DPIC

AM_CPPFLAGS = -I. -I${top_srcdir} ${PERL5_INCLUDES}

include ../storage.deps

perlextensiondir_LTLIBRARIES = storage.la
nodist_storage_la_SOURCES = storage_wrap.cxx

storage_la_LIBADD = $(top_builddir)/storage/libstorage.la
storage_la_LDFLAGS = -no-undefined -module -avoid-version

storage_la_DEPENDENCIES =				\
	$(DEPENDENCIES)					\
	${top_builddir}/storage/libstorage.la

storage.pm storage_wrap.cxx: ../storage.i storage-perl5.i $(DEPENDENCIES)
	swig -o storage_wrap.cxx -c++ -perl5 ${AM_CPPFLAGS} storage-perl5.i

install-data-hook:
	install storage.pm $(DESTDIR)/${perlextensiondirdir}

CLEANFILES = storage.pm storage_wrap.cxx

