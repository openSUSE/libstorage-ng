#! /bin/sh

# This is a CI build script for running inside the Travis builds.

# exit on error immediately, print the executed commands
set -e -x

# use ccache for faster rebuilds
export PATH="/usr/lib64/ccache:$PATH"
# set 2GB cache size
ccache --set-config=max_size=2.0G
# print the initial ccache statistics
ccache -s

# fetch the full history so the log can be generated correctly
if [ `git rev-parse --is-shallow-repository` = "true" ]; then
  git fetch --unshallow
fi

# generate the .tar.xz source tarball and the *.changes file
utils/make_package

# generate the *.spec file
make -f Makefile.repo

# run the osc source validator to check the *.spec and *.changes locally
(cd package && /usr/lib/obs/service/source_validator)

cp package/* /usr/src/packages/SOURCES/
# Build the binary packages
rpmbuild -bb package/*.spec

# test the %pre/%post scripts by installing/updating/removing the built packages
# ignore the dependencies to make the test easier, as a smoke test it's good enough
rpm -iv --force --nodeps /usr/src/packages/RPMS/**/*.rpm
rpm -Uv --force --nodeps /usr/src/packages/RPMS/**/*.rpm
# get the plain package names and remove all packages at once
rpm -ev --nodeps `rpm -q --qf '%{NAME} ' -p /usr/src/packages/RPMS/**/*.rpm`

# print the final ccache statistics
ccache -s
