#ifndef CAMERACALIBRATION_H_
#define CAMERACALIBRATION_H_

#include <QObject>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

//#include <src/Camera.h>
#include <src/CameraCalibrationData.h> 
#include <meshlab/meshmodel.h>
class CameraCalibration{
public:
	
	static const QString XML_CALIBRATION;
	static const QString XML_TYPE;
	
	//virtual CameraCalibration();
	virtual ~CameraCalibration(){};
	virtual void loadCalibration(QString ){};
	virtual void loadFromXml(QDomElement *xml_cam){
		
	};
	virtual void saveAsXml(QDomDocument* doc,QDomElement *root){
		
	};
	virtual void getUVforPoint(double x, double y, double z, double *u, double *v){
		
	};
	virtual void calibrate(QList<CameraCalibrationData*> &ccd ){
		
	};
	
	virtual void calibrateToTsai(MeshModel *mm){
		
	};
//private:
	//Camera *camera;
	
};

#endif /*CAMERACALIBRATION_H_*/
