#
# Makefile.am for libstorage/storage/Utils
#

noinst_LTLIBRARIES = libutils.la

libutils_la_SOURCES =					\
	Logger.h		Logger.cc		\
	LoggerImpl.h		LoggerImpl.cc		\
	AppUtil.cc		AppUtil.h		\
	CommentedConfigFile.cc  CommentedConfigFile.h	\
	ColumnConfigFile.cc	ColumnConfigFile.h	\
	Diff.cc			Diff.h			\
	Text.cc			Text.h			\
	Stopwatch.cc		Stopwatch.h		\
	LinesIterator.cc	LinesIterator.h		\
	Math.cc			Math.h			\
	Algorithm.h					\
	FileUtils.cc		FileUtils.h		\
	Exception.h		Exception.cc		\
	ExceptionImpl.h					\
	AsciiFile.cc 		AsciiFile.h		\
	Enum.h						\
	GraphUtils.h					\
	HumanString.h		HumanString.cc		\
	Lock.cc 		Lock.h			\
	OutputProcessor.cc	OutputProcessor.h	\
	Region.cc 		Region.h		\
	RegionImpl.cc 		RegionImpl.h		\
	Topology.cc		Topology.h		\
	TopologyImpl.cc		TopologyImpl.h		\
	Alignment.cc		Alignment.h		\
	AlignmentImpl.cc	AlignmentImpl.h		\
	StorageTmpl.h					\
	StorageTypes.h					\
	SystemCmd.cc		SystemCmd.h		\
	Mockup.cc		Mockup.h		\
	Remote.cc		Remote.h		\
	XmlFile.h		XmlFile.cc		\
	SnapperConfig.h		SnapperConfig.cc	\
	CDgD.h						\
	Swig.h						\
	StorageDefines.h

$(srcdir)/gen-pathlist.cc: StorageDefines.h
	grep BIN $< | sed -e "s/^[^ ]*[ ]\+//" -e "s/ .*/,/" > $@

BUILT_SOURCES = gen-pathlist.cc

pkgincludedir = $(includedir)/storage/Utils

pkginclude_HEADERS =		\
	Swig.h			\
	Logger.h		\
	Exception.h		\
	HumanString.h		\
	Region.h		\
	Topology.h		\
	Alignment.h		\
	Remote.h

