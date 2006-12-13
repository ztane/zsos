#!/bin/sh

gcc -c joo.c
ld -T exe.ld joo.o
