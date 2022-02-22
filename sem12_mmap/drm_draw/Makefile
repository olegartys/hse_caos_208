CC = gcc
CFLAGS = -O0 -g -Wall -Werror

LIBDRM_FLAGS = `pkg-config --cflags --libs libdrm`

OUT_DIR = out

%: drm_fb.c drm_draw.c
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $^ $(LIBDRM_FLAGS) -o $(OUT_DIR)/$@
