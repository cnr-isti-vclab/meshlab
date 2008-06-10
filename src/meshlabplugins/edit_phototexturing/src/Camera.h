#ifndef CAMERA_H_
#define CAMERA_H_

#include <QObject> 
#include <QList>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include "CameraSensor.h"
#include "CameraCalibration.h"
/*
 * 
 */
class Camera{
public: 
	
	static const QString XML_CAMERA;
	static const QString XML_NAME;
	static const QString XML_IMAGE;
	
	bool calculatedTextures;
	QString name;
	//QString type;
	QString textureImage;
	CameraSensor sensor;
	CameraCalibration *calibration;
	
	Camera();
	~Camera();
	void loadCameraCalibration(QString);
	void calibrateCamera();
	void assignTextureImage(QString);
	
	/*
	 * Saves the camera model and its calibration into a
	 * 
	 */
	void saveAsXml(QDomDocument* doc,QDomElement *root);
	
	/*
	 * Loads a camera model from a xml file. 
	 */
	void loadFromXml(QDomElement *xml_cam);
};

#endif /*CAMERA_H_*/
