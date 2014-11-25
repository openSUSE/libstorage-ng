#
# Makefile.am for libstorage/storage
#

SUBDIRS = Devices Holders

lib_LTLIBRARIES = libstorage.la

libstorage_la_SOURCES =					\
	DeviceGraph.h		DeviceGraph.cc		\
	DeviceGraphImpl.h	DeviceGraphImpl.cc	\
	Action.h		Action.cc		\
	ActionGraph.h		ActionGraph.cc		\
	GraphUtils.h

libstorage_la_LDFLAGS = -version-info 5:0:2
libstorage_la_LIBADD = Holders/libholders.la Devices/libdevices.la

