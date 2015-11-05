#!/bin/sh

cd `dirname "${0}"`
g++ -Wall logger_benchmark.cpp -I../src
