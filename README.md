libstorage-bgl-eval
===================

Evaluating use of the [boost graph library
(BGL)](http://www.boost.org/doc/libs/1_56_0/libs/graph/doc/index.html) in
libstorage. More information is in the
[libstorage-bgl-wiki](https://github.com/openSUSE/libstorage-bgl-eval/wiki).

Compiling
---------

```sh
make -f Makefile.repo
make -j$(nproc)
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
