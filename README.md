# Семинары
[Семинар 1 (5.11.21)](./sem01_asm): x86 ASM, целочисленная и побитовая арифметика, условные и безусловные переходы

[Семинар 2 (8.11.21)](./sem02_asm): x86 ASM, работа с памятью, indirect call, вызов ASM процедур из C кода

[Семинар 3 (15.11.21)](./sem03_asm): x86 ASM, ASCII строки, вызов libc из ASM

[Семинар 4 (22.11.21)](./sem04_asm): x86 ASM, системные вызовы

# Полезные ссылки
* [Wiki](http://wiki.cs.hse.ru/CAOS-2021)
* [Материалы прошлогоднего курса](https://github.com/blackav/hse-caos-2020)
	* [Базовые команды ассемблера](https://github.com/blackav/hse-caos-2020/tree/master/07-asm1)

---
* [x86 instruction set reference](https://c9x.me/x86/): описание основных инструкций x86
* [GNU as](https://sourceware.org/binutils/docs/as/): тут можно почитать, например, про директивы GAS
* [cdecl](https://aaronbloomfield.github.io/pdr/book/x86-32bit-ccc-chapter.pdf): описание cdecl calling convention

---
* [godbolt](https://godbolt.org/)
* [Bit Twiddling Hacks](https://graphics.stanford.edu/~seander/bithacks.html): решение различных задач с помощью битовых операций

# Tips & tricks
Настройка `gdb` для отображения регистров: `cp gdbinit ~/.gdbinit`
