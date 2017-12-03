#!/bin/bash

make clean
make all
./test && \
    echo 'ok'
