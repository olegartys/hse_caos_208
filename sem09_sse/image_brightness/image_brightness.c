// SIMD -- Single Instruction Multiple Data
// CPU   <-->(bus)   GPU
// SIMD-extension

// x x x x x
// x x x x x
// x x x x x

// WIDTH * HEIGHT * sizeof(Pix)

// struct Pix {
// 	char a; // 1byte
// 	char r; // 1byte
// 	char g; // 1byte
// 	char b; // 1byte
// }

// struct Pix {
// 	char gray; // 1byte
// }

// ARGB8888
// RGB888
// RGB565
// YCbCr

#include <stdio.h>
#include <stdlib.h>

#define IMAGE_PATH "images/lena_gray.raw"
#define IMAGE_PROCESSED_PATH "images/lena_gray_processed.raw"

enum {
	IMAGE_WIDTH = 2048,
	IMAGE_HEIGHT = 2048,
	IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT,
};

static void naive_brightness(unsigned char *line, int len, int brightness) {
	// clamp (std::clamp)
	// если > 255 : 255
	// если < 0 : 0
	int pix;
	if (brightness > 0) {
		for (int i = 0; i < len; i++) {
			pix = line[i];
			pix += brightness;
			if (pix > 255) {
				pix = 255;
			}
			line[i] = pix;
		}
	} else {
		for (int i = 0; i < len; i++) {
			pix = line[i];
			pix += brightness;
			if (pix < 0) {
				pix = 0;
			}
			line[i] = pix;
		}
	}
}

static void sse_brightness(unsigned char *line, int len, int brightness) {
	// inline ASM
	asm (
		"	mov $123, %%eax \n\t"
		"   add $5, "
		"	add .."
		: "D"(line), "c"(len), "a"(brightness)
		: // output
		: "%eax"
		);
}

int main(void) {
	unsigned char *buf = NULL;
	FILE *f = fopen(IMAGE_PATH, "r");
	if (!f) {
		perror("fopen");
	}

	buf = malloc(IMAGE_SIZE);
	// calloc -- выделяет инициализированный буфер
	if (!buf) {
		perror("malloc");
	}

	if (fread(buf, IMAGE_SIZE, 1, f) != 1) {
		perror("fread");
	}

	// gray pixel = [0..255]
	for (int i = 0; i < IMAGE_HEIGHT; i++) {
		naive_brightness(buf + i * IMAGE_WIDTH, IMAGE_WIDTH, 30);
	}

	FILE *f_processed = fopen(IMAGE_PROCESSED_PATH, "w");
	if (!f_processed) {
		perror("fopen 2");
	}

	if (fwrite(buf, IMAGE_SIZE, 1, f_processed) != 1) {
		perror("fwrite");
	}

	return 0;
}
