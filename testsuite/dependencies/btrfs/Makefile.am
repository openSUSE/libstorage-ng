#
# Makefile.am for libstorage/testsuite/dependencies/btrfs
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	create1.test create2.test delete1.test delete2.test cycle1.test		\
	cycle2.test extend1.test replace1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =										\
	create1-probed.xml create1-staging.xml create1-expected.txt			\
	create2-probed.xml create2-staging.xml create2-expected.txt create2-mockup.xml	\
	delete1-probed.xml delete1-staging.xml delete1-expected.txt			\
	delete2-probed.xml delete2-staging.xml delete2-expected.txt			\
	cycle1-probed.xml cycle1-staging.xml cycle1-expected.txt			\
	cycle2-probed.xml cycle2-staging.xml cycle2-expected.txt			\
	extend1-probed.xml extend1-staging.xml extend1-expected.txt			\
	replace1-probed.xml replace1-staging.xml replace1-expected.txt

