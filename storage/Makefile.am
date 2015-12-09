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
	ActiongraphImpl.h	ActiongraphImpl.cc	\
	EtcFstab.h		EtcFstab.cc		\
	EtcMdadm.h		EtcMdadm.cc		\
	Geometry.h		Geometry.cc		\
	HumanString.h		HumanString.cc		\
	StorageInterface.h	StorageInterface.cc	\
	Utils.cc		Utils.h			\
	Graph.cc		Graph.h			\
	StorageVersion.h

libstorage_la_LDFLAGS = -version-info @LIBVERSION_INFO@

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
	StorageInterface.h	\
	StorageVersion.h	\
	StorageSwig.h		\
	Devicegraph.h		\
	Actiongraph.h		\
	Graph.h			\
	HumanString.h		\
	Utils.h

