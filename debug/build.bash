#!/bin/bash

ls | grep -x build > /dev/null
[ $? -eq 0 ] || mkdir build
cd build

g++  -I../../include -c ../../src/debug_print.cpp
ar rcs libdebug_print.a debug_print.o

g++  -I../../include -c ../../src/markov_model.cpp -ldebug_print #../../src/parser.cpp

ar rcs libmarkov_model.a markov_model.o
#ar rcs libparser.a parser.o

g++ -o debug.out ../test.cpp -I../../include -L. -lmarkov_model -ldebug_print #-lparser