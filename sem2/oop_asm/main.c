#include <assert.h>
#include <stdio.h>

// FIXME(olegartys): следует пометить атрибутом "packed",
// чтобы гарантировать отсутсвие padding между элементами
struct obj {
	int x;
	int (*method)(int);
};

#define OBJ_ARRAY_CAPACITY 100

struct obj obj_arr[OBJ_ARRAY_CAPACITY];
int obj_arr_size;

// cdecl
extern void obj_arr_append(int x, int i);
extern void obj_arr_call(int i);

int main(void) {
	assert(sizeof(struct obj) == 8);

	// TODO (olegartys): добавить проверку входных данных
	char c;
	while ((c = getchar()) != EOF) {
		int x, i;
		if (c == '0') {
			scanf("%d %d", &x, &i);
			obj_arr_append(x, i);
		} else if (c == '1') {
			scanf("%d", &i);
			obj_arr_call(i);
		} else if (c == '\n') {
			// no-op
		} else {
			puts("Wrong input");
			return -1;
		}
	}

	return 0;
}
