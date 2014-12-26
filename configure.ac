AC_INIT([test programs], 1.0)
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(config.h)

AC_DISABLE_STATIC

AC_PROG_CXX

AM_PATH_PYTHON

AC_PREFIX_DEFAULT(/usr)

PYTHON_CONFIG=${PYTHON_CONFIG:-python2-config}
PYTHON_INCLUDES=$($PYTHON_CONFIG --includes)
PYTHON_LIBS=$($PYTHON_CONFIG --libs)

LT_INIT

CXXFLAGS="${CXXFLAGS} -std=c++11 -Wall -Wextra -Wnon-virtual-dtor -Wno-unused-parameter"

AC_SUBST(PYTHON_INCLUDES)
AC_SUBST(PYTHON_LIBS)

AC_PATH_PROG(RUBY, ruby)
if test -z "$RUBY" ; then
    AC_MSG_ERROR(ruby is missing; please install ruby)
fi

AC_DEFUN([RBCONFIG], [$RUBY -rrbconfig -e 'puts RbConfig::CONFIG[["$1"]]'])

RUBY_VERSION=`$RUBY -e "puts RUBY_VERSION"`
AC_SUBST(RUBY_VERSION)
RUBY_BIN_DIR=$(RBCONFIG(bindir))
AC_SUBST(RUBY_BIN_DIR)
RUBY_INCLUDE_DIR=$(RBCONFIG(rubyhdrdir))
if test "$RUBY_INCLUDE_DIR" = "nil" ; then
    RUBY_INCLUDE_DIR=$(RBCONFIG(archdir))
fi
AC_SUBST(RUBY_INCLUDE_DIR)
RUBY_LIB_DIR=$(RBCONFIG(libdir))
AC_SUBST(RUBY_LIB_DIR)
RUBY_EXTENSION_DIR=$(RBCONFIG(vendorarchdir))
AC_SUBST(RUBY_EXTENSION_DIR)
RUBY_VENDOR_DIR=$(RBCONFIG(vendordir))
AC_SUBST(RUBY_VENDOR_DIR)
RUBY_SO_NAME=$(RBCONFIG(RUBY_SO_NAME))
AC_SUBST(RUBY_SO_NAME)
RUBY_SHARED_LIB=$(RBCONFIG(LIBRUBY))
AC_SUBST(RUBY_SHARED_LIB)
RUBY_ARCH_NAME=$(RBCONFIG(arch))
AC_SUBST(RUBY_ARCH_NAME)

AC_OUTPUT(
	Makefile
	storage/Makefile
	storage/StorageVersion.h:storage/StorageVersion.h.in
	storage/Devices/Makefile
	storage/Holders/Makefile
	storage/Utils/Makefile
	storage/SystemInfo/Makefile
	examples/Makefile
	examples/SystemInfo/Makefile
	testsuite/Makefile
	integration-tests/Makefile
	bindings/Makefile
	bindings/python/Makefile
	bindings/python/examples/Makefile
	bindings/python/testsuite/Makefile
	bindings/ruby/Makefile
	bindings/ruby/examples/Makefile
	bindings/ruby/testsuite/Makefile
	bindings/perl5/Makefile
	bindings/perl5/examples/Makefile
	bindings/perl5/testsuite/Makefile
	bindings/ruby-legacy/Makefile
	bindings/ruby-legacy/examples/Makefile
)
