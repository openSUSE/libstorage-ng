#
# Makefile.am for libstorage/testsuite/actions/ext4
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	grow1.test shrink1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =										\
	grow1-probed.xml grow1-staging.xml grow1-expected.txt grow1-mockup.xml
	shrink1-probed.xml shrink1-staging.xml shrink1-expected.txt shrink1-mockup.xml

