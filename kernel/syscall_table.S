.data
.globl SYSCALL_VECTORS
SYSCALL_VECTORS:
	.long sys_BAD // restart
	.long sys_exit
	.long sys_BAD // fork
	.long sys_read
	.long sys_write
	.long sys_open
	.long sys_close
	.long sys_BAD // waitpid
	.long sys_BAD // creat
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_lseek

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_brk
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD

	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_BAD
	.long sys_stat
	.long sys_stat // LSTAT REALLY!
	.long sys_fstat
	.long sys_BAD
SYSCALL_TABLE_SIZE=( . - SYSCALL_VECTORS )
