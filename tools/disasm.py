#!/usr/bin/python
import tempfile
import os
import subprocess
import base64

while True:
    line = raw_input("machine code> ").strip()
    if not line:
        print "Bye"
        sys.exit(0)

    try:
        fd, name = tempfile.mkstemp()
        fil = os.fdopen(fd, "w")
        fil.write(base64.b16decode(line))
        fil.close()
    
        subprocess.call(["objdump", "-D", "--target=binary", "-m", "i386", name])

        os.unlink(name)
    except Exception, e:
        print e
