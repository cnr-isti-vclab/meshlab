/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

#ifndef LIBFREENECT_H
#define LIBFREENECT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FREENECT_FRAME_W 640
#define FREENECT_FRAME_H 480
#define FREENECT_FRAME_PIX (FREENECT_FRAME_H*FREENECT_FRAME_W)

#define FREENECT_IR_FRAME_W 640
#define FREENECT_IR_FRAME_H 488
#define FREENECT_IR_FRAME_PIX (FREENECT_IR_FRAME_H*FREENECT_IR_FRAME_W)

#define FREENECT_VIDEO_RGB_SIZE (FREENECT_FRAME_PIX*3)
#define FREENECT_VIDEO_BAYER_SIZE (FREENECT_FRAME_PIX)
#define FREENECT_VIDEO_YUV_RGB_SIZE (FREENECT_VIDEO_RGB_SIZE)
#define FREENECT_VIDEO_YUV_RAW_SIZE (FREENECT_FRAME_PIX*2)
#define FREENECT_VIDEO_IR_8BIT_SIZE (FREENECT_IR_FRAME_PIX)
#define FREENECT_VIDEO_IR_10BIT_SIZE (FREENECT_IR_FRAME_PIX*sizeof(uint16_t))
#define FREENECT_VIDEO_IR_10BIT_PACKED_SIZE 390400

#define FREENECT_DEPTH_11BIT_SIZE (FREENECT_FRAME_PIX*sizeof(uint16_t))
#define FREENECT_DEPTH_10BIT_SIZE FREENECT_DEPTH_11BIT_SIZE
#define FREENECT_DEPTH_11BIT_PACKED_SIZE 422400
#define FREENECT_DEPTH_10BIT_PACKED_SIZE 384000

#define FREENECT_COUNTS_PER_G 819

typedef enum {
	FREENECT_VIDEO_RGB = 0,
	FREENECT_VIDEO_BAYER = 1,
	FREENECT_VIDEO_IR_8BIT = 2,
	FREENECT_VIDEO_IR_10BIT = 3,
	FREENECT_VIDEO_IR_10BIT_PACKED = 4,
	FREENECT_VIDEO_YUV_RGB = 5,
	FREENECT_VIDEO_YUV_RAW = 6,
} freenect_video_format;

typedef enum {
	LED_OFF    = 0,
	LED_GREEN  = 1,
	LED_RED    = 2,
	LED_YELLOW = 3,
	LED_BLINK_YELLOW = 4,
	LED_BLINK_GREEN = 5,
	LED_BLINK_RED_YELLOW = 6
} freenect_led_options;

typedef enum {
	FREENECT_DEPTH_11BIT = 0,
	FREENECT_DEPTH_10BIT = 1,
	FREENECT_DEPTH_11BIT_PACKED = 2,
	FREENECT_DEPTH_10BIT_PACKED = 3,
} freenect_depth_format;

typedef enum {
	TILT_STATUS_STOPPED = 0x00,
	TILT_STATUS_LIMIT = 0x01,
	TILT_STATUS_MOVING = 0x04
} freenect_tilt_status_code;

typedef struct {
	int16_t accelerometer_x;
	int16_t accelerometer_y;
	int16_t accelerometer_z;
	int8_t tilt_angle;
	freenect_tilt_status_code tilt_status;
} freenect_raw_tilt_state;

struct _freenect_context;
typedef struct _freenect_context freenect_context;

struct _freenect_device;
typedef struct _freenect_device freenect_device;

// usb backend specific section
#include <libusb-1.0/libusb.h>
typedef libusb_context freenect_usb_context;
//

typedef enum {
	FREENECT_LOG_FATAL = 0,
	FREENECT_LOG_ERROR,
	FREENECT_LOG_WARNING,
	FREENECT_LOG_NOTICE,
	FREENECT_LOG_INFO,
	FREENECT_LOG_DEBUG,
	FREENECT_LOG_SPEW,
	FREENECT_LOG_FLOOD,
} freenect_loglevel;

int freenect_init(freenect_context **ctx, freenect_usb_context *usb_ctx);
int freenect_shutdown(freenect_context *ctx);

typedef void (*freenect_log_cb)(freenect_context *dev, freenect_loglevel level, const char *msg);

void freenect_set_log_level(freenect_context *ctx, freenect_loglevel level);
void freenect_set_log_callback(freenect_context *ctx, freenect_log_cb cb);

int freenect_process_events(freenect_context *ctx);

int freenect_num_devices(freenect_context *ctx);
int freenect_open_device(freenect_context *ctx, freenect_device **dev, int index);
int freenect_close_device(freenect_device *dev);

void freenect_set_user(freenect_device *dev, void *user);
void *freenect_get_user(freenect_device *dev);

typedef void (*freenect_depth_cb)(freenect_device *dev, void *depth, uint32_t timestamp);
typedef void (*freenect_video_cb)(freenect_device *dev, void *video, uint32_t timestamp);

void freenect_set_depth_callback(freenect_device *dev, freenect_depth_cb cb);
void freenect_set_video_callback(freenect_device *dev, freenect_video_cb cb);

int freenect_set_depth_format(freenect_device *dev, freenect_depth_format fmt);
int freenect_set_video_format(freenect_device *dev, freenect_video_format fmt);

int freenect_set_depth_buffer(freenect_device *dev, void *buf);
int freenect_set_video_buffer(freenect_device *dev, void *buf);

int freenect_start_depth(freenect_device *dev);
int freenect_start_video(freenect_device *dev);
int freenect_stop_depth(freenect_device *dev);
int freenect_stop_video(freenect_device *dev);

int freenect_update_tilt_state(freenect_device *dev);
freenect_raw_tilt_state* freenect_get_tilt_state(freenect_device *dev);
double freenect_get_tilt_degs(freenect_raw_tilt_state *state);
int freenect_set_tilt_degs(freenect_device *dev, double angle);
int freenect_set_led(freenect_device *dev, freenect_led_options option);
void freenect_get_mks_accel(freenect_raw_tilt_state *state, double* x, double* y, double* z);

#ifdef __cplusplus
}
#endif

#endif //

