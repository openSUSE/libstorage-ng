#
# Makefile.am for libstorage/testsuite/dependencies
#

SUBDIRS = luks lvm md partitions btrfs dasd-pt

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	test1.test test2.test test3.test test4.test test5.test grow1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	test1-probed.xml test1-staging.xml test1-expected.txt			\
	test3-probed.xml test3-staging.xml test3-expected.txt			\
	test4-probed.xml test4-staging.xml test4-expected.txt			\
	test5-probed.xml test5-staging.xml test5-expected.txt			\
	grow1-probed.xml grow1-staging.xml grow1-expected.txt

