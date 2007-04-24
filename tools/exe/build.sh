#!/bin/sh

# gcc -c joo.cc -I libc/include
./linkexe -o example.zsx lib/libc.a
# lib/crt0start.o lib/libc.a
