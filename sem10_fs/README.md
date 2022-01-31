* pss_counter

Посчитать количество занятой памяти в системе по метрика PSS и RSS
for (Process p : processes) {
	pss_overall += pss(p)
	rss_overall += rss(p)
}
