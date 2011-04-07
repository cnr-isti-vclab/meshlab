

#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>

namespace kinect_qt{

    extern  uint16_t gl_depth_front[640*480];
    extern  uint16_t gl_depth_back[640*480];
    extern  uint8_t gl_rgb_front[640*480*4];
    extern  uint8_t gl_rgb_back[640*480*4];

    extern   int freenect_angle;
    extern   int freenect_led;
    extern volatile int die ;
    extern  int got_frames;

struct  KinectThread: public QThread{

     void run();
};

extern KinectThread kinect_thread;
extern QMutex qbackbuf_mutex;
extern QWaitCondition qwait;

void  start_kinect ();
void  set_tilt_degs(double angle);
void  stop_kinect();

} // end namespace kinect_qt
