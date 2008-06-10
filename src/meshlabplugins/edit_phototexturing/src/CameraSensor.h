#ifndef CAMERASENSOR_H_
#define CAMERASENSOR_H_

class CameraSensor{
public:
	double size[2];	//sensor dimension in [mm]
	int selX;	//sensor elements per x row
	int pixX;	//pixel per x row
	int resolution[2]; //pixel resolution of the sensor
};

#endif /*CAMERASENSOR_H_*/
