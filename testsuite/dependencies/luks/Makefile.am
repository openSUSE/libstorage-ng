#
# Makefile.am for libstorage/testsuite/dependencies/luks
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	create.test delete.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	create-probed.xml create-staging.xml create-expected.txt		\
	delete-probed.xml delete-staging.xml delete-expected.txt

