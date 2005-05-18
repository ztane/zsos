#include <limits.h>

int main(int argc, char *argv[])
{
        char buf[12];
        char buf2[3];

        if (uintoxa(buf, sizeof(buf), 15) == -1)
                return 1;
        if (strcmp(buf, "F") != 0) {
                printf("uintoxa: %s, should be %s\n", buf, "F");
                return 1;
        }

        if (uintoxa(buf, sizeof(buf), 1023) == -1)
                return 1;
        if (strcmp(buf, "3FF") != 0) {
                printf("uintoxa: %s, should be %s\n", buf, "3FF");
                return 1;
        }

        if (uintoxa(buf, sizeof(buf), 0) == -1)
                return 1;
        if (strcmp(buf, "0") != 0) {
                printf("uintoxa: %s, should be %s\n", buf, "0");
                return 1;
        }

        if (uintoxa(buf, sizeof(buf), UINT_MAX) == -1)
                return 1;
        if (strcmp(buf, "FFFFFFFF") != 0) {
                printf("uintoxa: %s, should be %s\n", buf, "FFFFFFFF");
                return 1;
        }

        if (uintoxa(buf2, sizeof(buf2), UINT_MAX) != -1)
                return 1;
        if (strlen(buf2) != 2) {
                printf("uintoxa: strlen failure\n");
                return 1;
        }

        return 0; 
}


