.data
.globl SYSCALL_VECTORS
SYSCALL_VECTORS:
	.long sys_BAD
	.long sys_exit
	.long sys_get_pid
	.long sys_suspend
	.long sys_write
	.long sys_set_thread_priority	
	.long sys_sem_post
	.long sys_sem_wait
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
	.long sys_lstat
	.long sys_fstat
	.long sys_BAD
SYSCALL_TABLE_SIZE=( . - SYSCALL_VECTORS )
