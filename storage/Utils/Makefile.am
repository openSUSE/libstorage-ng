#
# Makefile.am for libstorage/storage/Utils
#

noinst_LTLIBRARIES = libutils.la

libutils_la_SOURCES =					\
	AppUtil.cc		AppUtil.h		\
	AsciiFile.cc 		AsciiFile.h		\
	Enum.cc			Enum.h			\
	GraphUtils.h					\
	HumanString.h		HumanString.cc		\
	Lock.cc 		Lock.h			\
	OutputProcessor.cc	OutputProcessor.h	\
	Regex.cc 		Regex.h			\
	Region.cc 		Region.h		\
	RegionImpl.cc 		RegionImpl.h		\
	StorageTmpl.h					\
	SystemCmd.cc		SystemCmd.h		\
	Mockup.cc		Mockup.h		\
	XmlFile.h		XmlFile.cc		\
	StorageDefines.h

$(srcdir)/gen-pathlist.cc: StorageDefines.h
	grep BIN $< | sed -e "s/^[^ ]*[ ]\+//" -e "s/ .*/,/" > $@

BUILT_SOURCES = gen-pathlist.cc

pkgincludedir = $(includedir)/storage/Utils

pkginclude_HEADERS =		\
	HumanString.h		\
	Region.h

