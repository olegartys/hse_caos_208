#define OBJ_SIZE 8

	// data -- секция, где хранятся данные
	.data
my_var:
	// Выделяем 4 байта памяти и помечаем адрес меткой my_var. Иначе говоря, объявляем переменную.
	// Можно использовать директивы byte, word, long (int), quad.
	// Обязательно нужно указывать значение, которым будет проиницализирована выделанная память.
	.long 456

my_initialized_array:
	// Выделяем память под 3 слова размером 4 байта.
	// Иначе говоря, объявляем массив из 3 целочисленных элементов
	// и инициализируем конкретными значениями.
	.long 1, 100, 0xbeef

my_initialized_array_size:
	// "." -- означает "текущий адрес"
	// ". - some_label" -- конструкция для подсчета количества байт между
	// текущим адресом и указанной меткой. В данном случае размер массива
	// my_initialized_array.
	.long . - my_initialized_array

my_array:
	// Выделяем 800 байт памяти, каждый байт проинициализирован значением 0xde
	.space 100 * OBJ_SIZE, 0xde

	// Память, которую мы выделяем в секции .data, будет включена
	// в итоговый исполняемый файл, поэтому при выделении больших
	// массивов может сильно вырасти финальный размер бинарника.

	// text -- секция, где хранится код
	.text
	.global main
main:
	// $ -- в AT&T синтаксисе всегда означает immediate value
	// вместо метки подставляется адрес, на который она указывает
	//
	// mov 0x10a0, %eax -- прочитать значение по адресу 0x10a0 и положить в регистр %eax
	// mov $0x10a0, %eax -- записать immediate value в регистр %eax
	//
	// аналогично:

	// прочитать значение из памяти по адресу my_initialized_array
	mov my_initialized_array, %eax
	call writei32

	// записать адрес my_initialized_array в %eax
	mov $my_initialized_array, %ebx

	mov my_initialized_array_size, %ecx
	shr $2, %ecx
	dec %ecx

.print_array_loop:
	// Прочитать значение из памяти по адресу ebx + ecx*4 в регистр eax
	mov (%ebx, %ecx, 4), %eax
	// Аналогчная конструкция:
	// mov my_initialized_array(, %ecx, 4), %eax
	//
	// Общая запись для подсчета адреса:
	// offset(base-reg, index-reg, scale) = offset + base-reg + scale*index-reg
	// результат операции называют effective address
	call writei32

	dec %ecx
	test %ecx, %ecx
	jns .print_array_loop

	call finish
