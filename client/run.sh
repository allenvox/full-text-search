#!/bin/bash

readonly scriptdir=$(realpath $(dirname $0))

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  filetype=so    # linux
elif [[ "$OSTYPE" == "darwin"* ]]; then
  filetype=dylib # macOS
fi

libpath=${scriptdir}/../build/lib/libsearcher-c.${filetype}

if [ ! -f ${libpath} ]; then
    cmake --preset release & cmake --build --preset release
fi

SEARCHER_LIB_PATH=${libpath} python3 ${scriptdir}/main.py