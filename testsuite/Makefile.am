#
# Makefile.am for libstorage/testsuite
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../storage/libstorage.la -lboost_unit_test_framework

check_PROGRAMS =									\
	blkid.test btrfs.test								\
	copy.test dependencies.test disk.test dynamic.test				\
	find-vertex.test humanstring.test lsscsi.test output.test			\
	mdadm-detail.test mdadm-examine.test						\
	parted.test probe.test								\
	proc-mdstat.test proc-mounts.test proc-parts.test stable.test relatives.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = probe.xml

