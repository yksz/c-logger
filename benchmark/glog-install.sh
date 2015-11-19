#!/bin/sh

VERSION='0.3.4'
INSTALL_DIR='/usr/local'
DOWNLOAD_DIR='/tmp'

wget -P ${DOWNLOAD_DIR} https://github.com/google/glog/archive/v${VERSION}.tar.gz
cd ${DOWNLOAD_DIR}
tar xzvf v${VERSION}.tar.gz
cd glog-${VERSION}
./configure
make
sudo make install

# Cleanup
cd ${DOWNLOAD_DIR}
rm v${VERSION}.tar.gz
rm -r glog-${VERSION}
