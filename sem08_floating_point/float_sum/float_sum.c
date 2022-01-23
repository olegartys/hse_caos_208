#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum {
	SIZE = 1 << 20, // trailing comma
};

static void fill_arr(float *arr) {
	for (int i = 0; i < SIZE; i++) {
		arr[i] = (float) rand() / RAND_MAX;
	}
}

static float naive_sum(float *arr) {
	float sum = 0; // 32bit
	int i = 0;
	for (i = 0; i < SIZE; i = i + 1) {
		sum += arr[i];
	}

	return sum;
}

static double double_sum(float *arr) {
	double sum = 0; // 64bit
	for (int i = 0; i < SIZE; i++) {
		sum += arr[i];
	}

	return sum;
}

// Kahan
static float kahan_sum(float *arr) {
	float s = arr[0];
	float c = 0;

	for (int i = 1; i < SIZE; i++) {
		float y = arr[i] - c;
		float t = s + y;
		c = (t - s) - y;
		s = t;
	}

	return s;
}

static int comparator(const void *v1, const void *v2) {
	// type-erasure; std::any
	float f1 = *(float *)v1;
	float f2 = *(float *)v2;
	return f1 > f2;
}

static float sorted_kahan_sum(float *arr) {
	qsort(arr, SIZE, sizeof(arr[0]), comparator);

	float s = arr[0];
	float c = 0;

	for (int i = 1; i < SIZE; i++) {
		float y = arr[i] - c;
		float t = s + y;
		c = (t - s) - y;
		s = t;
	}

	return s;
}

int main(void) {
	float a[SIZE];
	// ieee 754
	// fixed point: Q8.24; Q32; Q11

	srand(time(NULL)); // UNIX-timestamp: seconds since epoch
	fill_arr(a);

	printf("%f\n", naive_sum(a));
	printf("%f\n", double_sum(a));
	printf("%f\n", kahan_sum(a));
	printf("%f\n", sorted_kahan_sum(a));

	return 0;
}
