#
# Makefile.am for libstorage/testsuite/helpers
#

noinst_LTLIBRARIES = libhelpers.la

libhelpers_la_SOURCES =						\
	TsCmp.cc		TsCmp.h				\
	CallbacksRecorder.cc	CallbacksRecorder.h		\
	output.h

noinst_PROGRAMS =	\
	echoargs	\
	echoargs-stderr	\
	echoargs-mixed	\
	repeat		\
	retcode		\
	segfaulter	\
	test-systemcmd

AM_DEFAULT_SOURCE_EXT = .cc

test_systemcmd_LDADD = ../../storage/libstorage-ng.la
test_systemcmd_LDFLAGS = -no-install
