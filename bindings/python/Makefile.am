#
# Makefile.am for libstorage/bindings/python
#

SUBDIRS = examples

EXTRA_DIST = libstorage.i

AM_CXXFLAGS = -fPIC -DPIC

INCLUDES = -I. -I${top_srcdir} -I${PYTHON_INCLUDES}

DEPENDENCIES =						\
	${top_srcdir}/storage/Devices/Device.h		\
	${top_srcdir}/storage/Devices/Disk.h		\
	${top_srcdir}/storage/DeviceGraph.h

LIBS = $(top_builddir)/storage/libstorage.la ${PYTHON_LIBS}

python_PYTHON = libstorage.py

pyexec_LTLIBRARIES = _libstorage.la

_libstorage_la_LDFLAGS = -module -avoid-version
_libstorage_la_HEADERS: $(top_builddir)/config.h

_libstorage_la_DEPENDENCIES =				\
	$(DEPENDENCIES)					\
	${top_builddir}/storage/libstorage.la

nodist__libstorage_la_SOURCES = libstorage_wrap.cxx

libstorage.py libstorage_wrap.cxx: libstorage.i $(DEPENDENCIES)
	swig -o libstorage_wrap.cxx -c++ -python ${INCLUDES} libstorage.i

CLEANFILES = libstorage.py libstorage_wrap.cxx

