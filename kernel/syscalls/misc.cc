#include <syscall.hh>
#include <iostream>
#include <scheduler.hh>

#define __NEW_UTS_LEN 64

struct new_utsname {
        char sysname[__NEW_UTS_LEN + 1];
        char nodename[__NEW_UTS_LEN + 1];
        char release[__NEW_UTS_LEN + 1];
        char version[__NEW_UTS_LEN + 1];
        char machine[__NEW_UTS_LEN + 1];
        char domainname[__NEW_UTS_LEN + 1];
};

struct new_utsname uname_contents = {
     "Linux",
     "localhost",
     "2.6.38-generic",
     "#38-Ubuntu SMP Fri Dec 12 17:37:40 UTC 2014",
     "i686",
     "(none)"
};


extern Scheduler scheduler;
SYSCALL(newuname)
{
	memcpy((void*)r.arg0, &uname_contents, sizeof uname_contents);
        SYSCALL_RETURN(0);
}
