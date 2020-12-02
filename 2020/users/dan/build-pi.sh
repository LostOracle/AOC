#!/bin/bash

curr=$( pwd )
int_code=""
if [ ! -z "$1" ]; then int_code="../intcode/*.cpp"; fi
g++-10.1 $curr/*.cpp $int_code -O2 -std=c++20 -Wall -Wno-psabi -o $curr/test.exe
