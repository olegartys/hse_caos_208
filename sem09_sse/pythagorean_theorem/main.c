#include <stdio.h>

extern float pythagorean(float a, float b);

int main(void) {
	float a, b;
	scanf("%f%f", &a, &b);

	float c = pythagorean(a, b);
	printf("%f\n", c);

	return 0;
}
