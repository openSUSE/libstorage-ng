#
# Makefile.am for libstorage/testsuite/sorting
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage.la -lboost_unit_test_framework

check_PROGRAMS =								\
	disk1.test md1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

