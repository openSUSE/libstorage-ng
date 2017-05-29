#! /bin/bash

set -x

env
storage-ng-travis-cpp && bash <(curl -s https://codecov.io/bash)
