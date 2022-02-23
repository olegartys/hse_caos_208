#include "drm_fb.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Специфичные для моей ОС параметры DRM
#define CARD "/dev/dri/card0"

#define FB_WIDTH 1792
#define FB_HEIGHT 1120
#define FB_BPP 32 // input image bits per pixel
#define FB_DEPTH 24 // color depth to be set when adding framebuffer

#define CONNECTOR_TYPE DRM_MODE_CONNECTOR_VIRTUAL
#define CONNECTOR_TYPE_ID 1

static int min(int a, int b) {
    return a < b ? a : b;
}

int main(int argc, char *argv[]) {
    int card_fd = -1;
    int file_fd = -1;
    struct drm_fb *fb = NULL;
    void *fb_map = MAP_FAILED;
    size_t fb_map_size = 0;
    void *file_map = MAP_FAILED;
    size_t file_map_size = 0;

    printf("Pid: %d\n", getpid());

    // 1. Инициализируем подсистему DRM и получаем дескриптор фреймбуфера
    card_fd = open(CARD, O_RDWR);
    if (card_fd < 0) {
        perror("open card");
        return 1;
    }

    fb = drm_fb_init(card_fd, FB_WIDTH, FB_HEIGHT, FB_BPP, FB_DEPTH, CONNECTOR_TYPE, CONNECTOR_TYPE_ID);
    if (!fb) {
        fprintf(stderr, "Error FB init\n");
        goto exit;
    }

    // 2. Создаем маппинг фреймбуфера в адресном пространстве нашего процесса
    fb_map_size = fb->size;
    off_t offset = fb->offset;
    printf("size=%zu, offset=%zd fd=%d\n", fb_map_size, offset, card_fd);

    fb_map = mmap(NULL, fb_map_size, PROT_WRITE, MAP_SHARED, fb->fd, offset);
    if (fb_map == MAP_FAILED) {
        perror("mmap error");
        goto exit;
    }

    // 3. Открываем файл с картинкой
    const char *file_path = argv[1];
    file_fd = open(file_path, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        goto exit;
    }

    struct stat st;
    if (fstat(file_fd, &st) < 0) {
        perror("stat");
        goto exit;
    }

    // Создаем маппинг файла в адресном пространстве нашего процесса
    file_map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
    if (file_map == MAP_FAILED) {
        perror("mmap");
        goto exit;
    }

    // Теперь у нас есть 2 указателя: на память фреймбуфера, куда мы хотим вывести картинку,
    // и на содержимое файла
    //
    // Просто вызываем копирование из одного адреса в другой
    memcpy(fb_map, file_map, min(fb_map_size, st.st_size));

    // 4. Показываем картинку
    if (drm_draw(fb) < 0) {
        fprintf(stderr, "Error drawing\n");
        goto exit;
    }

    sleep(3);

exit:
    if (file_map != MAP_FAILED) {
        munmap(file_map, file_map_size);
    }

    if (file_fd != -1) {
        close(file_fd);
    }

    if (fb_map != MAP_FAILED) {
        munmap(fb_map, fb_map_size);
    }

    if (fb) {
        drm_fb_release(fb);
    }

    if (card_fd != -1) {
        close(card_fd);
    }

    return 0;
}
