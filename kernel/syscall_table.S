.data
.globl SYSCALL_VECTORS
SYSCALL_VECTORS:
	.long sys_BAD // restart 0
	.long sys_exit
	.long sys_BAD // fork
	.long sys_read
	.long sys_write
	.long sys_open
	.long sys_close
	.long sys_BAD // waitpid
	.long sys_BAD // creat
	.long sys_BAD

	.long sys_BAD // 1
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_lseek

	.long sys_getpid // 20
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 30
	.long sys_BAD
	.long sys_BAD
	.long sys_access
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 40
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_brk
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 50
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 60
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 70
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 80
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 90
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 100
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_stat
	.long sys_stat // LSTAT REALLY!
	.long sys_fstat
	.long sys_BAD

	.long sys_BAD // 110
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 120
	.long sys_BAD
	.long sys_newuname
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 130
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 140
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_writev
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 150
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 160
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 170
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 180
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 190
	.long sys_BAD
	.long sys_mmap2
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_fstat64
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 200
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 210
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 220
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 230
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 240
	.long sys_BAD
	.long sys_BAD
	.long sys_set_thread_area
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD // 250
	.long sys_BAD
	.long sys_exit_group
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
SYSCALL_TABLE_SIZE=( . - SYSCALL_VECTORS )
