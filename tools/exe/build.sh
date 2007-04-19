#!/bin/sh

gcc -c crt0start.c
gcc -c joo.cc
./linkexe -o example.zsx joo.o
