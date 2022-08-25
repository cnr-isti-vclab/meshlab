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
#include <unistd.h>
#include <string.h>
#include "libfreenect.h"
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>

#include <math.h>

int g_argc;
char **g_argv;

freenect_context *f_ctx;
freenect_device *f_dev;

int got_frames = 0;

#define AS3_BITMAPDATA_LEN 640 * 480 * 4

struct sockaddr_in si_depth, si_rgb, si_data;
pthread_t freenect_thread, depth_thread, rgb_thread, data_thread, data_in_thread, data_out_thread;
pthread_mutex_t depth_mutex	= PTHREAD_MUTEX_INITIALIZER,
rgb_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t depth_cond = PTHREAD_COND_INITIALIZER,
rgb_cond = PTHREAD_COND_INITIALIZER;

int freenect_angle = 0;

char *conf_ip		= "127.0.0.1";
int s_depth			= -1,
s_rgb			= -1,
s_data			= -1,
conf_port_depth	= 6001,
conf_port_rgb	= 6002,
conf_port_data	= 6003;

uint8_t buf_depth[AS3_BITMAPDATA_LEN];
uint8_t	buf_rgb[AS3_BITMAPDATA_LEN];

int die = 0;

int depth_child;
int depth_connected = 0;

int rgb_child;
int rgb_connected = 0;

int data_child;
int data_connected = 0;


int psent = 0;

void send_policy_file(int child){
	if(psent == 0){
		int n;
		char * str = "<?xml version='1.0'?><!DOCTYPE cross-domain-policy SYSTEM '/xml/dtds/cross-domain-policy.dtd'><cross-domain-policy><site-control permitted-cross-domain-policies='all'/><allow-access-from domain='*' to-ports='*'/></cross-domain-policy>\n";
		//n = write(child,str , 237);
		if ( n < 0 || n != 237)
		{
			fprintf(stderr, "Error on write() for depth (%d instead of %d)\n",n, 237);
			//break;
		}
		psent = 1;
	}
}

void *network_depth(void *arg)
{
	int childlen;
	struct sockaddr_in childaddr;
	
	childlen = sizeof(childaddr);
	while ( !die )
	{
		printf("### Wait depth client\n");
		depth_child = accept(s_depth, (struct sockaddr *)&childaddr, (unsigned int *)&childlen);
		if ( network_depth < 0 )
		{
			fprintf(stderr, "Error on accept() for depth, exit depth thread.\n");
			break;
		}
		
		printf("### Got depth client\n");
		send_policy_file(depth_child);
		depth_connected = 1;
		freenect_start_depth(f_dev);
	}
	
	return NULL;
}

void *network_rgb(void *arg)
{
	int childlen;
	struct sockaddr_in childaddr;
	
	childlen = sizeof(childaddr);
	while ( !die )
	{
		printf("### Wait rgb client\n");
		rgb_child = accept(s_rgb, (struct sockaddr *)&childaddr, (unsigned int *)&childlen);
		if ( rgb_child < 0 )
		{
			fprintf(stderr, "Error on accept() for rgb, exit rgb thread.\n");
			break;
		}
		
		printf("### Got rgb client\n");
		send_policy_file(rgb_child);
		rgb_connected = 1;
		freenect_start_video(f_dev);
	}
	
	return NULL;
}

void *network_data(void *arg)
{
	int childlen;
	struct sockaddr_in childaddr;
	
	childlen = sizeof(childaddr);
	while ( !die )
	{
		printf("### Wait data client\n");
		data_child = accept(s_data, (struct sockaddr *)&childaddr, (unsigned int *)&childlen);
		if ( data_child < 0 )
		{
			fprintf(stderr, "Error on accept() for data, exit data thread.\n");
			break;
		}
		
		printf("### Got data client\n");
		
		data_connected = 1;
	}
	
	return NULL;
}

void *data_in(void *arg) {
	int n;
	while(!die && freenect_process_events(f_ctx) >= 0 )
	{
		if(data_connected == 1){
			char buffer[6];
			n = read(data_child, buffer, 1024);
			//printf("n: %d\n", n);
			if(n == 6){
				if (buffer[0] == 1) { //MOTOR
					if (buffer[1] == 1) { //MOVE
						int angle;
						memcpy(&angle, &buffer[2], sizeof(int));
						freenect_set_tilt_degs(f_dev,ntohl(angle));
					}
				}
			}
		}
	}
	return NULL;
}

void *data_out(void *arg) {
	int n;
	int16_t ax,ay,az;
	double dx,dy,dz;
	while(!die && freenect_process_events(f_ctx) >= 0 )
	{
		if(data_connected == 1){
			usleep(1000000 / 30); // EMULATE 30 FPS
			freenect_raw_tilt_state* state;
			freenect_update_tilt_state(f_dev);
			state = freenect_get_tilt_state(f_dev);
			freenect_get_mks_accel(state, &dx, &dy, &dz);
			char buffer_send[3*2+3*8];
			memcpy(&buffer_send,&state->accelerometer_x, sizeof(int16_t));
			memcpy(&buffer_send[2],&state->accelerometer_y, sizeof(int16_t));
			memcpy(&buffer_send[4],&state->accelerometer_z, sizeof(int16_t));
			memcpy(&buffer_send[6],&dx, sizeof(double));
			memcpy(&buffer_send[14],&dy, sizeof(double));
			memcpy(&buffer_send[22],&dz, sizeof(double));
			n = write(data_child, buffer_send, 3*2+3*8);
		}
	}
	
	return NULL;
}
int network_init()
{
	int optval = 1;
	
	signal(SIGPIPE, SIG_IGN);
	
	if ( (s_depth = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		fprintf(stderr, "Unable to create depth socket\n");
		return -1;
	}
	
	if ( (s_rgb = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		fprintf(stderr, "Unable to create rgb socket\n");
		return -1;
	}
	
	if ( (s_data = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		fprintf(stderr, "Unable to create data socket\n");
		return -1;
	}
	
	setsockopt(s_depth, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(optval));
	setsockopt(s_rgb, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(optval));
	setsockopt(s_data, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(optval));
	
	memset((char *) &si_depth, 0, sizeof(si_depth));
	memset((char *) &si_rgb, 0, sizeof(si_rgb));
	memset((char *) &si_data, 0, sizeof(si_data));
	
	si_depth.sin_family			= AF_INET;
	si_depth.sin_port			= htons(conf_port_depth);
	si_depth.sin_addr.s_addr	= inet_addr(conf_ip);
	
	si_rgb.sin_family			= AF_INET;
	si_rgb.sin_port				= htons(conf_port_rgb);
	si_rgb.sin_addr.s_addr		= inet_addr(conf_ip);
	
	si_data.sin_family			= AF_INET;
	si_data.sin_port			= htons(conf_port_data);
	si_data.sin_addr.s_addr		= inet_addr(conf_ip);
	
	if ( bind(s_depth, (struct sockaddr *)&si_depth,
			  sizeof(si_depth)) < 0 )
	{
		fprintf(stderr, "Error at bind() for depth\n");
		return -1;
	}
	
	if ( bind(s_rgb, (struct sockaddr *)&si_rgb,
			  sizeof(si_rgb)) < 0 )
	{
		fprintf(stderr, "Error at bind() for rgb\n");
		return -1;
	}
	
	if ( bind(s_data, (struct sockaddr *)&si_data,
			  sizeof(si_data)) < 0 )
	{
		fprintf(stderr, "Error at bind() for data\n");
		return -1;
	}
	
	if ( listen(s_depth, 1) < 0 )
	{
		fprintf(stderr, "Error on listen() for depth\n");
		return -1;
	}
	
	if ( listen(s_rgb, 1) < 0 )
	{
		fprintf(stderr, "Error on listen() for rgb\n");
		return -1;
	}
	
	if ( listen(s_data, 1) < 0 )
	{
		fprintf(stderr, "Error on listen() for data\n");
		return -1;
	}
	
	/* launch 3 threads, 2 for each images and 1 for control
	 */
	
	if ( pthread_create(&depth_thread, NULL, network_depth, NULL) )
	{
		fprintf(stderr, "Error on pthread_create() for depth\n");
		return -1;
	}
	
	if ( pthread_create(&rgb_thread, NULL, network_rgb, NULL) )
	{
		fprintf(stderr, "Error on pthread_create() for rgb\n");
		return -1;
	}
	
	if ( pthread_create(&data_thread, NULL, network_data, NULL) )
	{
		fprintf(stderr, "Error on pthread_create() for data\n");
		return -1;
	}
	
	
	return 0;
}

void network_close()
{
	die = 1;
	if ( s_depth != -1 )
		close(s_depth), s_depth = -1;
	if ( s_rgb != -1 )
		close(s_rgb), s_rgb = -1;
}

uint16_t t_gamma[2048];

void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
	int i, n;
	uint16_t *depth = v_depth;
	
	pthread_mutex_lock(&depth_mutex);
	for (i=0; i<FREENECT_FRAME_PIX; i++) {
		int pval = t_gamma[depth[i]];
		int lb = pval & 0xff;
		switch (pval>>8) {
			case 0:
				buf_depth[4 *  i + 2] = 255;
				buf_depth[4 *  i + 1] = 255-lb;
				buf_depth[4 *  i + 0] = 255-lb;
				break;
			case 1:
				buf_depth[4 *  i + 2] = 255;
				buf_depth[4 *  i + 1] = lb;
				buf_depth[4 *  i + 0] = 0;
				break;
			case 2:
				buf_depth[4 *  i + 2] = 255-lb;
				buf_depth[4 *  i + 1] = 255;
				buf_depth[4 *  i + 0] = 0;
				break;
			case 3:
				buf_depth[4 *  i + 2] = 0;
				buf_depth[4 *  i + 1] = 255;
				buf_depth[4 *  i + 0] = lb;
				break;
			case 4:
				buf_depth[4 *  i + 2] = 0;
				buf_depth[4 *  i + 1] = 255-lb;
				buf_depth[4 *  i + 0] = 255;
				break;
			case 5:
				buf_depth[4 *  i + 2] = 0;
				buf_depth[4 *  i + 1] = 0;
				buf_depth[4 *  i + 0] = 255-lb;
				break;
			default:
				buf_depth[4 *  i + 2] = 0;
				buf_depth[4 *  i + 1] = 0;
				buf_depth[4 *  i + 0] = 0;
				break;
		}
		buf_depth[4 *  i + 3] = 0x00;
	}
	got_frames++;
	 
	if ( depth_connected == 1 )
	{	
		n = write(depth_child, buf_depth, AS3_BITMAPDATA_LEN);
		if ( n < 0 || n != AS3_BITMAPDATA_LEN)
		{
			fprintf(stderr, "Error on write() for depth (%d instead of %d)\n",n, AS3_BITMAPDATA_LEN);
		}
	}
	pthread_cond_signal(&depth_cond);
	pthread_mutex_unlock(&depth_mutex);
}

void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
	int n;
	pthread_mutex_lock(&depth_mutex);
	got_frames++;
	//memcpy(buf_rgb, rgb, FREENECT_RGB_SIZE);
	//unsigned char * tmp_depth;
	//int compressed_size = SaveJPGToBuffer(tmp_depth, 75, 640, 480, rgb, 640*480);
	//printf("size: %d", compressed_size);
	int x;
	for (x=0; x<640 * 480; x++) {
		buf_rgb[4 * x + 0] = ((uint8_t*)rgb)[3 * x + 2];
		buf_rgb[4 * x + 1] = ((uint8_t*)rgb)[3 * x + 1];
		buf_rgb[4 * x + 2] = ((uint8_t*)rgb)[3 * x + 0];
		buf_rgb[4 * x + 3] = 0x00;
	}
	printf("rgb received\n ");
	if ( rgb_connected == 1 )
	{
		n = write(rgb_child, buf_rgb, AS3_BITMAPDATA_LEN);
		if ( n < 0 || n != AS3_BITMAPDATA_LEN)
		{
			fprintf(stderr, "Error on write() for rgb (%d instead of %d)\n",n, AS3_BITMAPDATA_LEN);
			//break;
		}
	}
	pthread_cond_signal(&depth_cond);
	pthread_mutex_unlock(&depth_mutex);
}

void *freenect_threadfunc(void *arg)
{
	freenect_set_tilt_degs(f_dev,freenect_angle);
	freenect_set_led(f_dev,LED_RED);
	freenect_set_depth_callback(f_dev, depth_cb);
	freenect_set_video_callback(f_dev, rgb_cb);
	freenect_set_video_format(f_dev, FREENECT_VIDEO_RGB);
	freenect_set_depth_format(f_dev, FREENECT_DEPTH_11BIT);
	
	printf("'w'-tilt up, 's'-level, 'x'-tilt down, '0'-'6'-select LED mode\n");
	if ( pthread_create(&data_in_thread, NULL, data_in, NULL) )
	{
		fprintf(stderr, "Error on pthread_create() for data_in\n");
	}
	
	if ( pthread_create(&data_out_thread, NULL, data_out, NULL) )
	{
		fprintf(stderr, "Error on pthread_create() for data_out\n");
	}
	
	while(!die && freenect_process_events(f_ctx) >= 0 );

	printf("\nshutting down streams...\n");

	freenect_stop_depth(f_dev);
	freenect_stop_video(f_dev);
	network_close();
	
	printf("-- done!\n");
	return NULL;
}

int main(int argc, char **argv)
{
	int i, res;
	for (i=0; i<2048; i++) {
		float v = i/2048.0;
		v = powf(v, 3)* 6;
		t_gamma[i] = v*6*256;
	}
	
	g_argc = argc;
	g_argv = argv;
	
	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		return 1;
	}
	
	freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
	
	int nr_devices = freenect_num_devices (f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);
	
	int user_device_number = 0;
	if (argc > 1)
		user_device_number = atoi(argv[1]);
	
	if (nr_devices < 1)
		return 1;
	
	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		return 1;
	}
	
	if ( network_init() < 0 )
		return -1;
	
	res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
	if (res) {
		printf("pthread_create failed\n");
		return 1;
	}

	while(!die && freenect_process_events(f_ctx) >= 0 );
	
	return 0;
}
