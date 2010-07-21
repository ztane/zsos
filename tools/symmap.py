#!/usr/bin/python
import os
import sys
from subprocess import Popen, PIPE

def get_kernel_bin_path():
   return os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), "kernel.bin")

def read_content():
    output = Popen(["nm", "--demangle", get_kernel_bin_path()], stdout=PIPE).communicate()[0]
    lines = output.splitlines()

    global _split_records
    _split_records = [ i.split(' ', 3) for i in lines ]
    _split_records = [ (int(i[0], 16), i[1], i[2]) for i in _split_records ]

    _split_records.sort(lambda x, y: cmp(y[0], x[0]))

read_content()
 
def get_sym_map():
    return _split_records

def get_sym_at(addr):
    for i in _split_records:
        if i[0] < addr:
	   break

    return i
