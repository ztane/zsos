#ifndef __LINUXCALL_H__INCLUDED__
#define __LINUXCALL_H__INCLUDED__

#define __NO__sys_restart_syscall      0
#define __NO__sys_exit                 1
#define __NO__sys_fork                 2
#define __NO__sys_read                 3
#define __NO__sys_write                4
#define __NO__sys_open                 5
#define __NO__sys_close                6
#define __NO__sys_waitpid              7
#define __NO__sys_creat                8
#define __NO__sys_link                 9
#define __NO__sys_unlink              10
#define __NO__sys_execve              11
#define __NO__sys_chdir               12
#define __NO__sys_time                13
#define __NO__sys_mknod               14
#define __NO__sys_chmod               15
#define __NO__sys_lchown              16
#define __NO__sys_break               17
#define __NO__sys_oldstat             18
#define __NO__sys_lseek               19
#define __NO__sys_getpid              20
#define __NO__sys_mount               21
#define __NO__sys_umount              22
#define __NO__sys_setuid              23
#define __NO__sys_getuid              24
#define __NO__sys_stime               25
#define __NO__sys_ptrace              26
#define __NO__sys_alarm               27
#define __NO__sys_oldfstat            28
#define __NO__sys_pause               29
#define __NO__sys_utime               30
#define __NO__sys_stty                31
#define __NO__sys_gtty                32
#define __NO__sys_access              33
#define __NO__sys_nice                34
#define __NO__sys_ftime               35
#define __NO__sys_sync                36
#define __NO__sys_kill                37
#define __NO__sys_rename              38
#define __NO__sys_mkdir               39
#define __NO__sys_rmdir               40
#define __NO__sys_dup                 41
#define __NO__sys_pipe                42
#define __NO__sys_times               43
#define __NO__sys_prof                44
#define __NO__sys_brk                 45
#define __NO__sys_setgid              46
#define __NO__sys_getgid              47
#define __NO__sys_signal              48
#define __NO__sys_geteuid             49
#define __NO__sys_getegid             50
#define __NO__sys_acct                51
#define __NO__sys_umount2             52
#define __NO__sys_lock                53
#define __NO__sys_ioctl               54
#define __NO__sys_fcntl               55
#define __NO__sys_mpx                 56
#define __NO__sys_setpgid             57
#define __NO__sys_ulimit              58
#define __NO__sys_oldolduname         59
#define __NO__sys_umask               60
#define __NO__sys_chroot              61
#define __NO__sys_ustat               62
#define __NO__sys_dup2                63
#define __NO__sys_getppid             64
#define __NO__sys_getpgrp             65
#define __NO__sys_setsid              66
#define __NO__sys_sigaction           67
#define __NO__sys_sgetmask            68
#define __NO__sys_ssetmask            69
#define __NO__sys_setreuid            70
#define __NO__sys_setregid            71
#define __NO__sys_sigsuspend          72
#define __NO__sys_sigpending          73
#define __NO__sys_sethostname         74
#define __NO__sys_setrlimit           75
#define __NO__sys_getrlimit           76     /* Back compatible 2Gig limited rlimit */
#define __NO__sys_getrusage           77
#define __NO__sys_gettimeofday        78
#define __NO__sys_settimeofday        79
#define __NO__sys_getgroups           80
#define __NO__sys_setgroups           81
#define __NO__sys_select              82
#define __NO__sys_symlink             83
#define __NO__sys_oldlstat            84
#define __NO__sys_readlink            85
#define __NO__sys_uselib              86
#define __NO__sys_swapon              87
#define __NO__sys_reboot              88
#define __NO__sys_readdir             89
#define __NO__sys_mmap                90
#define __NO__sys_munmap              91
#define __NO__sys_truncate            92
#define __NO__sys_ftruncate           93
#define __NO__sys_fchmod              94
#define __NO__sys_fchown              95
#define __NO__sys_getpriority         96
#define __NO__sys_setpriority         97
#define __NO__sys_profil              98
#define __NO__sys_statfs              99
#define __NO__sys_fstatfs            100
#define __NO__sys_ioperm             101
#define __NO__sys_socketcall         102
#define __NO__sys_syslog             103
#define __NO__sys_setitimer          104
#define __NO__sys_getitimer          105
#define __NO__sys_stat               106
#define __NO__sys_lstat              107
#define __NO__sys_fstat              108
#define __NO__sys_olduname           109
#define __NO__sys_iopl               110
#define __NO__sys_vhangup            111
#define __NO__sys_idle               112
#define __NO__sys_vm86old            113
#define __NO__sys_wait4              114
#define __NO__sys_swapoff            115
#define __NO__sys_sysinfo            116
#define __NO__sys_ipc                117
#define __NO__sys_fsync              118
#define __NO__sys_sigreturn          119
#define __NO__sys_clone              120
#define __NO__sys_setdomainname      121
#define __NO__sys_uname              122
#define __NO__sys_modify_ldt         123
#define __NO__sys_adjtimex           124
#define __NO__sys_mprotect           125
#define __NO__sys_sigprocmask        126
#define __NO__sys_create_module      127
#define __NO__sys_init_module        128
#define __NO__sys_delete_module      129
#define __NO__sys_get_kernel_syms    130
#define __NO__sys_quotactl           131
#define __NO__sys_getpgid            132
#define __NO__sys_fchdir             133
#define __NO__sys_bdflush            134
#define __NO__sys_sysfs              135
#define __NO__sys_personality        136
#define __NO__sys_afs_syscall        137 /* Syscall for Andrew File System */
#define __NO__sys_setfsuid           138
#define __NO__sys_setfsgid           139
#define __NO__sys__llseek            140
#define __NO__sys_getdents           141
#define __NO__sys__newselect         142
#define __NO__sys_flock              143
#define __NO__sys_msync              144
#define __NO__sys_readv              145
#define __NO__sys_writev             146
#define __NO__sys_getsid             147
#define __NO__sys_fdatasync          148
#define __NO__sys__sysctl            149
#define __NO__sys_mlock              150
#define __NO__sys_munlock            151
#define __NO__sys_mlockall           152
#define __NO__sys_munlockall         153
#define __NO__sys_sched_setparam             154
#define __NO__sys_sched_getparam             155
#define __NO__sys_sched_setscheduler         156
#define __NO__sys_sched_getscheduler         157
#define __NO__sys_sched_yield                158
#define __NO__sys_sched_get_priority_max     159
#define __NO__sys_sched_get_priority_min     160
#define __NO__sys_sched_rr_get_interval      161
#define __NO__sys_nanosleep          162
#define __NO__sys_mremap             163
#define __NO__sys_setresuid          164
#define __NO__sys_getresuid          165
#define __NO__sys_vm86               166
#define __NO__sys_query_module       167
#define __NO__sys_poll               168
#define __NO__sys_nfsservctl         169
#define __NO__sys_setresgid          170
#define __NO__sys_getresgid          171
#define __NO__sys_prctl              172
#define __NO__sys_rt_sigreturn       173
#define __NO__sys_rt_sigaction       174
#define __NO__sys_rt_sigprocmask     175
#define __NO__sys_rt_sigpending      176
#define __NO__sys_rt_sigtimedwait    177
#define __NO__sys_rt_sigqueueinfo    178
#define __NO__sys_rt_sigsuspend      179
#define __NO__sys_pread64            180
#define __NO__sys_pwrite64           181
#define __NO__sys_chown              182
#define __NO__sys_getcwd             183
#define __NO__sys_capget             184
#define __NO__sys_capset             185
#define __NO__sys_sigaltstack        186
#define __NO__sys_sendfile           187
#define __NO__sys_getpmsg            188     /* some people actually want streams */
#define __NO__sys_putpmsg            189     /* some people actually want streams */
#define __NO__sys_vfork              190
#define __NO__sys_ugetrlimit         191     /* SuS compliant getrlimit */
#define __NO__sys_mmap2              192
#define __NO__sys_truncate64         193
#define __NO__sys_ftruncate64        194
#define __NO__sys_stat64             195
#define __NO__sys_lstat64            196
#define __NO__sys_fstat64            197
#define __NO__sys_lchown32           198
#define __NO__sys_getuid32           199
#define __NO__sys_getgid32           200
#define __NO__sys_geteuid32          201
#define __NO__sys_getegid32          202
#define __NO__sys_setreuid32         203
#define __NO__sys_setregid32         204
#define __NO__sys_getgroups32        205
#define __NO__sys_setgroups32        206
#define __NO__sys_fchown32           207
#define __NO__sys_setresuid32        208
#define __NO__sys_getresuid32        209
#define __NO__sys_setresgid32        210
#define __NO__sys_getresgid32        211
#define __NO__sys_chown32            212
#define __NO__sys_setuid32           213
#define __NO__sys_setgid32           214
#define __NO__sys_setfsuid32         215
#define __NO__sys_setfsgid32         216
#define __NO__sys_pivot_root         217
#define __NO__sys_mincore            218
#define __NO__sys_madvise            219
#define __NO__sys_madvise1           219     /* delete when C lib stub is removed */
#define __NO__sys_getdents64         220
#define __NO__sys_fcntl64            221
#define __NO__sys_gettid             224
#define __NO__sys_readahead          225
#define __NO__sys_setxattr           226
#define __NO__sys_lsetxattr          227
#define __NO__sys_fsetxattr          228
#define __NO__sys_getxattr           229
#define __NO__sys_lgetxattr          230
#define __NO__sys_fgetxattr          231
#define __NO__sys_listxattr          232
#define __NO__sys_llistxattr         233
#define __NO__sys_flistxattr         234
#define __NO__sys_removexattr        235
#define __NO__sys_lremovexattr       236
#define __NO__sys_fremovexattr       237
#define __NO__sys_tkill              238
#define __NO__sys_sendfile64         239
#define __NO__sys_futex              240
#define __NO__sys_sched_setaffinity  241
#define __NO__sys_sched_getaffinity  242
#define __NO__sys_set_thread_area    243
#define __NO__sys_get_thread_area    244
#define __NO__sys_io_setup           245
#define __NO__sys_io_destroy         246
#define __NO__sys_io_getevents       247
#define __NO__sys_io_submit          248
#define __NO__sys_io_cancel          249
#define __NO__sys_fadvise64          250
#define __NO__sys_exit_group         252
#define __NO__sys_lookup_dcookie     253
#define __NO__sys_epoll_create       254
#define __NO__sys_epoll_ctl          255
#define __NO__sys_epoll_wait         256
#define __NO__sys_remap_file_pages   257
#define __NO__sys_set_tid_address    258
#define __NO__sys_timer_create       259
#define __NO__sys_timer_settime      (__NO__sys_timer_create+1)
#define __NO__sys_timer_gettime      (__NO__sys_timer_create+2)
#define __NO__sys_timer_getoverrun   (__NO__sys_timer_create+3)
#define __NO__sys_timer_delete       (__NO__sys_timer_create+4)
#define __NO__sys_clock_settime      (__NO__sys_timer_create+5)
#define __NO__sys_clock_gettime      (__NO__sys_timer_create+6)
#define __NO__sys_clock_getres       (__NO__sys_timer_create+7)
#define __NO__sys_clock_nanosleep    (__NO__sys_timer_create+8)
#define __NO__sys_statfs64           268
#define __NO__sys_fstatfs64          269
#define __NO__sys_tgkill             270
#define __NO__sys_utimes             271
#define __NO__sys_fadvise64_64       272
#define __NO__sys_vserver            273
#define __NO__sys_mbind              274
#define __NO__sys_get_mempolicy      275
#define __NO__sys_set_mempolicy      276
#define __NO__sys_mq_open            277
#define __NO__sys_mq_unlink          (__NO__sys_mq_open+1)
#define __NO__sys_mq_timedsend       (__NO__sys_mq_open+2)
#define __NO__sys_mq_timedreceive    (__NO__sys_mq_open+3)
#define __NO__sys_mq_notify          (__NO__sys_mq_open+4)
#define __NO__sys_mq_getsetattr      (__NO__sys_mq_open+5)
#define __NO__sys_kexec_load         283
#define __NO__sys_waitid             284
/* #define __NO__sys_sys_setaltroot  285 */
#define __NO__sys_add_key            286
#define __NO__sys_request_key        287
#define __NO__sys_keyctl             288
#define __NO__sys_ioprio_set         289
#define __NO__sys_ioprio_get         290
#define __NO__sys_inotify_init       291
#define __NO__sys_inotify_add_watch  292
#define __NO__sys_inotify_rm_watch   293
#define __NO__sys_migrate_pages      294
#define __NO__sys_openat             295
#define __NO__sys_mkdirat            296
#define __NO__sys_mknodat            297
#define __NO__sys_fchownat           298
#define __NO__sys_futimesat          299
#define __NO__sys_fstatat64          300
#define __NO__sys_unlinkat           301
#define __NO__sys_renameat           302
#define __NO__sys_linkat             303
#define __NO__sys_symlinkat          304
#define __NO__sys_readlinkat         305
#define __NO__sys_fchmodat           306
#define __NO__sys_faccessat          307
#define __NO__sys_pselect6           308
#define __NO__sys_ppoll              309
#define __NO__sys_unshare            310
#define __NO__sys_set_robust_list    311
#define __NO__sys_get_robust_list    312
#define __NO__sys_splice             313
#define __NO__sys_sync_file_range    314
#define __NO__sys_tee                315
#define __NO__sys_vmsplice           316
#define __NO__sys_move_pages         317

#define __syscall_return(type, res) \
do { \
        if ((unsigned long)(res) >= (unsigned long)(-(128 + 1))) { \
                errno = -(res); \
                res = -1; \
        } \
        return (type) (res); \
} while (0)

/* XXX - _foo needs to be __foo, while __NObar could be _NR_bar. */
#define _syscall0(type,name) \
type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
        : "=a" (__res) \
        : "0" (__NO##name)); \
__syscall_return(type,__res); \
}

// ""

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
long __res; \
__asm__ volatile ("push %%ebx ; movl %2,%%ebx ; int $0x80 ; pop %%ebx" \
        : "=a" (__res) \
        : "0" (__NO##name),"ri" ((long)(arg1)) : "memory"); \
__syscall_return(type,__res); \
}

// ""

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) \
{ \
long __res; \
__asm__ volatile ("push %%ebx ; movl %2,%%ebx ; int $0x80 ; pop %%ebx" \
        : "=a" (__res) \
        : "0" (__NO##name),"ri" ((long)(arg1)),"c" ((long)(arg2)) \
        : "memory"); \
__syscall_return(type,__res); \
}

// ""

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
long __res; \
__asm__ volatile ("push %%ebx ; movl %2,%%ebx ; int $0x80 ; pop %%ebx" \
        : "=a" (__res) \
        : "0" (__NO##name),"ri" ((long)(arg1)),"c" ((long)(arg2)), \
                  "d" ((long)(arg3)) : "memory"); \
__syscall_return(type,__res); \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) \
{ \
long __res; \
__asm__ volatile ("push %%ebx ; movl %2,%%ebx ; int $0x80 ; pop %%ebx" \
        : "=a" (__res) \
        : "0" (__NO##name),"ri" ((long)(arg1)),"c" ((long)(arg2)), \
          "d" ((long)(arg3)),"S" ((long)(arg4)) : "memory"); \
__syscall_return(type,__res); \
}

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) \
{ \
long __res; \
__asm__ volatile ("push %%ebx ; movl %2,%%ebx ; movl %1,%%eax ; " \
                  "int $0x80 ; pop %%ebx" \
        : "=a" (__res) \
        : "i" (__NO##name),"ri" ((long)(arg1)),"c" ((long)(arg2)), \
          "d" ((long)(arg3)),"S" ((long)(arg4)),"D" ((long)(arg5)) \
        : "memory"); \
__syscall_return(type,__res); \
}

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5,type6,arg6) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5,type6 arg6) \
{ \
long __res; \
  struct { long __a1; long __a6; } __s = { (long)arg1, (long)arg6 }; \
__asm__ volatile ("push %%ebp ; push %%ebx ; movl 4(%2),%%ebp ; " \
                  "movl 0(%2),%%ebx ; movl %1,%%eax ; int $0x80 ; " \
                  "pop %%ebx ;  pop %%ebp" \
        : "=a" (__res) \
        : "i" (__NO##name),"0" ((long)(&__s)),"c" ((long)(arg2)), \
          "d" ((long)(arg3)),"S" ((long)(arg4)),"D" ((long)(arg5)) \
        : "memory"); \
__syscall_return(type,__res); \
}

#endif
