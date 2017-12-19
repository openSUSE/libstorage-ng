FROM yastdevel/cpp
RUN zypper --non-interactive install swig graphviz python3-devel libxml2-devel libjson-c-devel libboost_test-devel
COPY . /usr/src/app
