#
# Makefile.am for libstorage/testsuite/dependencies/partitions
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	rename1.test rename2.test rename3.test rename4.test dasd1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	rename1-probed.xml rename1-staging.xml rename1-expected.txt		\
	rename2-probed.xml rename2-staging.xml rename2-expected.txt		\
	rename3-probed.xml rename3-staging.xml rename3-expected.txt		\
	rename4-probed.xml rename4-staging.xml rename4-expected.txt		\
	dasd1-probed.xml dasd1-staging.xml dasd1-expected.txt dasd1-mockup.xml

