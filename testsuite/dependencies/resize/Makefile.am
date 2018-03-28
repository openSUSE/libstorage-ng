#
# Makefile.am for libstorage/testsuite/dependencies/resize
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	resize1.test resize2.test resize3.test resize4.test resize5.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	resize1-probed.xml resize1-staging.xml resize1-expected.txt		\
	resize2-probed.xml resize2-staging.xml resize2-expected.txt		\
	resize3-probed.xml resize3-staging.xml resize3-expected.txt		\
	resize4-probed.xml resize4-staging.xml resize4-expected.txt		\
	resize5-probed.xml resize5-staging.xml resize5-expected.txt

