CC = gcc
CFLAGS = -O0 -g -Wall -Werror -m32 -msse2 -mmmx -Wno-error=unused-function

OUT_DIR = out

%: %.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $(OUT_DIR)/$@
