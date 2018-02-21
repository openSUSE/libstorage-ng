#
# Makefile.am for libstorage/testsuite/dependencies/resize
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	resize1.test resize2.test resize3.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	resize1-probed.xml resize1-staging.xml resize1-expected.txt		\
	resize2-probed.xml resize2-staging.xml resize2-expected.txt		\
	resize3-probed.xml resize3-staging.xml resize3-expected.txt

