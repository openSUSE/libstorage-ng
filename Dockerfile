FROM yastdevel/cpp
RUN zypper --non-interactive install swig python-devel libxml2-devel
COPY . /usr/src/app
