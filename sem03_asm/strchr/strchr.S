// Написать функцию my_strchr(const char *s, int c), которая находит символ c в строке s
// и выводит на экран подстроку, начинающуюся с этого символа, если символ найден, или строку
// "Symbol not found" иначе.

// Строка в языке C представляется массивом символом, заверщающимся символом '\0'
// Такую строку называют null-terminated string

	.data
.string_not_found:
	.asciz "Symbol not found"

	.text
	.global my_strchr
my_strchr:
	// Пролог
	push %ebp
	mov %esp, %ebp

	mov 8(%ebp), %eax // const char*
	mov 12(%ebp), %ecx // int c

	// Проверяем, не передали ли нам nullptr вместо строки
	test %eax, %eax
	jz .my_strchr.exit

.my_strchr.loop:
	// Признак окончания строки -- '\0' == 0
	cmpb $0, (%eax)
	je .my_strchr.not_found

	// Сравниваем символ c с текущим элементом строки
	// Здесь важно использовать инструкцию cmp с суффиксом
	// b, потому что мы хотим сравнить только 1 байт данных,
	// находящихся по адресу из %eax. По умолчанию ассемблер
	// попытается вывести, сколько данных нужно считать,
	// из типов аргументов.
	cmpb %cl, (%eax)
	je .my_strchr.found

	incb %al

	jmp .my_strchr.loop

.my_strchr.not_found:
	// Вызываем puts: аргументы кладутся на стек
	push $.string_not_found
	call puts
	// После вызова возвращаем указатель на стек
	// на то место, где он был до вызова функции
	add $4, %esp
	jmp .my_strchr.exit

.my_strchr.found:
	push %eax
	call puts
	add $4, %esp

.my_strchr.exit:
	mov %ebp, %esp
	pop %ebp
	ret
