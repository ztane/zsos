#!/bin/sh

gcc -c crt0start.c
gcc -c joo.cc
./linkexe joo.o -o example.zsx 
