#
# Makefile.am for libstorage/testsuite/dependencies/md
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	reduce1.test extend1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	reduce1-probed.xml reduce1-staging.xml reduce1-expected.txt		\
	extend1-probed.xml extend1-staging.xml extend1-expected.txt

