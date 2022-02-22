#include "drm_fb.h"

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>

static int init_fb(int fd, int width, int height, int bpp, int depth,
        size_t *size, off_t *offset, int *buf_id) {
    printf("Init framebuffer: width = %d, height = %d, bpp = %d, depth = %d\n",
        width, height, bpp, depth);

    struct drm_mode_create_dumb dreq;
    memset(&dreq, 0, sizeof(dreq));
    dreq.height = height;
    dreq.width = width;
    dreq.bpp = bpp;
    int ret = ioctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &dreq);
    if (ret == -1)  {
        perror("DRM_IOCTL_MODE_CREATE_DUMB");
        return -1;
    }

    struct drm_mode_fb_cmd creq;
    memset(&creq, 0, sizeof(creq));
    creq.width  = dreq.width;
    creq.height = dreq.height;
    creq.pitch  = dreq.pitch;
    creq.bpp    = dreq.bpp;
    creq.depth  = depth;
    creq.handle = dreq.handle;
    ret = ioctl(fd, DRM_IOCTL_MODE_ADDFB, &creq);
    if (ret == -1)  {
        perror("DRM_IOCTL_MODE_ADDFB");
        return -1;
    }

    struct drm_mode_map_dumb mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = dreq.handle;
    ret = ioctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (ret == -1)  {
        perror("DRM_IOCTL_MODE_MAP_DUMB");
        return -1;
    }

    *size = dreq.size;
    *offset = mreq.offset;
    *buf_id = creq.fb_id;

    return 0;
}

struct drm_fb *drm_fb_init(int fd, int width, int height, int bpp, int depth,
        int connector_type, int connector_type_id) {
    struct drm_fb *fb = NULL;
    size_t size;
    off_t offset;
    int buf_id;

    if (init_fb(fd, width, height, bpp, depth, &size, &offset, &buf_id) < 0) {
        perror("init fb");
        return NULL;
    }

    drmModeResPtr res = drmModeGetResources(fd);
    if (!res) {
        perror("drmModeGetResources");
        return NULL;
    }

    drmModeConnectorPtr connector = NULL;
    for (int i = 0; i < res->count_connectors; i++) {
        connector = drmModeGetConnectorCurrent(fd, res->connectors[i]);
        if (!connector) {
            continue;
        }

        if (connector->connector_type == connector_type &&
            connector->connector_type_id == connector_type_id) {
            break;
        }

        drmModeFreeConnector(connector);
    }

    if (!connector) {
        fprintf(stderr, "Failed searching for connector\n");
        return NULL;
    }

    drmModeModeInfoPtr mode = NULL;
    for (int i = 0; i < connector->count_modes; i++) {
        mode = &connector->modes[i];
        if (mode->type & DRM_MODE_TYPE_PREFERRED) {
            break;
        }
    }

    if (!mode) {
        fprintf(stderr, "Failed searching for preferred mode\n");
        goto error;
    }

    drmModeEncoderPtr encoder = NULL;
    encoder = drmModeGetEncoder(fd, connector->encoder_id);
    if (!encoder) {
        fprintf(stderr, "Failed searching for encoder\n");
        goto error;
    }

    fb = malloc(sizeof(*fb));
    if (!fb) {
        perror("malloc");
        goto error;
    }

    fb->connector_id = connector->connector_id;
    fb->crtc_id = encoder->crtc_id;
    fb->fd = fd;

    fb->size = size;
    fb->offset = offset;
    fb->buf_id = buf_id;

    fb->res = res;
    fb->connector = connector;
    fb->encoder = encoder;
    fb->preferred_mode = mode;

    return fb;

    // FIXME: possibly free unallocated resources
error:
    drmModeFreeEncoder(encoder);
    drmModeFreeConnector(connector);
    drmModeFreeResources(res);

    return fb;
}

int drm_draw(struct drm_fb *fb) {
    struct drm_mode_crtc crtc;
    memset(&crtc, 0, sizeof(crtc));
    crtc.x = 0;
    crtc.y = 0;
    crtc.crtc_id = fb->crtc_id;
    crtc.fb_id = fb->buf_id;
    crtc.set_connectors_ptr = (uint64_t)&fb->connector_id;
    crtc.count_connectors = 1;
    crtc.mode_valid = 1;
    memcpy(&crtc.mode, fb->preferred_mode, sizeof(crtc.mode));

    drmSetMaster(fb->fd);

    int ret = ioctl(fb->fd, DRM_IOCTL_MODE_SETCRTC, &crtc);
    if (ret == -1) {
        perror("DRM_IOCTL_MODE_SETCRTC");
        return -1;
    }

    drmDropMaster(fb->fd);

    return 0;
}

void drm_fb_release(struct drm_fb *fb) {
    // TODO: drop crtc, free dumb framebuffer

    // drmModeFreeModeInfo(fb->preferred_mode);
    // drmModeFreeEncoder(fb->encoder);
    // drmModeFreeConnector(fb->connector);
    // drmModeFreeResources(fb->res);

    free(fb);
}
