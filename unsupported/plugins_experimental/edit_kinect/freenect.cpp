
#include <libfreenect.h>
#include "freenect.h"

#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <stdio.h>
#include <stdlib.h>


#include <math.h>

namespace kinect_qt{


uint16_t gl_depth_front[640*480];
uint16_t gl_depth_back[640*480];
uint8_t gl_rgb_front[640*480*4];
uint8_t gl_rgb_back[640*480*4];

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_angle;
int freenect_led;
volatile int die = 0;
int got_frames = 0;
QMutex qbackbuf_mutex, qstop_mutex;
QWaitCondition qwait;

void  KinectThread::run()
{
       printf("'w'-tilt up, 's'-level, 'x'-tilt down, '0'-'6'-select LED mode\n");

       while(!die && freenect_process_events(f_ctx) >= 0 )
       {
//               int16_t ax,ay,az;
//               freenect_get_raw_accel(f_dev, &ax, &ay, &az);
//               double dx,dy,dz;
//               freenect_get_mks_accel(f_dev, &dx, &dy, &dz);
//               printf("\r raw acceleration: %4d %4d %4d  mks acceleration: %4f %4f %4f\r", ax, ay, az, dx, dy, dz);
//               fflush(stdout);
       }

       printf("\nshutting down streams...\n");

       freenect_stop_depth(f_dev);
       freenect_stop_video(f_dev);

       freenect_close_device(f_dev);
       freenect_shutdown(f_ctx);
       die = false;
       printf("-- done!\n");
       qstop_mutex.unlock();

}


static void rgb_cb(freenect_device */*dev*/, void *rgb, uint32_t /*timestamp*/)
{
        qbackbuf_mutex.lock();
        got_frames++;
        memcpy(gl_rgb_back, rgb, FREENECT_VIDEO_RGB_SIZE);
        qwait.wakeAll();
        qbackbuf_mutex.unlock();
}


static void depth_cb(freenect_device */*dev*/, void *v_depth, uint32_t /*timestamp*/)
{
        qbackbuf_mutex.lock();
        memcpy(gl_depth_front, v_depth, sizeof(v_depth)*640*480);
        got_frames++;
        qbackbuf_mutex.unlock();
        qwait.wakeAll();
}

KinectThread kinect_thread;



void  start_kinect ()
{
  // initialize Kinect
  printf("Kinect camera test\n");



  if (freenect_init(&f_ctx, NULL) < 0) {
          printf("freenect_init() failed\n");
          exit(0);
  }

  freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);

  int nr_devices = freenect_num_devices (f_ctx);
  printf ("Number of devices found: %d\n", nr_devices);

  int user_device_number = 0;

  if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
          printf("Could not open device\n");
          exit(0);
  }

  freenect_set_tilt_degs(f_dev,0.f/*freenect_angle*/);
  freenect_set_led(f_dev,LED_RED);
  freenect_set_depth_callback(f_dev, depth_cb);
  freenect_set_video_callback(f_dev, rgb_cb);
  freenect_set_video_format(f_dev, FREENECT_VIDEO_RGB);
  freenect_set_depth_format(f_dev, FREENECT_DEPTH_11BIT);

  freenect_start_depth(f_dev);
  freenect_start_video(f_dev);

  kinect_thread.start();

}

void stop_kinect(){
    qstop_mutex.lock();
    die = true;
    qstop_mutex.lock();
    qstop_mutex.unlock();
}

void  set_tilt_degs(double angle){freenect_set_tilt_degs(f_dev,angle);}

} // end namespace kinect_qt
