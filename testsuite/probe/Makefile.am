#
# Makefile.am for libstorage/testsuite/probe
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	disk.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = disk-mockup.xml disk-devicegraph.xml

