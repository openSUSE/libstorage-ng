
AC_INIT(storage/StorageInterface.h)

VERSION=`cat ./VERSION`

LIBVERSION=`cat ./LIBVERSION`
LIBVERSION_MAJOR=`cut -d . -f 1 ./LIBVERSION`
LIBVERSION_MINOR=`cut -d . -f 2 ./LIBVERSION`
LIBVERSION_PATCHLEVEL=`cut -d . -f 3 ./LIBVERSION`

LIBVERSION_CURRENT=`expr $LIBVERSION_MAJOR + $LIBVERSION_MINOR`
LIBVERSION_REVISON=`expr $LIBVERSION_PATCHLEVEL`
LIBVERSION_AGE=`expr $LIBVERSION_MINOR`
LIBVERSION_INFO=$LIBVERSION_CURRENT:$LIBVERSION_REVISON:$LIBVERSION_AGE

AM_INIT_AUTOMAKE(libstorage, $VERSION)
AM_CONFIG_HEADER(config.h)

AC_DISABLE_STATIC

AC_PROG_CXX

AM_PATH_PYTHON

AC_PREFIX_DEFAULT(/usr)

LT_INIT

CWARNS="-Wall -Wextra -Wformat=2 -Wmissing-prototypes"
CFLAGS="${CFLAGS} ${CWARNS}"
CXXWARNS="-Wall -Wextra -Wformat=2 -Wnon-virtual-dtor -Wno-unused-parameter"
CXXFLAGS="${CXXFLAGS} -std=c++11 ${CXXWARNS}"

AC_SUBST(VERSION)
AC_SUBST(LIBVERSION)
AC_SUBST(LIBVERSION_MAJOR)
AC_SUBST(LIBVERSION_MINOR)
AC_SUBST(LIBVERSION_PATCHLEVEL)
AC_SUBST(LIBVERSION_INFO)

docdir=\${prefix}/share/doc/packages/libstorage
fillupdir=/var/adm/fillup-templates

AC_SUBST(docdir)
AC_SUBST(fillupdir)

PYTHON_CONFIG=${PYTHON_CONFIG:-python2-config}
PYTHON_INCLUDES=$($PYTHON_CONFIG --includes)
PYTHON_LIBS=$($PYTHON_CONFIG --libs)

AC_SUBST(PYTHON_INCLUDES)
AC_SUBST(PYTHON_LIBS)

AC_PATH_PROG([RUBY], [ruby])
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

PERL5_VERSION=$(perl -e "print substr(\$^V, 1);")
AC_SUBST(PERL5_VERSION)
PERL5_INCLUDES=$(perl -e "print '-I', join('/CORE -I', @INC);")
AC_SUBST(PERL5_INCLUDES)

AC_OUTPUT(
	Makefile
	storage/Makefile
	storage/StorageVersion.h:storage/StorageVersion.h.in
	storage/Devices/Makefile
	storage/Holders/Makefile
	storage/Utils/Makefile
	storage/SystemInfo/Makefile
	doc/Makefile
	doc/autodocs/Makefile
	data/Makefile
	po/Makefile
	examples/Makefile
	examples/SystemInfo/Makefile
	testsuite/Makefile
	testsuite/helpers/Makefile
	testsuite/Utils/Makefile
	testsuite/SystemInfo/Makefile
	testsuite/probe/Makefile
	testsuite/dependencies/Makefile
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
	package/libstorage.spec:libstorage.spec.in
)
