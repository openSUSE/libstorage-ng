#
# Makefile.am for libstorage/storage
#

SUBDIRS = Devices Holders Utils

lib_LTLIBRARIES = libstorage.la

libstorage_la_SOURCES =					\
	Environment.h		Environment.cc		\
	Storage.h		Storage.cc		\
	StorageImpl.h		StorageImpl.cc		\
	DeviceGraph.h		DeviceGraph.cc		\
	DeviceGraphImpl.h	DeviceGraphImpl.cc	\
	Action.h		Action.cc		\
	ActionGraph.h		ActionGraph.cc		\
	GraphUtils.h

libstorage_la_LDFLAGS = -version-info 7:0:2
libstorage_la_LIBADD = Devices/libdevices.la Holders/libholders.la Utils/libutils.la -lxml2

