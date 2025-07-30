#
# Makefile.am for libstorage/testsuite/probe
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	disk.test disk-zoned1.test bitlocker1.test bitlocker2.test		\
	multipath1.test multipath+luks1.test md1.test md2.test			\
	md3.test lvm1.test lvm2.test lvm-cache1.test lvm-cache+thin1.test	\
	lvm-raid1.test lvm-mirror1.test lvm-snapshot1.test			\
	lvm-errors1.test lvm-unsupported1.test integrity.test			\
	luks+lvm1.test lvm+luks1.test luks1.test luks2.test luks3.test		\
	luks4.test								\
	multi-mount-point1.test multi-mount-point2.test				\
	multi-mount-point3.test multi-mount-point4.test				\
	bcache1.test bcache2.test btrfs1.test btrfs2.test btrfs3.test		\
	btrfs4.test btrfs5.test swap1.test tmpfs1.test				\
	dasd1.test dasd2.test dasd3.test external-journal.test			\
	dmraid1.test md-imsm1.test md-ddf1.test nfs1.test ntfs1.test xen1.test	\
	ambiguous1.test ambiguous2.test md+lvm1.test plain-encryption1.test	\
	missing1.test error1.test prefixed1.test prefixed2.test			\
	unsupported1.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

EXTRA_DIST = disk-mockup.xml disk-devicegraph.xml				\
	disk-zoned1-mockup.xml disk-zoned1-devicegraph.xml			\
	bitlocker1-mockup.xml bitlocker1-devicegraph.xml			\
	bitlocker2-mockup.xml bitlocker2-devicegraph.xml			\
	multi-mount-point-mockup1.xml multi-mount-point-devicegraph1.xml	\
	multi-mount-point-mockup2.xml						\
	multi-mount-point-mockup3.xml						\
	multi-mount-point-mockup4.xml						\
	multipath1-mockup.xml multipath1-devicegraph.xml			\
	multipath+luks1-mockup.xml multipath+luks1-devicegraph.xml		\
	dmraid1-mockup.xml dmraid1-devicegraph.xml				\
	xen1-mockup.xml xen1-devicegraph.xml					\
	lvm1-mockup.xml lvm1-devicegraph.xml					\
	lvm2-mockup.xml lvm2-devicegraph.xml					\
	lvm-cache1-mockup.xml lvm-cache1-devicegraph.xml			\
	lvm-cache+thin1-mockup.xml lvm-cache+thin1-devicegraph.xml		\
	lvm-raid1-mockup.xml lvm-raid1-devicegraph.xml				\
	lvm-mirror1-mockup.xml lvm-mirror1-devicegraph.xml			\
	lvm-snapshot1-mockup.xml lvm-snapshot1-devicegraph.xml			\
	lvm-errors1-mockup.xml lvm-errors1-devicegraph.xml			\
	lvm-unsupported1-mockup.xml lvm-unsupported1-devicegraph.xml		\
	integrity-mockup.xml integrity-devicegraph.xml				\
	plain-encryption1-mockup.xml plain-encryption1-devicegraph.xml		\
	luks+lvm1-mockup.xml luks+lvm1-devicegraph.xml				\
	luks1-mockup.xml luks1-devicegraph.xml					\
	luks2-mockup.xml luks2-devicegraph.xml					\
	luks3-mockup.xml luks3-devicegraph.xml					\
	luks4-mockup.xml luks4-devicegraph.xml					\
	lvm+luks1-mockup.xml lvm+luks1-devicegraph.xml				\
	bcache1-mockup.xml bcache1-devicegraph.xml				\
	bcache2-mockup.xml bcache2-devicegraph.xml				\
	btrfs1-mockup.xml btrfs1-devicegraph.xml				\
	btrfs2-mockup.xml btrfs2-devicegraph.xml				\
	btrfs3-mockup.xml btrfs3-devicegraph.xml				\
	btrfs4-mockup.xml btrfs4-devicegraph.xml				\
	btrfs5-mockup.xml btrfs5-devicegraph.xml				\
	swap1-mockup.xml swap1-devicegraph.xml					\
	tmpfs1-mockup.xml tmpfs1-devicegraph.xml				\
	dasd1-mockup.xml dasd1-devicegraph.xml					\
	dasd2-mockup.xml dasd2-devicegraph.xml					\
	dasd3-mockup.xml dasd3-devicegraph.xml					\
	md1-mockup.xml md1-devicegraph.xml					\
	md2-mockup.xml md2-devicegraph.xml					\
	md3-mockup.xml md3-devicegraph.xml					\
	md-imsm1-mockup.xml md-imsm1-devicegraph.xml				\
	md-ddf1-mockup.xml md-ddf1-devicegraph.xml				\
	nfs1-mockup.xml nfs1-devicegraph.xml					\
	ntfs1-mockup.xml ntfs1-devicegraph.xml					\
	external-journal-mockup.xml external-journal-devicegraph.xml		\
	ambiguous1-mockup.xml ambiguous1-devicegraph.xml			\
	ambiguous2-mockup.xml ambiguous2-devicegraph.xml			\
	md+lvm1-mockup.xml md+lvm1-devicegraph.xml				\
	prefixed1-mockup.xml prefixed1-devicegraph.xml				\
	prefixed2-mockup.xml prefixed2-devicegraph.xml				\
	missing1-mockup.xml							\
	error1-mockup.xml							\
	unsupported1-mockup.xml unsupported1-devicegraph.xml
