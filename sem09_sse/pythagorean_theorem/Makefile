CC = gcc
CFLAGS = -O0 -g -Wall -Werror -m32

OUT_DIR = out

%: pythagorean.S main.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $(OUT_DIR)/$@
