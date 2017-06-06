#
# Makefile.am for libstorage/bindings/ruby
#

if ENABLE_RUBY

SUBDIRS = . examples testsuite

EXTRA_DIST = storage-ruby.i

MAINTAINERCLEANFILES = storage_wrap.cxx

BUILT_SOURCES = storage_wrap.cxx

rubyextensiondirdir = $(RUBY_EXTENSION_DIR)

AM_CXXFLAGS = -fPIC -DPIC

AM_CPPFLAGS = -I${top_srcdir} -I$(RUBY_INCLUDE_DIR) -I$(RUBY_INCLUDE_DIR)/$(RUBY_ARCH_NAME)

include ../storage.deps

# Build Ruby module as shared library
rubyextensiondir_LTLIBRARIES = storage.la
nodist_storage_la_SOURCES = storage_wrap.cxx

storage_la_LIBADD = $(top_builddir)/storage/libstorage-ng.la -l$(RUBY_SO_NAME)
storage_la_LDFLAGS = -no-undefined -module -avoid-version -L$(RUBY_EXTENSION_DIR)

storage_la_DEPENDENCIES =				\
	$(DEPENDENCIES)					\
	${top_builddir}/storage/libstorage-ng.la

storage_wrap.cxx: storage-ruby.i $(DEPENDENCIES)
	swig -o storage_wrap.cxx -c++ -ruby -autorename ${AM_CPPFLAGS} storage-ruby.i

CLEANFILES = storage.so storage_wrap.cxx storage_wrap.h

endif
