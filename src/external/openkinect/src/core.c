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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "freenect_internal.h"

int freenect_init(freenect_context **ctx, freenect_usb_context *usb_ctx)
{
	*ctx = malloc(sizeof(freenect_context));
	if (!ctx)
		return -1;

	memset(*ctx, 0, sizeof(freenect_context));

	(*ctx)->log_level = LL_WARNING;
	return fnusb_init(&(*ctx)->usb, usb_ctx);
}

int freenect_shutdown(freenect_context *ctx)
{
	while (ctx->first) {
		FN_NOTICE("Device %p open during shutdown, closing...\n", ctx->first);
		freenect_close_device(ctx->first);
	}

	fnusb_shutdown(&ctx->usb);
	free(ctx);
	return 0;
}

int freenect_process_events(freenect_context *ctx)
{
	return fnusb_process_events(&ctx->usb);
}

int freenect_num_devices(freenect_context *ctx)
{
	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	ssize_t cnt = libusb_get_device_list (ctx->usb.ctx, &devs); //get the list of devices

	if (cnt < 0)
		return (-1);

	int nr = 0, i = 0;
	struct libusb_device_descriptor desc;
	for (i = 0; i < cnt; ++i)
	{
		int r = libusb_get_device_descriptor (devs[i], &desc);
		if (r < 0)
			continue;
		if (desc.idVendor == VID_MICROSOFT && desc.idProduct == PID_NUI_CAMERA)
			nr++;
	}

	libusb_free_device_list (devs, 1);  // free the list, unref the devices in it

	return (nr);
}

int freenect_open_device(freenect_context *ctx, freenect_device **dev, int index)
{
	int res;
	freenect_device *pdev = malloc(sizeof(freenect_device));
	if (!pdev)
		return -1;

	memset(pdev, 0, sizeof(*pdev));

	pdev->parent = ctx;

	res = fnusb_open_subdevices(pdev, index);

	if (res < 0) {
		free(pdev);
		return res;
	}

	if (!ctx->first) {
		ctx->first = pdev;
	} else {
		freenect_device *prev = ctx->first;
		while (prev->next)
			prev = prev->next;
		prev->next = pdev;
	}

	*dev = pdev;
	return 0;
}

int freenect_close_device(freenect_device *dev)
{
	freenect_context *ctx = dev->parent;
	int res;

	// stop streams, if active
	freenect_stop_depth(dev);
	freenect_stop_video(dev);

	res = fnusb_close_subdevices(dev);
	if (res < 0) {
		FN_ERROR("fnusb_close_subdevices failed: %d\n", res);
		return res;
	}

	freenect_device *last = NULL;
	freenect_device *cur = ctx->first;

	while (cur && cur != dev) {
		last = cur;
		cur = cur->next;
	}

	if (!cur) {
		FN_ERROR("device %p not found in linked list for this context!\n", dev);
		return -1;
	}

	if (last)
		last->next = cur->next;
	else
		ctx->first = cur->next;

	free(dev);
	return 0;
}

void freenect_set_user(freenect_device *dev, void *user)
{
	dev->user_data = user;
}

void *freenect_get_user(freenect_device *dev)
{
	return dev->user_data;
}

void freenect_set_log_level(freenect_context *ctx, freenect_loglevel level)
{
	ctx->log_level = level;
}

void freenect_set_log_callback(freenect_context *ctx, freenect_log_cb cb)
{
	ctx->log_cb = cb;
}

void fn_log(freenect_context *ctx, freenect_loglevel level, const char *fmt, ...)
{
	va_list ap;

	if (level > ctx->log_level)
		return;

	if (ctx->log_cb) {
		char msgbuf[1024];

		va_start(ap, fmt);
		vsnprintf(msgbuf, 1024, fmt, ap);
		msgbuf[1023] = 0;
		va_end(ap);

		ctx->log_cb(ctx, level, msgbuf);
	} else {
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}
