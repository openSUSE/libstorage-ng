#
# Makefile.am for libstorage/testsuite/dependencies/resize/ext4
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../../storage/libstorage-ng.la ../../../helpers/libhelpers.la	\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	grow1.test grow2.test shrink1.test shrink2.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =										\
	grow1-probed.xml grow1-staging.xml grow1-expected.txt grow1-mockup.xml		\
	grow2-probed.xml grow2-staging.xml grow2-expected.txt grow2-mockup.xml		\
	shrink1-probed.xml shrink1-staging.xml shrink1-expected.txt shrink1-mockup.xml	\
	shrink2-probed.xml shrink2-staging.xml shrink2-expected.txt shrink2-mockup.xml

