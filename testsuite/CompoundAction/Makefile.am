#
# Makefile.am for libstorage/testsuite/CompoundAction
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la -lboost_unit_test_framework

check_PROGRAMS = btrfs-subvolume-sentence.test lvm-vg-sentence.test lvm-lv-sentence.test \
		 nfs-sentence.test partition-sentence.test is-delete.test
		 

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

