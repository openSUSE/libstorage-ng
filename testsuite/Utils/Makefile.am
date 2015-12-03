#
# Makefile.am for libstorage/testsuite/Utils
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage.la -lboost_unit_test_framework

check_PROGRAMS = udev-encoding.test humanstring.test region.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

