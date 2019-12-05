#
# Makefile.am for libstorage/testsuite/dependencies/resize/btrfs
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../../storage/libstorage-ng.la ../../../helpers/libhelpers.la	\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	grow1.test grow-multi1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =												\
	grow1-probed.xml grow1-staging.xml grow1-expected.txt grow1-mockup.xml				\
	grow-multi1-probed.xml grow-multi1-staging.xml grow-multi1-expected.txt grow-multi1-mockup.xml

