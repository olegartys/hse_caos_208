// ! В ejudge не надо делать такой include

#include "../simpleio_i686.S" // директива препроцессора, по которой он включит содержимое указанного файла в текущий

// preprocessing -> compilation -> linkage

	.text // всек, что ниже, попадет в секцию .text
	.global main // экспортируем символ main
main:
	call readi32 // читаем целое число в регистр eax
	mov %eax, %ebx

	call readi32

	add %ebx, %eax // eax += ebx
	call writei32

	call finish
