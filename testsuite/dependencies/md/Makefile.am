#
# Makefile.am for libstorage/testsuite/dependencies/md
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	create1.test reduce1.test extend1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =											\
	create1-probed.xml create1-staging.xml create1-expected.txt create1-mockup.xml		\
	reduce1-probed.xml reduce1-staging.xml reduce1-expected.txt reduce1-mockup.xml		\
	extend1-probed.xml extend1-staging.xml extend1-expected.txt extend1-mockup.xml

