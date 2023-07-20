#!/bin/bash

ls | grep -x build > /dev/null
[ $? -eq 0 ] || mkdir build
cd build

g++  -I../include -c ../src/debug_print.cpp
ar rcs libdebug_print.a debug_print.o

g++  -I../include -c ../src/markov_model.cpp -ldebug_print

ar rcs libmarkov_model.a markov_model.o

g++ -o baumwelch.out ../src/baumwelch.cpp -I../include -L. -lmarkov_model -ldebug_print
g++ -o forward.out ../src/forward.cpp -I../include -L. -lmarkov_model -ldebug_print