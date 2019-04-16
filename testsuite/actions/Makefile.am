#
# Makefile.am for libstorage/testsuite/actions
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	btrfs1.test dasd1.test md1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =										\
	btrfs1-probed.xml btrfs1-staging.xml btrfs1-expected.txt btrfs1-mockup.xml	\
	dasd1-probed.xml dasd1-staging.xml dasd1-expected.txt dasd1-mockup.xml		\
	md1-probed.xml md1-staging.xml md1-expected.txt md1-mockup.xml

