#
# Makefile.am for libstorage/bindings/perl5
#

SUBDIRS = examples

EXTRA_DIST = storage-perl5.i

MAINTAINERCLEANFILES = storage_wrap.cxx

BUILT_SOURCES = storage_wrap.cxx

# TODO
perlextensiondirdir = /usr/lib/perl5/site_perl

AM_CXXFLAGS = -fPIC -DPIC

# TODO
INCLUDES = -I. -I${top_srcdir} -I/usr/lib/perl5/5.20.1/x86_64-linux-thread-multi/CORE/

DEPENDENCIES =						\
	${top_srcdir}/storage/Devices/Device.h		\
	${top_srcdir}/storage/Devices/Disk.h		\
	${top_srcdir}/storage/Devices/Gpt.h		\
	${top_srcdir}/storage/DeviceGraph.h

perlextensiondir_LTLIBRARIES = storage.la
nodist_storage_la_SOURCES = storage_wrap.cxx

storage_la_LIBADD = $(top_builddir)/storage/libstorage.la
storage_la_LDFLAGS = -no-undefined -module -avoid-version

storage_la_DEPENDENCIES =				\
	$(DEPENDENCIES)					\
	${top_builddir}/storage/libstorage.la

storage.pm storage_wrap.cxx: ../storage.i storage-perl5.i $(DEPENDENCIES)
	swig -o storage_wrap.cxx -c++ -perl5 ${INCLUDES} storage-perl5.i

# TODO
install-data-hook:
	install storage.pm /usr/lib/perl5/site_perl

CLEANFILES = storage.pm storage_wrap.cxx

