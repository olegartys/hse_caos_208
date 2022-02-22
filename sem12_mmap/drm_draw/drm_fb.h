#pragma once

#include <stdint.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

struct drm_fb {
	int fd;

	int buf_id;
    int crtc_id;
    int connector_id;

    size_t size;
    off_t offset;

	drmModeResPtr res;
    drmModeConnectorPtr connector;
    drmModeEncoderPtr encoder;
    drmModeModeInfoPtr preferred_mode;
};

struct drm_fb *drm_fb_init(int fd, int width, int height, int bpp, int depth,
	int connector_type, int connector_type_id);
int drm_draw(struct drm_fb *);
void drm_fb_release(struct drm_fb *);
