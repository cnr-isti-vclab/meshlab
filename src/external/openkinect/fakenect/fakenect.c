/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2010 Brandyn White (bwhite@dappervision.com)
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

#include <libfreenect.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define GRAVITY 9.80665

// The dev and ctx are just faked with these numbers

static freenect_device *fake_dev = (freenect_device *)1234;
static freenect_context *fake_ctx = (freenect_context *)5678;
static freenect_depth_cb cur_depth_cb = NULL;
static freenect_video_cb cur_rgb_cb = NULL;
static char *input_path = NULL;
static FILE *index_fp = NULL;
static freenect_raw_tilt_state state = {};
static int already_warned = 0;
static double playback_prev_time = 0.;
static double record_prev_time = 0.;
static void *depth_buffer = NULL;
static void *rgb_buffer = NULL;
static int depth_running = 0;
static int rgb_running = 0;
static void *user_ptr = NULL;

static void sleep_highres(double tm)
{
	int sec = floor(tm);
	int usec = (tm - sec) * 1000000;
	if (tm > 0) {
		sleep(sec);
		usleep(usec);
	}
}

static double get_time()
{
	struct timeval cur;
	gettimeofday(&cur, NULL);
	return cur.tv_sec + cur.tv_usec / 1000000.;
}

static char *one_line(FILE *fp)
{
	int pos = 0;
	char *out = NULL;
	char c;
	while ((c = fgetc(fp))) {
		if (c == '\n' || c == EOF)
			break;
		out = realloc(out, pos + 1);
		out[pos++] = c;
	}
	if (out) {
		out = realloc(out, pos + 1);
		out[pos] = '\0';
	}
	return out;
}

static int get_data_size(FILE *fp)
{
	int orig = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	int out = ftell(fp);
	fseek(fp, orig, SEEK_SET);
	return out;
}

static int parse_line(char *type, double *cur_time, unsigned int *timestamp, unsigned int *data_size, char **data)
{
	char *line = one_line(index_fp);
	if (!line) {
		printf("Warning: No more lines in [%s]\n", input_path);
		return -1;
	}
	int file_path_size = strlen(input_path) + strlen(line) + 50;
	char *file_path = malloc(file_path_size);
	snprintf(file_path, file_path_size, "%s/%s", input_path, line);
	// Open file
	FILE *cur_fp = fopen(file_path, "r");
	if (!cur_fp) {
		printf("Error: Cannot open file [%s]\n", file_path);
		exit(1);
	}
	// Parse data from file name
	*data_size = get_data_size(cur_fp);
	sscanf(line, "%c-%lf-%u-%*s", type, cur_time, timestamp);
	*data = malloc(*data_size);
	if (fread(*data, *data_size, 1, cur_fp) != 1) {
		printf("Error: Couldn't read entire file.\n");
		return -1;
	}
	fclose(cur_fp);
	free(line);
	free(file_path);
	return 0;
}

static void open_index()
{
	input_path = getenv("FAKENECT_PATH");
	if (!input_path) {
		printf("Error: Environmental variable FAKENECT_PATH is not set.  Set it to a path that was created using the 'record' utility.\n");
		exit(1);
	}
	int index_path_size = strlen(input_path) + 50;
	char *index_path = malloc(index_path_size);
	snprintf(index_path, index_path_size, "%s/INDEX.txt", input_path);
	index_fp = fopen(index_path, "r");
	if (!index_fp) {
		printf("Error: Cannot open file [%s]\n", index_path);
		exit(1);
	}
	free(index_path);
}

static char *skip_line(char *str)
{
	char *out = strchr(str, '\n');
	if (!out) {
		printf("Error: PGM/PPM has incorrect formatting, expected a header on one line followed by a newline\n");
		exit(1);
	}
	return out + 1;
}

int freenect_process_events(freenect_context *ctx)
{
	/* This is where the magic happens. We read 1 update from the index
	   per call, so this needs to be called in a loop like usual.  If the
	   index line is a Depth/RGB image the provided callback is called.  If
	   the index line is accelerometer data, then it is used to update our
	   internal state.  If you query for the accelerometer data you get the
	   last sensor reading that we have.  The time delays are compensated as
	   best as we can to match those from the original data and current run
	   conditions (e.g., if it takes longer to run this code then we wait less).
	 */
	if (!index_fp)
		open_index();
	char type;
	double record_cur_time;
	unsigned int timestamp, data_size;
	char *data = NULL;
	if (parse_line(&type, &record_cur_time, &timestamp, &data_size, &data))
		return -1;
	// Sleep an amount that compensates for the original and current delays
	// playback_ is w.r.t. the current time
	// record_ is w.r.t. the original time period during the recording
	if (record_prev_time != 0. && playback_prev_time != 0.)
		sleep_highres((record_cur_time - record_prev_time) - (get_time() - playback_prev_time));
	record_prev_time = record_cur_time;
	switch (type) {
		case 'd':
			if (cur_depth_cb && depth_running) {
				void *cur_depth = skip_line(data);
				if (depth_buffer) {
					memcpy(depth_buffer, cur_depth, FREENECT_DEPTH_11BIT_SIZE);
					cur_depth = depth_buffer;
				}
				cur_depth_cb(fake_dev, cur_depth, timestamp);
			}
			break;
		case 'r':
			if (cur_rgb_cb && rgb_running) {
				void *cur_rgb = skip_line(data);
				if (rgb_buffer) {
					memcpy(rgb_buffer, cur_rgb, FREENECT_VIDEO_RGB_SIZE);
					cur_rgb = rgb_buffer;
				}
				cur_rgb_cb(fake_dev, cur_rgb, timestamp);
			}
			break;
		case 'a':
			if (data_size == sizeof(state)) {
				memcpy(&state, data, sizeof(state));
			} else if (!already_warned) {
				already_warned = 1;
				printf("\n\nWarning: Accelerometer data has an unexpected"
				       " size [%u] instead of [%u].  The acceleration "
				       "and tilt data will be substituted for dummy "
				       "values.  This data was probably made with an "
				       "older version of record (the upstream interface "
				       "changed).\n\n",
				       data_size, (unsigned int)sizeof state);
			}
			break;
	}
	free(data);
	playback_prev_time = get_time();
	return 0;
}

double freenect_get_tilt_degs(freenect_raw_tilt_state *state)
{
	// NOTE: This is duped from tilt.c, this is the only function we need from there
	return ((double)state->tilt_angle) / 2.;
}

freenect_raw_tilt_state* freenect_get_tilt_state(freenect_device *dev)
{
	return &state;
}

void freenect_get_mks_accel(freenect_raw_tilt_state *state, double* x, double* y, double* z)
{
	//the documentation for the accelerometer (http://www.kionix.com/Product%20Sheets/KXSD9%20Product%20Brief.pdf)
	//states there are 819 counts/g
	*x = (double)state->accelerometer_x/FREENECT_COUNTS_PER_G*GRAVITY;
	*y = (double)state->accelerometer_y/FREENECT_COUNTS_PER_G*GRAVITY;
	*z = (double)state->accelerometer_z/FREENECT_COUNTS_PER_G*GRAVITY;
}

void freenect_set_depth_callback(freenect_device *dev, freenect_depth_cb cb)
{
	cur_depth_cb = cb;
}

void freenect_set_video_callback(freenect_device *dev, freenect_video_cb cb)
{
	cur_rgb_cb = cb;
}

int freenect_num_devices(freenect_context *ctx)
{
	// Always 1 device
	return 1;
}

int freenect_open_device(freenect_context *ctx, freenect_device **dev, int index)
{
	// Set it to some number to allow for NULL checks
	*dev = fake_dev;
	return 0;
}

int freenect_init(freenect_context **ctx, freenect_usb_context *usb_ctx)
{
	*ctx = fake_ctx;
	return 0;
}

int freenect_set_depth_buffer(freenect_device *dev, void *buf)
{
	depth_buffer = buf;
	return 0;
}

int freenect_set_video_buffer(freenect_device *dev, void *buf)
{
	rgb_buffer = buf;
	return 0;
}

void freenect_set_user(freenect_device *dev, void *user)
{
	user_ptr = user;
}

void *freenect_get_user(freenect_device *dev)
{
	return user_ptr;
}

int freenect_start_depth(freenect_device *dev)
{
	depth_running = 1;
	return 0;
}

int freenect_start_video(freenect_device *dev)
{
	rgb_running = 1;
	return 0;
}

int freenect_stop_depth(freenect_device *dev)
{
	depth_running = 0;
	return 0;
}

int freenect_stop_video(freenect_device *dev)
{
	rgb_running = 0;
	return 0;
}

int freenect_set_video_format(freenect_device *dev, freenect_video_format fmt)
{
	assert(fmt == FREENECT_VIDEO_RGB);
	return 0;
}
int freenect_set_depth_format(freenect_device *dev, freenect_depth_format fmt)
{
	assert(fmt == FREENECT_DEPTH_11BIT);
	return 0;
}

void freenect_set_log_callback(freenect_context *ctx, freenect_log_cb cb) {}
void freenect_set_log_level(freenect_context *ctx, freenect_loglevel level) {}
int freenect_shutdown(freenect_context *ctx)
{
	return 0;
}
int freenect_close_device(freenect_device *dev)
{
	return 0;
}
int freenect_set_tilt_degs(freenect_device *dev, double angle)
{
	return 0;
}
int freenect_set_led(freenect_device *dev, freenect_led_options option)
{
	return 0;
}
int freenect_update_tilt_state(freenect_device *dev)
{
	return 0;
}
