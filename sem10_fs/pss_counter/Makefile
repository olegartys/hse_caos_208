CC = gcc
CFLAGS =-O0 -g -Wall -Werror

OUT_DIR = out
INCLUDE = -I../../libcaos/include

LIBS_PATH = -L../../libcaos
LIBS = -l:libcaos.a

%: %.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OUT_DIR)/$@ $(LIBS_PATH) $(LIBS)
