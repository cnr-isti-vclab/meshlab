#ifndef FREENECT_CV_H
#define FREENECT_CV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <opencv/cv.h>

	IplImage *freenect_sync_get_depth_cv(int index);
	IplImage *freenect_sync_get_rgb_cv(int index);

#ifdef __cplusplus
}
#endif

#endif