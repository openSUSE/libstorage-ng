#
# Makefile.am for libstorage/testsuite
#

SUBDIRS = Utils SystemInfo

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../storage/libstorage.la -lboost_unit_test_framework

check_PROGRAMS =								\
	copy.test dependencies.test disk.test dynamic.test			\
	find-vertex.test output.test				\
	probe.test								\
	stable.test relatives.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = probe.xml

