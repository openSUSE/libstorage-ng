#
# Makefile.am for libstorage/storage
#

SUBDIRS = Devices Holders Utils

lib_LTLIBRARIES = libstorage.la

libstorage_la_SOURCES =					\
	Environment.h		Environment.cc		\
	EnvironmentImpl.h	EnvironmentImpl.cc	\
	Storage.h		Storage.cc		\
	StorageImpl.h		StorageImpl.cc		\
	Devicegraph.h		Devicegraph.cc		\
	DevicegraphImpl.h	DevicegraphImpl.cc	\
	Action.h		Action.cc		\
	Actiongraph.h		Actiongraph.cc

libstorage_la_LDFLAGS = -version-info 7:0:2
libstorage_la_LIBADD =			\
	Devices/libdevices.la		\
	Holders/libholders.la		\
	Utils/libutils.la 		\
	-lxml2

pkgincludedir = $(includedir)/storage

pkginclude_HEADERS =		\
	Environment.h		\
	Storage.h		\
	Devicegraph.h

