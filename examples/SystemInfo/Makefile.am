#
# Makefile.am for libstorage/examples/SystemInfo
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la

noinst_PROGRAMS = test-blkid test-cmd-btrfs-filesystem-show			\
	test-cmd-dmsetup-info test-cmd-dmsetup-table test-cmd-lvs test-cmd-pvs \
	test-cmd-vgs test-cmd-udevadm-info test-dasdview test-dir test-dmraid	\
	test-cmd-dumpe2fs test-cmd-resize2fs test-cmd-ntfsresize		\
	test-lsattr test-lsscsi							\
	test-mdadm-detail test-mdadm-examine test-mdlinks test-multipath	\
	test-parted test-cmd-stat test-cmd-blockdev				\
	test-proc-mdstat test-proc-mounts test-cmd-df 				\
	test-cmd-btrfs-subvolume-list test-cmd-btrfs-subvolume-get-default	\
	test-cmd-btrfs-filesystem-df test-cmd-btrfs-qgroup-show 		\
	test-cmd-cryptsetup-luks-dump

AM_DEFAULT_SOURCE_EXT = .cc

