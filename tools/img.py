#!/usr/bin/python
import os
import struct

class File:
	def __init__(self):
		self.offset = None
		pass

	def get_size(self):
		raise NotImplementedError()

	def get_offset(self):
		if self.get_size() == 0:
			return 0xFFFFFFFF

		return self.offset	

	def set_offset(self, offset):
		self.offset = offset

class RealFile(File):
	def __init__(self, filename):
		self.filename = filename
		stat = os.stat(filename)
		self.filesize = stat.st_size

	def get_size(self):
		return self.filesize

	def get_type(self):
		return 'f'

	def get_data(self):
		f = file(self.filename)
		dat = f.read()
		f.close()
		return dat

class DirectoryEntry:
	def __init__(self, name, f):
		self.file = f
		self.name = name

	def get_data(self):
		return struct.pack("48sc3xLL4x", self.name, 
			self.file.get_type(), self.file.get_size(), self.file.get_offset())

class DirectoryFile(File):
	def __init__(self):
		self.entries = []

	def get_size(self):
		return 64 * len(self.entries)

	def add_entry(self, entry):
		self.entries += [ entry ]

	def get_type(self):
		return 'd'

	def get_data(self):
		data = ''
		for i in self.entries:
			data += i.get_data()

		return data

PAGE_SIZE = 4096

def next_padded_offset(current):
        return int((current + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE
	

root_dir = DirectoryFile()
directories = { '/' : root_dir }

allfiles = [ root_dir ]

root = "test/"
outfile = "out.img"

root_dir.add_entry(DirectoryEntry('.',  root_dir))
root_dir.add_entry(DirectoryEntry('..', root_dir))

for i in os.walk(root):
        hostroot = i[0]
	basedir, dirs, files = i
	basedir = '/' + basedir.replace(root, '')
	
	dirfile = directories[basedir]
	for i in sorted(dirs):
		newdir = DirectoryFile()
		newdirentry = DirectoryEntry(i, newdir)
		directories[os.path.join(basedir, i)] = newdir
		dirfile.add_entry(newdirentry)

		newdir.add_entry(DirectoryEntry('.',  newdir))
		newdir.add_entry(DirectoryEntry('..', dirfile))
		allfiles.append(newdir)

	for i in sorted(files):
		newfile = RealFile(os.path.join(hostroot, i))
		dirfile.add_entry(DirectoryEntry(i, newfile))
		allfiles.append(newfile)

current_offset = 0
for i in allfiles:
	i.set_offset(current_offset)
	current_offset += i.get_size()
	current_offset  = next_padded_offset(current_offset)

outf = file(outfile, "wb")

current_pos = 0
for i in allfiles:
	data = i.get_data()
	outf.write(data)

	current_pos += len(data)
	next_pos = next_padded_offset(current_pos)

	outf.write('\0' * (next_pos - current_pos))
	current_pos = next_pos

outf.close()
