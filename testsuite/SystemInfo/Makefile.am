#
# Makefile.am for libstorage/testsuite/SystemInfo
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la -lboost_unit_test_framework

check_PROGRAMS =								\
	blkid.test btrfs.test cryptsetup.test dasdview.test dir.test		\
	dmraid.test								\
	dmsetup-info.test dmsetup-table.test lsscsi.test lvs.test		\
	mdadm-detail.test mdadm-examine.test mdlinks.test			\
	parted.test								\
	proc-mdstat.test proc-mounts.test proc-parts.test pvs.test		\
	udevadm-info.test vgs.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

