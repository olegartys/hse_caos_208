# mem_howto
*Примеры работы с памятью*

# oop_asm
*Работа с массивами и структурами, indirect calls, cdecl calling convention, вызов из C кода*

Объектно-ориентированный ассемблер. “Объектом” назовём структуру
{ int x; int (*method)(int); }. Заведём две “реализации виртуального метода”: функцию, которая удваивает свой аргумент, и функцию, которая прибавляет к нему 1.
Заведём статический пустой массив “объектов” objects достаточно большой ёмкости. На стандартный вход поступают команды:
0 x i (добавить в массив объект с заданным x и указателем на i-ю реализацию метода), i = [0, 1]
1 i (установить objects[i].x = objects[i].method(objects[i].x) и распечатать это число).
