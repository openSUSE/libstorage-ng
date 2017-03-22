#
# Makefile.am for libstorage/testsuite/commented-config-file
#

AM_CPPFLAGS = -I$(top_srcdir)/src

LDADD = ../../storage/libstorage-ng.la -lboost_unit_test_framework

check_PROGRAMS =		\
	simple_string_ops.test	\
	container_ops.test	\
	parser.test		\
	formatter.test		\
	diff.test

AM_DEFAULT_SOURCE_EXT = .cc

TESTS = $(check_PROGRAMS)
