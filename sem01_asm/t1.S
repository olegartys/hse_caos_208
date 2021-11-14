/*
	Программа читает c stdin знаковые 32-битное и 64-битное число и выводит их сумму.
	В случае знакового переполнения программа печатает -1.
	Если числа считать не удалось, программа печатает 0.
*/

#include "../simpleio_i686.S"

	.text
	.global main
main:
	// Читаем первое число
	call readi32
	jc .exit_wrong_input

	// sxxx xxxx (64bit)
	// +
	// 0000 sxxx (32bit)
	// Нужен sign-extension.
	//
	// Кейс: 1 + -1:
	// Без sign-extension
	// 0000 0001 (64bit)
	// 0000 1111 (32bit)
	// +
	// 00001 0000
	// С sign-extension:
	// 0000 0001 (64bit)
	// +
	// 1111 1111 (32bit)
	// 0000 0000
	cdq // sign-extend eax в edx
	mov %edx, %ecx
	mov %eax, %ebx

	// Читаем второе число
	call readi64
	jc .exit_wrong_input

	// edx  eax  (64bit)
	// +
	// ecx  ebx  (32bit)
	//
	// sxxx xxxx (64bit)
	// ssss sxxx (32bit)
	add %ebx, %eax
	// Учитываем CF -- флаг переполнения регистра при сложении младших разрядов
	adc %ecx, %edx
	jo .exit_wrong_input2

	call writei64
	call finish

.exit_wrong_input:
	mov $0, %eax
	call writei32
	call finish

.exit_wrong_input2:
	mov $-1, %eax
	call writei32
	call finish
