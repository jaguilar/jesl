#! /bin/bash

cd test && IDF_TARGET=linux idf.py build && ./build/host_jesl_test.elf