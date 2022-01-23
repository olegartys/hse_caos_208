CC = gcc
CFLAGS = -O0 -g -Wall -Werror

OUT_DIR = out

%: %.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $(OUT_DIR)/$@
