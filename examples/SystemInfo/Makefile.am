#
# Makefile.am for libstorage/examples/SystemInfo
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la

noinst_PROGRAMS = test-blkid test-cmd-btrfs-filesystem-show test-cmddmsetupinfo	\
	test-cmddmsetuptable test-cmdlvs test-cmdpvs 				\
	test-cmdvgs test-cmdudevadminfo test-dasdview test-dir test-dmraid	\
	test-cmd-dumpe2fs test-cmd-resize2fs test-cmd-ntfsresize		\
	test-lsattr test-lsscsi							\
	test-mdadmdetail test-mdadmexamine test-mdlinks test-multipath		\
	test-parted test-cmdstat						\
	test-procmdstat test-procmounts test-cmddf 				\
	test-cmd-btrfs-subvolume-list test-cmd-btrfs-subvolume-get-default	\
	test-cmd-btrfs-filesystem-df test-cmd-cryptsetup-luks-dump

AM_DEFAULT_SOURCE_EXT = .cc

