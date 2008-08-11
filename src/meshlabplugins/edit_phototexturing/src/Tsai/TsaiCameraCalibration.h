#ifndef TSAICAMERACALIBRATION_H_
#define TSAICAMERACALIBRATION_H_

#include <src/CameraCalibration.h>

extern "C" {

	#include <src/Tsai/cal_main.h>
	#include <src/Tsai/cal_util.h>
}

class TsaiCameraCalibration: public CameraCalibration{
public:

	TsaiCameraCalibration();
	virtual ~TsaiCameraCalibration();
	virtual void loadCalibration(QString);
	virtual void loadFromXml(QDomElement *xml_cam);
	virtual void saveAsXml(QDomDocument* doc,QDomElement *root);
	virtual void getUVforPoint(double x, double y, double z, double *u, double *v);
	virtual void calibrate(QList<CameraCalibrationData*> &ccd );
	void calibrate(QList<CameraCalibrationData*> &ccd, bool optimize);
	virtual CameraCalibration* calibrateToTsai(MeshModel *mm, bool optimize);
//private:
	struct camera_parameters cam_para;
	struct calibration_constants calib_const;
};

#endif /*TSAICAMERACALIBRATION_H_*/
