
AC_INIT(storage/Storage.h)

VERSION=`cat ./VERSION`

LIBVERSION=`cat ./LIBVERSION`
LIBVERSION_MAJOR=`cut -d . -f 1 ./LIBVERSION`
LIBVERSION_MINOR=`cut -d . -f 2 ./LIBVERSION`
LIBVERSION_PATCHLEVEL=`cut -d . -f 3 ./LIBVERSION`

LIBVERSION_CURRENT=`expr $LIBVERSION_MAJOR + $LIBVERSION_MINOR`
LIBVERSION_REVISON=`expr $LIBVERSION_PATCHLEVEL`
LIBVERSION_AGE=`expr $LIBVERSION_MINOR`
LIBVERSION_INFO=$LIBVERSION_CURRENT:$LIBVERSION_REVISON:$LIBVERSION_AGE

CWARNS="-Wall -Wextra -Wformat=2 -Wmissing-prototypes"
CFLAGS="${CFLAGS} ${CWARNS}"
CXXWARNS="-Wall -Wextra -Wformat=2 -Wnon-virtual-dtor -Wno-unused-parameter -Wsuggest-override"
CXXFLAGS="${CXXFLAGS} -std=c++14 ${CXXWARNS}"

AM_INIT_AUTOMAKE(libstorage-ng, $VERSION)
AM_CONFIG_HEADER(config.h)


AC_PROG_CXX

AM_PATH_PYTHON

AC_PREFIX_DEFAULT(/usr)

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
LT_INIT([disable-static pic-only])
PKG_CHECK_MODULES(XML, libxml-2.0 >= 2.4)
AC_SUBST([XML_CFLAGS])
AC_SUBST([XML_LIBS])
CXXFLAGS="${CXXFLAGS} ${XML_CFLAGS}"
CFLAGS="${CFLAGS} ${XML_CFLAGS}"

AC_CHECK_HEADER([boost/config.hpp],[],
		[AC_MSG_ERROR([boost/config.hpp not found, install e.g. boost-devel])])

AC_SUBST(VERSION)
AC_SUBST(LIBVERSION)
AC_SUBST(LIBVERSION_MAJOR)
AC_SUBST(LIBVERSION_MINOR)
AC_SUBST(LIBVERSION_PATCHLEVEL)
AC_SUBST(LIBVERSION_INFO)

docdir=\${prefix}/share/doc/packages/libstorage-ng

AC_SUBST(docdir)

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

AC_OUTPUT(
	Makefile
	storage/Makefile
	storage/Version.h:storage/Version.h.in
	storage/CompoundAction/Makefile
	storage/Devices/Makefile
	storage/Filesystems/Makefile
	storage/Holders/Makefile
	storage/Utils/Makefile
	storage/SystemInfo/Makefile
	doc/Makefile
	doc/autodocs/Makefile
	doc/autodocs/html/Makefile
	doc/autodocs/xml/Makefile
	po/Makefile
	utils/Makefile
	examples/Makefile
	examples/SystemInfo/Makefile
	testsuite/Makefile
	testsuite/helpers/Makefile
	testsuite/Utils/Makefile
	testsuite/SystemInfo/Makefile
	testsuite/probe/Makefile
	testsuite/dependencies/Makefile
	testsuite/dependencies/luks/Makefile
	testsuite/dependencies/lvm/Makefile
	testsuite/dependencies/md/Makefile
	testsuite/dependencies/partitions/Makefile
	testsuite/dependencies/btrfs/Makefile
	testsuite/dependencies/partition-tables/Makefile
	testsuite/sorting/Makefile
	testsuite/partitions/Makefile
	testsuite/Devices/Makefile
	testsuite/freeinfo/Makefile
	testsuite/performance/Makefile
	testsuite/commented-config-file/Makefile
	testsuite/CompoundAction/Makefile
	integration-tests/Makefile
	integration-tests/partition-tables/Makefile
	integration-tests/partitions/Makefile
	integration-tests/filesystems/Makefile
	integration-tests/filesystems/btrfs/Makefile
	integration-tests/luks/Makefile
	integration-tests/md/Makefile
	integration-tests/lvm/Makefile
	integration-tests/misc/Makefile
	bindings/Makefile
	bindings/python/Makefile
	bindings/python/examples/Makefile
	bindings/python/testsuite/Makefile
	bindings/ruby/Makefile
	bindings/ruby/examples/Makefile
	bindings/ruby/testsuite/Makefile
	package/libstorage-ng.spec:libstorage-ng.spec.in
)
