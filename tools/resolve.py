#!/usr/bin/python
import os
import sys
from subprocess import Popen, PIPE
import symmap

try:
    while True:
	addr = raw_input("\033[1maddress> ")
	if addr.strip().lower() in ["exit", "e", "quit", "q"]:
		print "Bye"
		sys.exit(0)

	try:
		addr = int(addr, 16)
	except:
		try:
			addr = int(addr, 0)
		except:
			addr = None
			print "\033[0mInvalid address..."

	if addr == None:
		continue

	i = symmap.get_sym_at(addr)
	print "\033[0m0x%08x: %s + 0x%x" % (addr, i[2], addr - i[0])
except EOFError, e:
	print "Bye"
except KeyboardInterrupt, e:
	print "Bye"
