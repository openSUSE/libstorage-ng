#
# Makefile.am for libstorage/examples/SystemInfo
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la

noinst_PROGRAMS = test-cmd-blkid test-cmd-btrfs-filesystem-show			\
	test-cmd-dmsetup-info test-cmd-dmsetup-table test-cmd-lvs test-cmd-pvs	\
	test-cmd-vgs test-cmd-udevadm-info test-cmd-dasdview test-dir		\
	test-cmd-dmraid test-cmd-dumpe2fs test-cmd-resize2fs 			\
	test-cmd-ntfsresize test-cmd-lsattr test-cmd-lsscsi test-cmd-nvme-list	\
	test-cmd-nvme-list-subsys test-cmd-mdadm-detail				\
	test-mdlinks test-cmd-multipath						\
	test-cmd-parted test-cmd-stat test-cmd-blockdev				\
	test-proc-mdstat test-proc-mounts test-cmd-df 				\
	test-cmd-btrfs-subvolume-list test-cmd-btrfs-subvolume-get-default	\
	test-cmd-btrfs-filesystem-df test-cmd-btrfs-qgroup-show 		\
	test-cmd-cryptsetup-bitlk-dump test-cmd-cryptsetup-luks-dump

AM_DEFAULT_SOURCE_EXT = .cc

