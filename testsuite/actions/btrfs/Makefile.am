#
# Makefile.am for libstorage/testsuite/actions/btrfs
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	create1.test grow1.test grow-multi1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =												\
	create1-probed.xml create1-staging.xml create1-expected.txt create1-mockup.xml			\
	grow1-probed.xml grow1-staging.xml grow1-expected.txt grow1-mockup.xml				\
	grow-multi1-probed.xml grow-multi1-staging.xml grow-multi1-expected.txt grow-multi1-mockup.xml

