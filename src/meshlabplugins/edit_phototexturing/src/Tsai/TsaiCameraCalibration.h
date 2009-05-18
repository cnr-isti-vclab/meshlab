#ifndef TSAICAMERACALIBRATION_H_
#define TSAICAMERACALIBRATION_H_
extern "C" {

	#include "../../../external/TSAI30B3/CAL_MAIN.h"
	//#include "../../../external/TSAI30B3/CAL_UTIL.h"
}

#include <src/CameraCalibration.h>


class TsaiCameraCalibration: public CameraCalibration{
public:

	static const QString XML_TSAICALIB_TYPE;
	static const QString XML_TSAICALIB_OPTIMIZED;
	
	static const QString XML_TSAICALIB_SENSOR;
	static const QString XML_TSAICALIB_DPX;
	static const QString XML_TSAICALIB_DPY;
	static const QString XML_TSAICALIB_SX;
	static const QString XML_TSAICALIB_CX;
	static const QString XML_TSAICALIB_CY;
	static const QString XML_TSAICALIB_CC;
	static const QString XML_TSAICALIB_F;
	static const QString XML_TSAICALIB_KAPPA1;
	static const QString XML_TSAICALIB_P1;
	static const QString XML_TSAICALIB_P2;
	
	static const QString XML_TSAICALIB_ROTATION;
	static const QString XML_TSAICALIB_RX;
	static const QString XML_TSAICALIB_RY;
	static const QString XML_TSAICALIB_RZ;
	
	static const QString XML_TSAICALIB_TRANSLATION;
	static const QString XML_TSAICALIB_TX;
	static const QString XML_TSAICALIB_TY;
	static const QString XML_TSAICALIB_TZ;
	
	static const QString XML_TSAICALIB_MATRIX;
	static const QString XML_TSAICALIB_R1;
	static const QString XML_TSAICALIB_R2;
	static const QString XML_TSAICALIB_R3;
	static const QString XML_TSAICALIB_R4;
	static const QString XML_TSAICALIB_R5;
	static const QString XML_TSAICALIB_R6;
	static const QString XML_TSAICALIB_R7;
	static const QString XML_TSAICALIB_R8;
	static const QString XML_TSAICALIB_R9;
	
	TsaiCameraCalibration();
	~TsaiCameraCalibration();
	virtual void loadFromXml(QDomElement *xml_cam);
	virtual void saveAsXml(QDomDocument* doc,QDomElement *root);
	virtual void getUVforPoint(double x, double y, double z, double *u, double *v);
	virtual void calibrate(QList<CameraCalibrationData*> &ccd );
	void calibrate(QList<CameraCalibrationData*> &ccd, bool optimize);
	virtual CameraCalibration* calibrateToTsai(MeshModel *mm, bool optimize);
//private:
	struct camera_parameters cam_para;
	struct calibration_constants calib_const;
	bool optimized;
};

#endif /*TSAICAMERACALIBRATION_H_*/
