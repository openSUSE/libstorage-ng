#
# Makefile.am for libstorage/testsuite/Utils
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la -lboost_unit_test_framework

check_PROGRAMS = enum.test udev-encoding.test humanstring.test region.test	\
	exception.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

