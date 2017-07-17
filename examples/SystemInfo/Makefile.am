#
# Makefile.am for libstorage/examples/SystemInfo
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la

noinst_PROGRAMS = test-blkid test-cmd-btrfs-filesystem-show test-cmddmsetupinfo	\
	test-cmddmsetuptable test-cmdlvs test-cmdpvs 				\
	test-cmdvgs test-cmdudevadminfo test-dir test-dmraid test-lsattr 	\
	test-lsscsi								\
	test-mdadmdetail test-mdadmexamine test-mdlinks test-multipath		\
	test-parted								\
	test-procmdstat test-procmounts test-procparts test-cmddf 		\
	test-cmd-btrfs-subvolume-list test-cmd-btrfs-subvolume-get-default

AM_DEFAULT_SOURCE_EXT = .cc

