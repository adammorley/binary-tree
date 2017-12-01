#!/bin/bash

make clean
make all
./node-test && \
    ./tree-test && \
    echo 'ok'
