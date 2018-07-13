#
# Makefile.am for libstorage/testsuite/dependencies/bcache
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	create1.test delete1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	create1-probed.xml create1-staging.xml create1-expected.txt		\
	delete1-probed.xml delete1-staging.xml delete1-expected.txt

