#!/usr/bin/env bash

/home/nabil/Qt/static/bin/qmake -config release
make -j12 clean
make -j12

