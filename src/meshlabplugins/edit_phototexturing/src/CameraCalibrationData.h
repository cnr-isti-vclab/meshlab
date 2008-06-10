#ifndef CAMERACALIBRATIONDATA_H_
#define CAMERACALIBRATIONDATA_H_

class CameraCalibrationData{
public:
	//world coordinates
	double wX;
	double wY;
	double wZ;
	
	//camera image coordinates
	double iX;
	double iY;	
};

#endif /*CAMERACALIBRATIONDATA_H_*/
