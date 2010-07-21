#!/usr/bin/python
import symmap
import os
import struct
	
PTR_FORMAT = "<I"
ALIGNMENT = 4096

records = reversed(symmap.get_sym_map())
path = symmap.get_kernel_bin_path()
path = os.path.join(os.path.dirname(path), "kernel.map")

try:
	size = os.path.getsize(path)
except:
	size = 0

def build_map_file(symmap):
    current_record_pos = 0
    record_addresses_and_pointers = []
    records                       = []
    for i in symmap:
	record_addresses_and_pointers.append(struct.pack("<II", i[0], current_record_pos))
	name_record = str(i[2]) + "\0"
	records.append(name_record)
	current_record_pos += len(name_record)
    
    addresses = ''.join(record_addresses_and_pointers)
    header = struct.pack("<II", len(record_addresses_and_pointers), len(addresses) + 8)
    return header + addresses + ''.join(records)

map_file = build_map_file(records)
size = len(map_file)
aligned_size = (size + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
padded = map_file + '\0' * (aligned_size - size)

out = open(path, "wb")
out.write(padded)
out.close()
