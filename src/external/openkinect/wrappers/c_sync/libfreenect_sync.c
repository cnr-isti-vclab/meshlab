/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2010 Brandyn White (bwhite@dappervision.com)
 *                    Andrew Miller (amiller@dappervision.com)
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
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "libfreenect_sync.h"

typedef struct buffer_ring {
	pthread_mutex_t lock;
	pthread_cond_t cb_cond;
	void *bufs[3];
	uint32_t timestamp;
	int valid; // True if middle buffer is valid
	int fmt;
} buffer_ring_t;

typedef struct sync_kinect {
	freenect_device *dev;
	buffer_ring_t video;
	buffer_ring_t depth;
} sync_kinect_t;

typedef int (*set_buffer_t)(freenect_device *dev, void *buf);

#define MAX_KINECTS 64
static sync_kinect_t *kinects[MAX_KINECTS] = {};
static freenect_context *ctx;
static int thread_running = 0;
static pthread_t thread;
static pthread_mutex_t runloop_lock = PTHREAD_MUTEX_INITIALIZER;
static int pending_runloop_tasks = 0;
static pthread_mutex_t pending_runloop_tasks_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t pending_runloop_tasks_cond = PTHREAD_COND_INITIALIZER;

/* Locking Convention
   Rules:
       - if you need more than one lock on a line, get them from left to right
       - do not mix locks on different lines
       - if you need to change the lock rules, make sure you check everything and update this
   Lock Families:
       - pending_runloop_tasks_lock
       - runloop_lock, buffer_ring_t.lock (NOTE: You may only have one)
*/

static int alloc_buffer_ring_video(freenect_video_format fmt, buffer_ring_t *buf)
{
	int sz, i;
	switch (fmt) {
		case FREENECT_VIDEO_RGB:
			sz = FREENECT_VIDEO_RGB_SIZE;
			break;
		case FREENECT_VIDEO_BAYER:
			sz = FREENECT_VIDEO_BAYER_SIZE;
			break;
		case FREENECT_VIDEO_IR_8BIT:
			sz = FREENECT_VIDEO_IR_8BIT_SIZE;
			break;
		case FREENECT_VIDEO_IR_10BIT:
			sz = FREENECT_VIDEO_IR_10BIT;
			break;
		case FREENECT_VIDEO_IR_10BIT_PACKED:
			sz = FREENECT_VIDEO_IR_10BIT_PACKED_SIZE;
			break;
		default:
			printf("Invalid video format %d\n", fmt);
			return -1;
	}
	for (i = 0; i < 3; ++i)
		buf->bufs[i] = malloc(sz);
	buf->timestamp = 0;
	buf->valid = 0;
	buf->fmt = fmt;
	return 0;
}

static int alloc_buffer_ring_depth(freenect_depth_format fmt, buffer_ring_t *buf)
{
	int sz, i;
	switch (fmt) {
		case FREENECT_DEPTH_11BIT:
			sz = FREENECT_DEPTH_11BIT_SIZE;
			break;
		case FREENECT_DEPTH_10BIT:
			sz = FREENECT_DEPTH_10BIT_SIZE;
			break;
		case FREENECT_DEPTH_11BIT_PACKED:
			sz = FREENECT_DEPTH_11BIT_PACKED_SIZE;
			break;
		case FREENECT_DEPTH_10BIT_PACKED:
			sz = FREENECT_DEPTH_10BIT_PACKED_SIZE;
			break;
		default:
			printf("Invalid depth format %d\n", fmt);
			return -1;
	}
	for (i = 0; i < 3; ++i)
		buf->bufs[i] = malloc(sz);
	buf->timestamp = 0;
	buf->valid = 0;
	buf->fmt = fmt;
	return 0;
}

static void free_buffer_ring(buffer_ring_t *buf)
{
	int i;
	for (i = 0; i < 3; ++i) {
		free(buf->bufs[i]);
		buf->bufs[i] = NULL;
	}
	buf->timestamp = 0;
	buf->valid = 0;
	buf->fmt = -1;
}

static void producer_cb_inner(freenect_device *dev, void *data, uint32_t timestamp, buffer_ring_t *buf, set_buffer_t set_buffer)
{
	pthread_mutex_lock(&buf->lock);
	assert(data == buf->bufs[2]);
	void *temp_buf = buf->bufs[1];
	buf->bufs[1] = buf->bufs[2];
	buf->bufs[2] = temp_buf;
	set_buffer(dev, temp_buf);
	buf->timestamp = timestamp;
	buf->valid = 1;
	pthread_cond_signal(&buf->cb_cond);
	pthread_mutex_unlock(&buf->lock);
}

static void video_producer_cb(freenect_device *dev, void *data, uint32_t timestamp)
{
	producer_cb_inner(dev, data, timestamp, &((sync_kinect_t *)freenect_get_user(dev))->video, freenect_set_video_buffer);
}

static void depth_producer_cb(freenect_device *dev, void *data, uint32_t timestamp)
{
	producer_cb_inner(dev, data, timestamp, &((sync_kinect_t *)freenect_get_user(dev))->depth, freenect_set_depth_buffer);
}

/* You should only use these functions to manipulate the pending_runloop_tasks_lock*/
static void pending_runloop_tasks_inc(void)
{
	pthread_mutex_lock(&pending_runloop_tasks_lock);
	assert(pending_runloop_tasks >= 0);
	++pending_runloop_tasks;
	pthread_mutex_unlock(&pending_runloop_tasks_lock);
}

static void pending_runloop_tasks_dec(void)
{
	pthread_mutex_lock(&pending_runloop_tasks_lock);
	--pending_runloop_tasks;
	assert(pending_runloop_tasks >= 0);
	if (!pending_runloop_tasks)
		pthread_cond_signal(&pending_runloop_tasks_cond);
	pthread_mutex_unlock(&pending_runloop_tasks_lock);
}

static void pending_runloop_tasks_wait_zero(void)
{
	pthread_mutex_lock(&pending_runloop_tasks_lock);
	while (pending_runloop_tasks)
		pthread_cond_wait(&pending_runloop_tasks_cond, &pending_runloop_tasks_lock);
	pthread_mutex_unlock(&pending_runloop_tasks_lock);
}

static void *init(void *unused)
{
	pending_runloop_tasks_wait_zero();
	pthread_mutex_lock(&runloop_lock);
	while (thread_running && freenect_process_events(ctx) >= 0) {
		pthread_mutex_unlock(&runloop_lock);
		// NOTE: This lets you run tasks while process_events isn't running
		pending_runloop_tasks_wait_zero();
		pthread_mutex_lock(&runloop_lock);
	}
	// Go through each device, call stop video, close device
	int i;
	for (i = 0; i < MAX_KINECTS; ++i) {
		if (kinects[i]) {
			freenect_stop_video(kinects[i]->dev);
			freenect_stop_depth(kinects[i]->dev);
			freenect_set_user(kinects[i]->dev, NULL);
			freenect_close_device(kinects[i]->dev);
			free_buffer_ring(&kinects[i]->video);
			free_buffer_ring(&kinects[i]->depth);
			free(kinects[i]);
			kinects[i] = NULL;
		}
	}
	freenect_shutdown(ctx);
	pthread_mutex_unlock(&runloop_lock);
	return NULL;
}

static void init_thread(void)
{
	thread_running = 1;
	freenect_init(&ctx, 0);
	pthread_create(&thread, NULL, init, NULL);
}

static int change_video_format(sync_kinect_t *kinect, freenect_video_format fmt)
{
	freenect_stop_video(kinect->dev);
	free_buffer_ring(&kinect->video);
	if (alloc_buffer_ring_video(fmt, &kinect->video))
		return -1;
	freenect_set_video_format(kinect->dev, fmt);
	freenect_set_video_buffer(kinect->dev, kinect->video.bufs[2]);
	freenect_start_video(kinect->dev);
	return 0;
}

static int change_depth_format(sync_kinect_t *kinect, freenect_depth_format fmt)
{
	freenect_stop_depth(kinect->dev);
	free_buffer_ring(&kinect->depth);
	if (alloc_buffer_ring_depth(fmt, &kinect->depth))
		return -1;
	freenect_set_depth_format(kinect->dev, fmt);
	freenect_set_depth_buffer(kinect->dev, kinect->depth.bufs[2]);
	freenect_start_depth(kinect->dev);
	return 0;
}

static sync_kinect_t *alloc_kinect(int index)
{
	sync_kinect_t *kinect = malloc(sizeof(sync_kinect_t));
	if (freenect_open_device(ctx, &kinect->dev, index)) {
		free(kinect);
		return NULL;
	}
	int i;
	for (i = 0; i < 3; ++i) {
		kinect->video.bufs[i] = NULL;
		kinect->depth.bufs[i] = NULL;
	}
	kinect->video.fmt = -1;
	kinect->depth.fmt = -1;
	freenect_set_video_callback(kinect->dev, video_producer_cb);
	freenect_set_depth_callback(kinect->dev, depth_producer_cb);
	pthread_mutex_init(&kinect->video.lock, NULL);
	pthread_mutex_init(&kinect->depth.lock, NULL);
	pthread_cond_init(&kinect->video.cb_cond, NULL);
	pthread_cond_init(&kinect->depth.cb_cond, NULL);
	return kinect;
}

static int setup_kinect(int index, int fmt, int is_depth)
{
	pending_runloop_tasks_inc();
	pthread_mutex_lock(&runloop_lock);
	int thread_running_prev = thread_running;
	if (!thread_running)
		init_thread();
	if (!kinects[index]) {
		kinects[index] = alloc_kinect(index);
	}
	if (!kinects[index]) {
		printf("Error: Invalid index [%d]\n", index);
		// If we started the thread, we need to bring it back
		if (!thread_running_prev) {
			thread_running = 0;
			pthread_mutex_unlock(&runloop_lock);
			pending_runloop_tasks_dec();
			pthread_join(thread, NULL);
		} else {
			pthread_mutex_unlock(&runloop_lock);
			pending_runloop_tasks_dec();
		}
		return -1;
	}
	freenect_set_user(kinects[index]->dev, kinects[index]);
	buffer_ring_t *buf;
	if (is_depth)
		buf = &kinects[index]->depth;
	else
		buf = &kinects[index]->video;
	pthread_mutex_lock(&buf->lock);
	if (buf->fmt != fmt) {
		if (is_depth)
			change_depth_format(kinects[index], fmt);
		else
			change_video_format(kinects[index], fmt);
	}
	pthread_mutex_unlock(&buf->lock);
	pthread_mutex_unlock(&runloop_lock);
	pending_runloop_tasks_dec();
	return 0;
}

static int sync_get(void **data, uint32_t *timestamp, buffer_ring_t *buf)
{
	pthread_mutex_lock(&buf->lock);
	// If there isn't a frame ready for us
	while (!buf->valid)
		pthread_cond_wait(&buf->cb_cond, &buf->lock);
	void *temp_buf = buf->bufs[0];
	*data = buf->bufs[0] = buf->bufs[1];
	buf->bufs[1] = temp_buf;
	buf->valid = 0;
	*timestamp = buf->timestamp;
	pthread_mutex_unlock(&buf->lock);
	return 0;
}

int freenect_sync_get_video(void **video, uint32_t *timestamp, int index, freenect_video_format fmt)
{
	if (index < 0 || index >= MAX_KINECTS) {
		printf("Error: Invalid index [%d]\n", index);
		return -1;
	}
	if (!thread_running || !kinects[index] || kinects[index]->video.fmt != fmt)
		if (setup_kinect(index, fmt, 0))
			return -1;
	sync_get(video, timestamp, &kinects[index]->video);
	return 0;
}

int freenect_sync_get_depth(void **depth, uint32_t *timestamp, int index, freenect_depth_format fmt)
{
	if (index < 0 || index >= MAX_KINECTS) {
		printf("Error: Invalid index [%d]\n", index);
		return -1;
	}
	if (!thread_running || !kinects[index] || kinects[index]->depth.fmt != fmt)
		if (setup_kinect(index, fmt, 1))
			return -1;
	sync_get(depth, timestamp, &kinects[index]->depth);
	return 0;
}

void freenect_sync_stop(void)
{
	if (thread_running) {
		thread_running = 0;
		pthread_join(thread, NULL);
	}
}
