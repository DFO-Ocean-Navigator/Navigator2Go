#!/usr/bin/env bash

if [ -z "$1" ]; then
    echo "Enter path to static Qt bin directory"
    exit 1
fi

if [ ! -d "$1" ]; then
    echo "Directory doesn't exist!"
    echo $1
    exit 1
fi

$1/qmake -config release
make -j12 clean
make -j12
make -j12 clean
