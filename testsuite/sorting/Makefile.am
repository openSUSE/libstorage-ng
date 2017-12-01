#
# Makefile.am for libstorage/testsuite/sorting
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la -lboost_unit_test_framework

check_PROGRAMS =								\
	format.test disk1.test dasd1.test md1.test partitionable1.test 		\
	blk-device1.test device1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

