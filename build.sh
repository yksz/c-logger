#!/bin/sh

BUILD_DIR="build"

# debug mode?
dirname=$BUILD_DIR
if [ $1 ] && [ $1 = "debug" ] ; then
    debug=true
    dirname="${dirname}_debug"
fi

# create a directory for build
cd `dirname "${0}"`
if [ ! -e $dirname ] ; then
    mkdir $dirname
fi
cd $dirname

# build
if [ $debug ] ; then
    cmake -DCMAKE_BUILD_TYPE=Debug \
        -Dbuild_tests=ON \
        -Dbuild_examples=ON \
        ..
else
    cmake -DCMAKE_BUILD_TYPE=Release ..
fi
