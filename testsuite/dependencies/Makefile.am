#
# Makefile.am for libstorage/testsuite/dependencies
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage.la -lboost_unit_test_framework

check_PROGRAMS =								\
	test1.test test2.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = test1-probed.xml test1-staging.xml

