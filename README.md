# Семинары
[Семинар 1 (5.11.21)](./sem01_asm): x86 ASM, целочисленная и побитовая арифметика, условные и безусловные переходы

[Семинар 2 (8.11.21)](./sem02_asm): x86 ASM, работа с памятью, indirect call, вызов ASM процедур из C кода

[Семинар 3 (15.11.21)](./sem03_asm): x86 ASM, ASCII строки, вызов libc из ASM

[Семинар 4 (22.11.21)](./sem04_asm): x86 ASM, системные вызовы

[Семинар 5 (29.11.21)](./sem05_syscalls_io):
* язык C. Использование argc/argv
* ввод/вывод с помощью read/write
* использование lseek
* немного работы с динамической памятью

[Семинар 6 (6.12.21)](./sem06_stdio_procfs):
* пример определения указателей на функции
* пример использования printf со сложным форматом вывода
* компиляция с -Wall, -Werror, -pedantic и исправление последствий
* работа с файловой системой procfs: чтение собственной памяти и изменение кода программы через /proc/self/mem

[Семинар 7 (13.12.21)](./sem07_dynamic_memory):
* подсчет метрик PSS, RSS через парсинг /proc/<pid>/smaps
* stdio для работы с файлами: FILE\*, fopen, getline
* примеры использования strstr и strtol
* парсинг строк с помощью sscanf
* пример использования realloc
* пример вызова qsort

[Семинар 8 (17.01.22)](./sem08_floating_point):
* подсчет суммы float и double чисел. Kahan's алгоритм
* подсчет загрузки CPU

[Семнинар 9 (24.01.22)](./sem09_sse):
* изменение яркости grayscaled изображения
* SSE инструкции для работы с целыми и вещественными числами 

[Семинар 10 (31.01.22)](./sem10_fs):
* обход директории с помощью opendir, readdir

[Семинар 11 (07.02.22)](./sem10_fs):
* статическая/динамическая линковка (доделка задачи из ./sem10_fs)

[Семинар 12 (14.02.22)](./sem12_fs):
* файловая система, stat, unlink

* [TO BE DONE]
* использование mmap для работы с файловой системой
* отрисовка с помощью DRM: делае mmap для записи в DRM буфер

# Полезные ссылки
## Общее
* [godbolt](https://godbolt.org/)
* [Bit Twiddling Hacks](https://graphics.stanford.edu/~seander/bithacks.html): решение различных задач с помощью битовых операций

## C
* [Стандарт C11](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1548.pdf)
* [Ужасы макросов: double evaluation](https://stackoverflow.com/questions/39439181/what-is-double-evaluation-and-why-should-it-be-avoided)
* [Куча теории про floating point](https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)
* [Q notation](https://en.wikipedia.org/wiki/Q_(number_format))

## Про курс
* [Wiki](http://wiki.cs.hse.ru/CAOS-2021)
* [Материалы прошлогоднего курса](https://github.com/blackav/hse-caos-2020)
	* [Базовые команды ассемблера](https://github.com/blackav/hse-caos-2020/tree/master/07-asm1)

## x86 ASM
* [x86 instruction set reference](https://c9x.me/x86/): описание основных инструкций x86
* [GNU as](https://sourceware.org/binutils/docs/as/): тут можно почитать, например, про директивы GAS
* [cdecl](https://aaronbloomfield.github.io/pdr/book/x86-32bit-ccc-chapter.pdf): описание cdecl calling convention

# Tips & tricks
Настройка `gdb` для отображения регистров: `cp gdbinit ~/.gdbinit`
