#
# Makefile.am for libstorage/testsuite/actions
#

SUBDIRS = ext4 btrfs

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	dasd1.test md1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =										\
	dasd1-probed.xml dasd1-staging.xml dasd1-expected.txt dasd1-mockup.xml		\
	md1-probed.xml md1-staging.xml md1-expected.txt md1-mockup.xml

