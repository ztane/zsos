.data
.globl SYSCALL_VECTORS
SYSCALL_VECTORS:
	.long sys_write_character
	.long sys_become_io_task

SYSCALL_TABLE_SIZE=(.-SYSCALL_VECTORS)
