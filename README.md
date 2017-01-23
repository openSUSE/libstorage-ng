libstorage-ng
=============

[![Build Status](https://travis-ci.org/openSUSE/libstorage-ng.svg?branch=master)](https://travis-ci.org/openSUSE/libstorage-ng)

libstorage-ng is the designated successor of
[libstorage](https://github.com/openSUSE/libstorage), a C++ library used by
[YaST](https://github.com/yast) to perform most storage related tasks.

The main idea of libstorage-ng compared to libstorage is to keep all
information about storage devices in a
[graph](https://en.wikipedia.org/wiki/Graph_(discrete_mathematics)) instead of
lists. For more information have a look at the [overview](doc/overview.md).


Requirements
------------

Some required tools for compiling and testing libstorage-ng are:

gcc-c++ boost-devel libxml2-devel libtool doxygen graphviz python-devel ruby
ruby-devel rubygem-test-unit swig >= 3.0.3 and != 3.0.8 (from
[YaST:storage-ng](https://build.opensuse.org/project/show/YaST:storage-ng))

In addition to the previous packages, add these distribution-specific packages as well.

For some openSUSE/SUSE distributions the naming of `rubygem-test-unit` might be the following:

ruby2.1-rubygem-test-unit

For Fedora:

rubypick


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
