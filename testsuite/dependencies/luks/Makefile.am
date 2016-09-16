#
# Makefile.am for libstorage/testsuite/dependencies/luks
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../../storage/libstorage-ng.la ../../helpers/libhelpers.la		\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	create.test delete.test grow-with-ext4.test shrink-with-lvm-pv.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST =									\
	create-probed.xml create-staging.xml create-expected.txt		\
	delete-probed.xml delete-staging.xml delete-expected.txt		\
	grow-with-ext4-probed.xml grow-with-ext4-staging.xml			\
	grow-with-ext4-expected.txt						\
	shrink-with-lvm-pv-probed.xml shrink-with-lvm-pv-staging.xml		\
	shrink-with-lvm-pv-expected.txt

