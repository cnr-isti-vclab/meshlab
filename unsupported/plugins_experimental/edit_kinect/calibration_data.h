#ifndef CALIBRATION_DATA_H
#define CALIBRATION_DATA_H

#include <stdio.h>
#include <QString>

#include <vcg/math/matrix33.h>


extern vcg::Matrix33f rgb_intrinsics,depth_intrinsics,R;
extern float rgb_distortion[5];
extern float depth_distortion[5];
extern vcg::Point3f T;
extern int rgb_size[2],raw_rgb_size[2],depth_size[2],raw_depth_size[2];

bool LoadCalibrationData(QString);
void DefaultCalibrationData();
#endif // CALIBRATION_DATA_H
