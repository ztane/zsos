#include <stdio.h>
#include <unistd.h>
#include <Python.h>
#include <sys/stat.h>
#include <stddef.h>

int main(int argc, char **argv) {
    Py_NoSiteFlag = 1;
    Py_Initialize();
    PyRun_SimpleString("print 'importing init'\nimport sys\nsys.path.append('/init')\nimport init");
    Py_Finalize();
    return 0;
}
