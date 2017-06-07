#
# Makefile.am for libstorage/storage
#

SUBDIRS = CompoundAction Devices Filesystems Holders SystemInfo Utils

lib_LTLIBRARIES = libstorage-ng.la

libstorage_ng_la_SOURCES =						\
	Environment.h			Environment.cc			\
	EnvironmentImpl.h		EnvironmentImpl.cc		\
	Storage.h			Storage.cc			\
	StorageImpl.h			StorageImpl.cc			\
	Devicegraph.h			Devicegraph.cc			\
	DevicegraphImpl.h		DevicegraphImpl.cc		\
	Action.h			Action.cc			\
	Actiongraph.h			Actiongraph.cc			\
	ActiongraphImpl.h		ActiongraphImpl.cc		\
	FindBy.h							\
	Redirect.h							\
	Graphviz.h			Graphviz.cc			\
	EtcCrypttab.h			EtcCrypttab.cc			\
	EtcFstab.h			EtcFstab.cc			\
	EtcMdadm.h			EtcMdadm.cc			\
	FreeInfo.h			FreeInfo.cc			\
	UsedFeatures.h							\
	Version.h							\
	CompoundAction.h		CompoundAction.cc		\
	CompoundActionImpl.h		CompoundActionImpl.cc

libstorage_ng_la_LDFLAGS = -version-info @LIBVERSION_INFO@

libstorage_ng_la_LIBADD =			\
	CompoundAction/libcompoundaction.la	\
	Devices/libdevices.la			\
	Filesystems/libfilesystems.la		\
	Holders/libholders.la			\
	Utils/libutils.la			\
	SystemInfo/libsysteminfo.la		\
	-lxml2

pkgincludedir = $(includedir)/storage

pkginclude_HEADERS =		\
	Environment.h		\
	Storage.h		\
	Version.h		\
	Devicegraph.h		\
	Actiongraph.h		\
	Graphviz.h		\
	FreeInfo.h		\
	UsedFeatures.h		\
	CompoundAction.h

