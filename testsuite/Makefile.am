#
# Makefile.am for libstorage/testsuite
#

SUBDIRS = helpers Utils SystemInfo probe dependencies sorting freeinfo Devices	\
	performance

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../storage/libstorage-ng.la -lboost_unit_test_framework

check_PROGRAMS =								\
	copy.test coverage-tests.test default-partition-table.test disk.test	\
	dynamic.test environment.test find-vertex.test fstab.test fstab-ng.test \
	output.test partition-size.test partition-slots.test probe.test 	\
	range.test stable.test relatives.test systemcmd.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

AM_TESTS_ENVIRONMENT = BOOST_TEST_CATCH_SYSTEM_ERRORS=no

EXTRA_DIST = probe.xml

