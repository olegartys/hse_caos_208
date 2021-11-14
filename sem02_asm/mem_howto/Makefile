CC = gcc
CFLAGS = -m32 -O0 -g

OUT_DIR = out

%: %.S
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) ../../simpleio_i686.S $^ -o $(OUT_DIR)/$@
