#
# Makefile.am for libstorage/testsuite
#

SUBDIRS = Utils SystemInfo probe dependencies

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../storage/libstorage.la -lboost_unit_test_framework

check_PROGRAMS =								\
	copy.test disk.test dynamic.test					\
	find-vertex.test fstab.test fstab-ng.test output.test			\
	partition-slots.test probe.test range.test				\
	stable.test relatives.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = probe.xml

