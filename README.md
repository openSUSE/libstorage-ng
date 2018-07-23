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

gcc-c++ boost-devel libxml2-devel libtool doxygen graphviz python3-devel ruby
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


Creating Changes And Package And Submitting To OBS
--------------------------------------------------

Creating the changes file and tar archive are handled by jenkins
using [linuxrc-devtools](https://github.com/openSUSE/linuxrc-devtools).

You can generate a preview of the changes file by running

```sh
make changes
```

and create the tar.xz source archive by running

```sh
make archive
```

Package versions are tracked by setting version tags in git. The last
version digit is auto-increased with every OBS commit.

The version can always be set manually by setting an appropriate tag in git.

> **Notes**
>
> 1. The `VERSION` file is auto-generated from the latest git tag.
>
> 2. The spec file template `libstorage-ng.spec.in` is **not** used.
Instead, the spec file from the OBS is used.

For a more detailed description about the handling of version numbers and changelog entries
look [here](https://github.com/openSUSE/linuxrc-devtools/blob/master/workflow.md).


Package Versions
----------------

Versioning follows [YaST](http://yastgithubio.readthedocs.io/en/latest/versioning)'s versioning scheme.

Currently this means

- 4.1.X for the `master` branch submitted to `Factory` and `SLE-15-SP1`
- 3.3.X for the `SLE-15-GA` branch submitted to `SLE-15:Update`


Code Documentation
------------------

```sh
xdg-open doc/autodocs/html/index.html
```

See especially the class hierarchy:

```sh
xdg-open doc/autodocs/html/inherits.html
```
