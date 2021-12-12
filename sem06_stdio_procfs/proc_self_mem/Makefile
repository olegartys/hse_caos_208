CC = gcc
CFLAGS = -O0 -g -Werror -Wall -pedantic

OUT_DIR = out

%: %.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $(OUT_DIR)/$@
