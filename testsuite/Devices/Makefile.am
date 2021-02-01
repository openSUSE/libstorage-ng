#
# Makefile.am for libstorage/testsuite/Devices
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la -lboost_unit_test_framework

check_PROGRAMS =								\
	lvm-vg.test md-underlying-size.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

