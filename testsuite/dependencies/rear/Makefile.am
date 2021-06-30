#
# Makefile.am for libstorage/testsuite/dependencies/rear
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	rear1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =										\
	rear1-probed.xml rear1-staging.xml rear1-expected.txt

