#!/bin/bash

readonly scriptdir=$(realpath $(dirname $0))
libpath=${scriptdir}/../build/lib/libsearcher-c.dylib

if [ ! -f ${libpath} ]; then
    cmake --preset release & cmake --build --preset release
fi

SEARCHER_LIB_PATH=${libpath} python3 ${scriptdir}/main.py