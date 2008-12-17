#include <stdio.h>
#include <unistd.h>
#include <Python.h>

int main(int argc, char **argv) {
    Py_NoSiteFlag = 1;
    Py_Initialize();
    PyRun_SimpleString("import re\n"
	"print 'Hello world from python!'\n");
    Py_Finalize();
    return 0;
}
