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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include "freenect_internal.h"

int fnusb_init(fnusb_ctx *ctx, freenect_usb_context *usb_ctx)
{
	int res;
	if (!usb_ctx) {
		res = libusb_init(&ctx->ctx);
		if (res >= 0) {
			ctx->should_free_ctx = 1;
			return 0;
		} else {
			ctx->should_free_ctx = 0;
			ctx->ctx = NULL;
			return res;
		}
	} else {
		ctx->ctx = usb_ctx;
		ctx->should_free_ctx = 0;
		return 0;
	}
}

int fnusb_shutdown(fnusb_ctx *ctx)
{
	//int res;
	if (ctx->should_free_ctx) {
		libusb_exit(ctx->ctx);
		ctx->ctx = NULL;
	}
	return 0;
}

int fnusb_process_events(fnusb_ctx *ctx)
{
	return libusb_handle_events(ctx->ctx);
}

int fnusb_open_subdevices(freenect_device *dev, int index)
{
	freenect_context *ctx = dev->parent;

	dev->usb_cam.parent = dev;
	dev->usb_cam.dev = NULL;
	dev->usb_motor.parent = dev;
	dev->usb_motor.dev = NULL;

	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	ssize_t cnt = libusb_get_device_list (dev->parent->usb.ctx, &devs); //get the list of devices
	if (cnt < 0)
		return -1;

	int i = 0, nr_cam = 0, nr_mot = 0;
	int res;
	struct libusb_device_descriptor desc;

	for (i = 0; i < cnt; i++) {
		int r = libusb_get_device_descriptor (devs[i], &desc);
		if (r < 0)
			continue;

		if (desc.idVendor != VID_MICROSOFT)
			continue;

		// Search for the camera
		if (!dev->usb_cam.dev && desc.idProduct == PID_NUI_CAMERA) {
			// If the index given by the user matches our camera index
			if (nr_cam == index) {
				res = libusb_open (devs[i], &dev->usb_cam.dev);
				if (res < 0 || !dev->usb_cam.dev) {
					FN_ERROR("Could not open camera: %d\n", res);
					dev->usb_cam.dev = NULL;
					break;
				}
				res = libusb_claim_interface (dev->usb_cam.dev, 0);
				if (res < 0) {
					FN_ERROR("Could not claim interface on camera: %d\n", res);
					libusb_close(dev->usb_cam.dev);
					dev->usb_cam.dev = NULL;
					break;
				}
			} else {
				nr_cam++;
			}
		}

		// Search for the motor
		if (!dev->usb_motor.dev && desc.idProduct == PID_NUI_MOTOR) {
			// If the index given by the user matches our camera index
			if (nr_mot == index) {
				res = libusb_open (devs[i], &dev->usb_motor.dev);
				if (res < 0 || !dev->usb_motor.dev) {
					FN_ERROR("Could not open motor: %d\n", res);
					dev->usb_motor.dev = NULL;
					break;
				}
				res = libusb_claim_interface (dev->usb_motor.dev, 0);
				if (res < 0) {
					FN_ERROR("Could not claim interface on motor: %d\n", res);
					libusb_close(dev->usb_motor.dev);
					dev->usb_motor.dev = NULL;
					break;
				}
			} else {
				nr_mot++;
			}
		}
	}

	libusb_free_device_list (devs, 1);  // free the list, unref the devices in it

	if (dev->usb_cam.dev && dev->usb_motor.dev) {
		return 0;
	} else {
		if (dev->usb_cam.dev) {
			libusb_release_interface(dev->usb_cam.dev, 0);
			libusb_close(dev->usb_cam.dev);
		}
		if (dev->usb_motor.dev) {
			libusb_release_interface(dev->usb_motor.dev, 0);
			libusb_close(dev->usb_motor.dev);
		}
		return -1;
	}
}

int fnusb_close_subdevices(freenect_device *dev)
{
	if (dev->usb_cam.dev) {
		libusb_release_interface(dev->usb_cam.dev, 0);
		libusb_close(dev->usb_cam.dev);
		dev->usb_cam.dev = NULL;
	}
	if (dev->usb_motor.dev) {
		libusb_release_interface(dev->usb_motor.dev, 0);
		libusb_close(dev->usb_motor.dev);
		dev->usb_motor.dev = NULL;
	}
	return 0;
}

static void iso_callback(struct libusb_transfer *xfer)
{
	int i;
	fnusb_isoc_stream *strm = xfer->user_data;

	if (strm->dead) {
		freenect_context *ctx = strm->parent->parent->parent;
		strm->dead_xfers++;
		FN_SPEW("EP %02x transfer complete, %d left\n", xfer->endpoint, strm->num_xfers - strm->dead_xfers);
		return;
	}

	if(xfer->status == LIBUSB_TRANSFER_COMPLETED) {
		uint8_t *buf = (void*)xfer->buffer;
		for (i=0; i<strm->pkts; i++) {
			strm->cb(strm->parent->parent, buf, xfer->iso_packet_desc[i].actual_length);
			buf += strm->len;
		}
		libusb_submit_transfer(xfer);
	} else {
		freenect_context *ctx = strm->parent->parent->parent;
		FN_WARNING("Isochronous transfer error: %d\n", xfer->status);
		strm->dead_xfers++;
	}
}

int fnusb_start_iso(fnusb_dev *dev, fnusb_isoc_stream *strm, fnusb_iso_cb cb, int ep, int xfers, int pkts, int len)
{
	freenect_context *ctx = dev->parent->parent;
	int ret, i;

	strm->parent = dev;
	strm->cb = cb;
	strm->num_xfers = xfers;
	strm->pkts = pkts;
	strm->len = len;
	strm->buffer = malloc(xfers * pkts * len);
	strm->xfers = malloc(sizeof(struct libusb_transfer*) * xfers);
	strm->dead = 0;
	strm->dead_xfers = 0;

	uint8_t *bufp = strm->buffer;

	for (i=0; i<xfers; i++) {
		FN_SPEW("Creating EP %02x transfer #%d\n", ep, i);
		strm->xfers[i] = libusb_alloc_transfer(pkts);

		libusb_fill_iso_transfer(strm->xfers[i], dev->dev, ep, bufp, pkts * len, pkts, iso_callback, strm, 0);

		libusb_set_iso_packet_lengths(strm->xfers[i], len);

		ret = libusb_submit_transfer(strm->xfers[i]);
		if (ret < 0)
			FN_WARNING("Failed to submit isochronous transfer %d: %d\n", i, ret);

		bufp += pkts*len;
	}

	return 0;

}

int fnusb_stop_iso(fnusb_dev *dev, fnusb_isoc_stream *strm)
{
	freenect_context *ctx = dev->parent->parent;
	int i;

	strm->dead = 1;

	for (i=0; i<strm->num_xfers; i++)
		libusb_cancel_transfer(strm->xfers[i]);

	while (strm->dead_xfers < strm->num_xfers) {
		libusb_handle_events(ctx->usb.ctx);
	}

	for (i=0; i<strm->num_xfers; i++)
		libusb_free_transfer(strm->xfers[i]);

	free(strm->buffer);
	free(strm->xfers);

	memset(strm, 0, sizeof(*strm));
	return 0;
}

int fnusb_control(fnusb_dev *dev, uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint8_t *data, uint16_t wLength)
{
	return libusb_control_transfer(dev->dev, bmRequestType, bRequest, wValue, wIndex, data, wLength, 0);
}
