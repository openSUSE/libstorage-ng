AC_INIT([test programs], 1.0)
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(config.h)

AC_DISABLE_STATIC

AC_PROG_CXX
LT_INIT

CXXFLAGS="${CXXFLAGS} -std=c++11 -Wall -Wextra -Wnon-virtual-dtor -Wno-unused-parameter"

AC_OUTPUT(
	Makefile
	storage/Makefile
	storage/Devices/Makefile
	storage/Holders/Makefile
	examples/Makefile
	testsuite/Makefile
)
