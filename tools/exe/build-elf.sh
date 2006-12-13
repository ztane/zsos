#!/bin/sh

gcc -c crt0start.c
gcc -c joo.cc
./linkexe-elf joo.o -o example.elf 
