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
	// SIMD x86
	// 16 byte: [p0, p1, p2 .. p15]
	 //			+
	// 			[b, b, b, b..]

	asm (
		"	xor %%ebx, %%ebx \n\t"
		"	test %%eax, %%eax \n\t"
		"	jns .brightness_non_neg \n\t"
		"	neg %%al \n\t"
		"	mov $1, %%ebx \n\t"
		""
		// 1. хотим заполнить регистр xmm1 яркостью
		".brightness_non_neg:"
		"	mov %%al, %%ah \n\t"
		"	pinsrw $0, %%eax, %%xmm1 \n\t" // [b, b]
		"	pinsrw $1, %%eax, %%xmm1 \n\t" // [b, b, b, b]
		"	pshufd $0, %%xmm1, %%xmm1 \n\t" // [b .. 16times]
		"	test %%ebx, %%ebx \n\t"
		"	jnz .brightness_neg_loop \n\t"
		""
		".brightness_loop: \n\t"
		// 2. хотим заполнить регистр xmm0 пикселями
		"	movups (%%edi), %%xmm0 \n\t" // [p0, p1 .. p15]
		// 3. сложить вектора
		"	paddusb %%xmm1, %%xmm0 \n\t"
		"	movups %%xmm0, (%%edi) \n\t" // записали результат обратно в память
		"	add $16, %%edi \n\t"
		"	sub $16, %%ecx \n\t"
		"	jnz .brightness_loop \n\t"
		"	jmp .brightness_end \n\t"
		""
		".brightness_neg_loop:\n\t"
		"	movups (%%edi), %%xmm0 \n\t" // [p0, p1 .. p15]
		"	psubusb %%xmm1, %%xmm0 \n\t"
		"	movups %%xmm0, (%%edi) \n\t" // записали результат обратно в память
		"	add $16, %%edi \n\t"
		"	sub $16, %%ecx \n\t"
		"	jnz .brightness_neg_loop \n\t"
		"	jmp .brightness_end \n\t"
		""
		".brightness_end: \n\t"
		: // output
		: "D"(line), "c"(len), "a"(brightness) // input
		: "%xmm0", "%xmm1", "%ebx" // clobber
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
		// sse_brightness(buf + i * IMAGE_WIDTH, IMAGE_WIDTH, 30);
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
