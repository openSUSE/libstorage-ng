#
# Makefile.am for libstorage/testsuite/probe
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	disk.test md1.test md2.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = disk-mockup.xml disk-devicegraph.xml				\
	md1-mockup.xml md1-devicegraph.xml					\
	md2-mockup.xml md2-devicegraph.xml

