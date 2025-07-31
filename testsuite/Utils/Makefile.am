#
# Makefile.am for libstorage/testsuite/Utils
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la -lboost_unit_test_framework

check_PROGRAMS = enum.test udev-encoding.test humanstring.test region.test	\
	exception.test topology.test alignment.test math.test systemcmd.test	\
	dirname.test basename.test algorithm.test format.test join.test 	\
	regex.test sort-by.test jsonfile.test rootprefix.test			\
	udev-filters.test dm-encoding.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

AM_TESTS_ENVIRONMENT = BOOST_TEST_CATCH_SYSTEM_ERRORS=no

