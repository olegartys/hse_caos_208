// Программа с помощью системного вызова read читает строку длиной не больше BUF_SIZE из stdin и считает количество символов,
// попадающих под условие заданного предиката. Количетсво символов выводится с помощью библиотечной функции printf.
#include <sys/syscall.h>
#define BUF_SIZE 32
#define PREDICATE_FUNC_NAME isalpha

	.data
.fmt1:
	.asciz "%d\n"

	.text
// int string_lookup(const char *s, int (*f)(int c))
.string_lookup:
	push %ebp
	mov %esp, %ebp
	push %ebx
	push %esi

	mov 8(%ebp), %ebx // s
	test %ebx, %ebx
	jz .string_lookup.exit

	mov 12(%ebp), %esi // f

	xor %eax, %eax

.string_lookup.loop:
	cmpb $0, (%ebx)
	je .string_lookup.exit

	// Вызываем предикат. Важно, что предикат принимает int (32 бита)
	// в качестве аргумента, а символ в ASCII строке представляется
	// 8 битами.
	push %eax
	xor %edx, %edx
	mov (%ebx), %dl // msb [ 000000xx ] lsb
	push %edx
	call *%esi
	mov %eax, %ecx
	add $4, %esp
	pop %eax

	// В ecx ранее сохранили значение, которое венул предикат
	test %ecx, %ecx
	jz .string_lookup.failed

	inc %eax

.string_lookup.failed:
	inc %ebx
	jmp .string_lookup.loop

.string_lookup.exit:
	pop %esi
	pop %ebx
	mov %ebp, %esp
	pop %ebp
	ret

	.global main
main:
	push %ebp
	mov %esp, %ebp

	// Выделяем пространство для буфера на стеке
	sub $BUF_SIZE, %esp

	// Читаем входные данные с помощью системного вызова read.
	// Читаем на байт меньше, чтобы вставить символ окончания строки.
	mov $SYS_read, %eax
	mov $0, %ebx
	lea -BUF_SIZE(%ebp), %ecx
	mov $BUF_SIZE-1, %edx
	int $0x80

	test %eax, %eax
	js .main.exit

	// Ставим символ окончания строки
	movb $0, (%ecx, %eax,)

	// Вызываем функцию обхода строки.
	// В качестве предиката указываем библиотечную функцию isalpha
	push $PREDICATE_FUNC_NAME
	push %ecx
	call .string_lookup
	add $8, %esp

	// Вызываем printf для вывода результата.
	//
	// Использовать printf здесь в общем случае небезопасно,
	// т.к. его реализации обычно используют внутренний буфер,
	// который сбрасывается при завершении программы с помощью библиотечных
	// функций или при завершении main. Мы же завершаем программу сразу
	// системный вызов exit.
	//
	// Сброс буфера происходит из-за \n в формате строки в реализации libc,
	// используемой в дистрибутивах на виртуальных машинах этого курса.
	push %eax
	push $.fmt1
	call printf
	add $8, %esp

.main.exit:
	mov $SYS_exit, %eax
	mov $0, %ebx
	int $0x80
