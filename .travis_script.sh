#! /bin/bash

set -x

ci_env=`bash <(curl -s https://codecov.io/env)`
docker run -it $ci_env -e TRAVIS=1 -e TRAVIS_JOB_ID="$TRAVIS_JOB_ID" libstorage-ng-image .docker.sh
