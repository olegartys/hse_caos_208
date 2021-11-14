/* Gadalka. Программа принимает на вход число N, вычисляет z = N mod 128 и предлагает пользователю угадать z за несколько попыток.
   Если пользователь вводит число, большее z, программа печатает “-1”; если меньшее — “1”; если пользователь угадал, программа печатает “0” и останавливается.
*/

#include "../simpleio_i686.S"

	.text
	.global main
main:
	call readi32

	// Bit twiddling hacks: https://graphics.stanford.edu/~seander/bithacks.html#ModulusDivisionEasy
	// X mod 2**N: X and (2**N - 1)
	//
	// xxxx
	// mod
	// 0100
	//
	// xxxx
	// &
	// 0011
	// 00xx
	//
	// После операции остаются только биты первого числа по порядку справа от 1 из операнда, являющегося степенью двойки
	and $127, %eax
	mov %eax, %ebx

.loop:
	call readi32

	cmp %ebx, %eax
	jg .greater_z
	jl .lesser_z

	xor %eax, %eax
	call writei32
	call finish

.greater_z:
	mov $-1, %eax
	call writei32
	jmp .loop

.lesser_z:
	mov $1, %eax
	call writei32
	jmp .loop
