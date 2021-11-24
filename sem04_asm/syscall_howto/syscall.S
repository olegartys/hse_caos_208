#include <sys/syscall.h>

	.data
.msg:
	.ascii "Hello world\n"

// . означает текущий адрес в данном контексте.
.msg_len = . - .msg

	.text
	.global main
main:
	mov $SYS_write, %eax

	// ABI системных вызовов описан для разных архитектур в
	// man 2 syscall.

	// Номера файловых дескрипторов, создаваемых для процесса
	// по умолчанию.
	// STDIN_FILENO = 0, STDOUT_FILENO = 1, STDERR_FILENO = 2
	mov $1, %ebx
	mov $.msg, %ecx
	mov $.msg_len, %edx
	int $0x80

	mov $SYS_exit, %eax
	mov $10, %ebx
	int $0x80
