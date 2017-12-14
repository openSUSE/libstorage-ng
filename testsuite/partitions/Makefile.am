#
# Makefile.am for libstorage/testsuite/partitions
#

AM_CPPFLAGS = -I$(top_srcdir)

LDADD = ../../storage/libstorage-ng.la ../helpers/libhelpers.la			\
	-lboost_unit_test_framework

check_PROGRAMS =								\
	get1.test size.test slots.test names.test udev1.test attributes.test	\
	set-number.test msdos-delete1.test dasd-create1.test dasd-delete1.test	\
	dasd-implicit1.test gpt-delete1.test					\
	surrounding.test resize-info.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)

