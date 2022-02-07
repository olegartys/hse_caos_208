# m4
CC = gcc
AR = ar

CFLAGS = -O0 -g -Wall -Werror

TARGET = libcaos
INCLUDE = include
SRC = \
	src/common.c \
	src/smaps_parser.c

OUT = out
_OBJ = $(SRC:.c=.o)
OBJ = $(patsubst %,$(OUT)/%,$(_OBJ))

all: $(TARGET).a $(TARGET).so

# static library: *.a
# архив, который содержит в себе объектные файлы
$(TARGET).a: $(OBJ)
	$(AR) rcs $@ $^

# dynamic library: *.so -- shared object (DLL *.dll)
#
$(TARGET).so: $(OBJ)
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $^

$(OUT)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -I$(INCLUDE) -o $@ $<
