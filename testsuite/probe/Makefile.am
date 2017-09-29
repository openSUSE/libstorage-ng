#
# Makefile.am for libstorage/testsuite/probe
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	disk.test multipath1.test md1.test md2.test md3.test lvm1.test		\
	lvm2.test 								\
	luks1.test bcache1.test btrfs1.test dasd1.test external-journal.test	\
	dmraid1.test md-imsm1.test md-ddf1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = disk-mockup.xml disk-devicegraph.xml				\
	multipath1-mockup.xml multipath1-devicegraph.xml			\
	dmraid1-mockup.xml dmraid1-devicegraph.xml				\
	lvm1-mockup.xml lvm1-devicegraph.xml					\
	lvm2-mockup.xml lvm2-devicegraph.xml					\
	luks1-mockup.xml luks1-devicegraph.xml					\
	bcache1-mockup.xml bcache1-devicegraph.xml				\
	btrfs1-mockup.xml btrfs1-devicegraph.xml				\
	dasd1-mockup.xml dasd1-devicegraph.xml					\
	md1-mockup.xml md1-devicegraph.xml					\
	md2-mockup.xml md2-devicegraph.xml					\
	md3-mockup.xml md3-devicegraph.xml					\
	md-imsm1-mockup.xml md-imsm1-devicegraph.xml				\
	md-ddf1-mockup.xml md-ddf1-devicegraph.xml				\
	external-journal-mockup.xml external-journal-devicegraph.xml

