#!/bin/sh

cd `dirname "${0}"`
g++ -Wall logger_benchmark.cpp ../src/logger.c -I../src
