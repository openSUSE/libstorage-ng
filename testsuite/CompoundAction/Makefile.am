#
# Makefile.am for libstorage/testsuite/CompoundAction
#

noinst_LTLIBRARIES = libcompoundactionfixture.la

libcompoundactionfixture_la_SOURCES = Fixture.h

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la libcompoundactionfixture.la -lboost_unit_test_framework

check_PROGRAMS =				\
	bcache-sentence.test			\
	flashonly-bcache-sentence.test		\
	btrfs-subvolume-sentence.test		\
	btrfs-quota-sentence.test		\
	encrypted-sentence.test			\
	is-delete.test				\
	lvm-lv-sentence.test			\
	lvm-vg-sentence.test			\
	md-sentence.test			\
	nfs-sentence.test			\
	partition-sentence.test			\
	stray-blk-device-sentence.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = devicegraph.xml

