#
# Makefile.am for libstorage/storage
#

SUBDIRS = Devices Holders SystemInfo Utils

lib_LTLIBRARIES = libstorage.la

libstorage_la_SOURCES =					\
	Environment.h		Environment.cc		\
	EnvironmentImpl.h	EnvironmentImpl.cc	\
	Storage.h		Storage.cc		\
	StorageImpl.h		StorageImpl.cc		\
	Devicegraph.h		Devicegraph.cc		\
	DevicegraphImpl.h	DevicegraphImpl.cc	\
	Action.h		Action.cc		\
	Actiongraph.h		Actiongraph.cc		\
	ArchInfo.h		ArchInfo.cc		\
	EtcFstab.h		EtcFstab.cc		\
	EtcMdadm.h		EtcMdadm.cc		\
	Geometry.h		Geometry.cc		\
	StorageInterface.h	StorageInterface.cc

libstorage_la_LDFLAGS = -version-info 7:0:2
libstorage_la_LIBADD =			\
	Devices/libdevices.la		\
	Holders/libholders.la		\
	Utils/libutils.la 		\
	SystemInfo/libsysteminfo.la	\
	-lxml2

pkgincludedir = $(includedir)/storage

pkginclude_HEADERS =		\
	Environment.h		\
	Storage.h		\
	Devicegraph.h

