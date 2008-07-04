.data
.globl SYSCALL_VECTORS
SYSCALL_VECTORS:
	.long sys_write_character
	.long sys_exit
	.long sys_get_pid
	.long sys_suspend
	.long sys_write
	.long sys_set_thread_priority	
	.long sys_sem_post
	.long sys_sem_wait
	.long sys_hello_world
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
SYSCALL_TABLE_SIZE=( . - SYSCALL_VECTORS )
