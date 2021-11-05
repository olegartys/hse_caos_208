CC = gcc
CFLAGS = -m32 -O0 -g

OUT_DIR = out

%: %.S $(OUT_DIR)
	$(CC) $(CFLAGS) $< -o $(OUT_DIR)/$@

$(OUT_DIR):
	@mkdir -p $@
