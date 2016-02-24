#
# Makefile.am for libstorage/bindings/python
#

SUBDIRS = . examples testsuite

EXTRA_DIST = storage-python.i

AM_CXXFLAGS = -fPIC -DPIC

AM_CPPFLAGS = -I. -I${top_srcdir} ${PYTHON_INCLUDES}

include ../storage.deps

LIBS = $(top_builddir)/storage/libstorage-ng.la ${PYTHON_LIBS}

python_PYTHON = storage.py

pyexec_LTLIBRARIES = _storage.la

_storage_la_LDFLAGS = -module -avoid-version
_storage_la_HEADERS: $(top_builddir)/config.h

_storage_la_DEPENDENCIES =				\
	$(DEPENDENCIES)					\
	${top_builddir}/storage/libstorage-ng.la

nodist__storage_la_SOURCES = storage_wrap.cxx

storage.py storage_wrap.cxx: storage-python.i $(DEPENDENCIES)
	swig -o storage_wrap.cxx -c++ -python ${AM_CPPFLAGS} storage-python.i

CLEANFILES = storage.py storage_wrap.cxx

