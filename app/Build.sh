#!/usr/bin/env bash

/home/nabil/Qt/static/bin/qmake -config debug
make -j12 clean
make -j12
make -j12 clean
