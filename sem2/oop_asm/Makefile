CC = gcc
CFLAGS = -m32 -O0 -g

OUT_DIR = out

%: %.S ../../simpleio_i686.S main.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $(OUT_DIR)/$@
