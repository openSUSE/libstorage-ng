#
# Makefile.am for libstorage/storage
#

SUBDIRS = CompoundAction Devices Filesystems Holders Actions SystemInfo	\
	Utils

lib_LTLIBRARIES = libstorage-ng.la

libstorage_ng_la_SOURCES =						\
	Environment.h			Environment.cc			\
	EnvironmentImpl.h		EnvironmentImpl.cc		\
	Storage.h			Storage.cc			\
	StorageImpl.h			StorageImpl.cc			\
	Devicegraph.h			Devicegraph.cc			\
	DevicegraphImpl.h		DevicegraphImpl.cc		\
	Registries.h			Registries.cc			\
	Actiongraph.h			Actiongraph.cc			\
	ActiongraphImpl.h		ActiongraphImpl.cc		\
	Pool.h				Pool.cc				\
	PoolImpl.h			PoolImpl.cc			\
	Prober.h			Prober.cc			\
	FindBy.h							\
	Redirect.h							\
	Graphviz.h			Graphviz.cc			\
	GraphvizImpl.h			GraphvizImpl.cc			\
	EtcFstab.h			EtcFstab.cc			\
	EtcCrypttab.h			EtcCrypttab.cc			\
	EtcMdadm.h			EtcMdadm.cc			\
	SimpleEtcFstab.h		SimpleEtcFstab.cc		\
	SimpleEtcCrypttab.h		SimpleEtcCrypttab.cc		\
	FreeInfo.h			FreeInfo.cc			\
	UsedFeatures.h			UsedFeatures.cc			\
	View.h								\
	Version.h			Version.cc			\
	CompoundAction.h		CompoundAction.cc		\
	CompoundActionImpl.h		CompoundActionImpl.cc		\
	CommitOptions.h

libstorage_ng_la_LDFLAGS = -version-info @LIBVERSION_INFO@

libstorage_ng_la_LIBADD =				\
	CompoundAction/libcompoundaction.la		\
	CompoundAction/Formatter/libcompformatter.la	\
	Devices/libdevices.la			        \
	Filesystems/libfilesystems.la		        \
	Holders/libholders.la			        \
	Actions/libactions.la			        \
	Utils/libutils.la			        \
	SystemInfo/libsystem-info.la		        \
	$(XML_LIBS)				        \
	$(JSON_C_LIBS)

pkgincludedir = $(includedir)/storage

pkginclude_HEADERS =		\
	Environment.h		\
	Storage.h		\
	Version.h		\
	Devicegraph.h		\
	Actiongraph.h		\
	Pool.h			\
	Graphviz.h		\
	SimpleEtcFstab.h	\
	SimpleEtcCrypttab.h	\
	FreeInfo.h		\
	UsedFeatures.h		\
	View.h			\
	CompoundAction.h	\
	CommitOptions.h

