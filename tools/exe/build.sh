#!/bin/sh

gcc -c test.c -I /home/ztane/xpython/build/target/Include
./linkexe -o example.zsx test.o
