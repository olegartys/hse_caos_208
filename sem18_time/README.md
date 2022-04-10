* time_compare
На вход программе подается дата в формате YYYY-MM-DD hh:mm:ss (2022-04-03 09:05:42). Требуется вывести 1, если дата в будущем, или 0, если дата в прошлом.

* execution_measure
В программе определна функция bar. Требуется замерить время ее исполнения.

time_t (man time)
struct tm (man mktime)

clock_t (man clock)

struct timeval (man gettimeofday)
struct timespec (man clock_gettime)
