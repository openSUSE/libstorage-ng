#
# Makefile.am for libstorage/testsuite/SystemInfo
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage.la -lboost_unit_test_framework

check_PROGRAMS =								\
	blkid.test btrfs.test cryptsetup.test dasdview.test dir.test		\
	dmraid.test								\
	dmsetup-info.test lsscsi.test						\
	majorminor.test mdadm-detail.test mdadm-examine.test mdlinks.test	\
	parted.test								\
	proc-mdstat.test proc-mounts.test proc-parts.test udevmap.test		\
	vgdisplay.test vgs.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

