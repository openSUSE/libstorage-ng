libstorage-ng
=============

Evaluating use of the [boost graph library
(BGL)](http://www.boost.org/doc/libs/1_56_0/libs/graph/doc/index.html) in
libstorage. More information is in the
[libstorage-bgl-wiki](https://github.com/openSUSE/libstorage-bgl-eval/wiki).


Requirements
------------

Some required tools for compiling and testing libstorage-ng are:

gcc-c++, boost-devel, libxml2-devel, libtool, swig >= 3.0.3 and != 3.0.8
(from [YaST:storage-ng](https://build.opensuse.org/project/show/YaST:storage-ng)),
doxygen, python-devel, ruby, ruby-devel, perl, perl-Test-Exception,
perl-Test-Unit, rubygem-test-unit

Compiling
---------

```sh
make -f Makefile.repo
make -j$(nproc)
```

Running Unit Tests
------------------

```sh
make -j$(nproc) install DESTDIR=/tmp/scratch
make -j$(nproc) check LOCALEDIR=/tmp/scratch/usr/share/locale
```

Making an RPM
-------------

```sh
make -f Makefile.repo
make package
cd package
osc build --local-package --alternative-project=openSUSE:Factory
```

Code Documentation
------------------

```sh
xdg-open doc/autodocs/index.html
```

See especially the class hierarchy:

```sh
xdg-open doc/autodocs/inherits.html
```
