CC = gcc
CFLAGS = -O0 -g -Wall -Werror -Wno-error=unused-function
LIBS = -lpthread

OUT_DIR = out

bq: main.c bq.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $^ -o $(OUT_DIR)/$@ $(LIBS)
