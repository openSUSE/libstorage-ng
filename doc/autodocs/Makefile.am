#
# Makefile.am for libstorage/doc/autodocs
#

SUBDIRS = . html xml

doxygen.timestamp: doxygen.conf $(srcdir)/../../storage/*.h $(srcdir)/../../storage/*/*.h
	doxygen doxygen.conf
	touch doxygen.timestamp

all: doxygen.timestamp

CLEANFILES = doxygen.timestamp

EXTRA_DIST = doxygen.conf

