#!/bin/sh

gcc -c crt0start.c
gcc -c joo.cc
gcc -c malloc.c
./linkexe -o example.zsx malloc.o joo.o
