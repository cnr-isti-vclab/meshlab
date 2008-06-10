#include "Camera.h"
#include <src/Tsai/TsaiCameraCalibration.h>


const QString Camera::XML_CAMERA = "camera";
const QString Camera::XML_NAME = "name";
const QString Camera::XML_IMAGE = "textureImage";

Camera::Camera(){
	
	textureImage = QString("");
	calculatedTextures = false;
}
Camera::~Camera(){
	
}
void Camera::loadCameraCalibration(QString calibfile){
	
}
void Camera::calibrateCamera(){
	
}

void Camera::assignTextureImage(QString image){
	textureImage = image;
}
void Camera::saveAsXml(QDomDocument* doc,QDomElement *root){
	QDomElement xml_cam = doc->createElement(XML_CAMERA);
	xml_cam.setAttribute(XML_NAME,name);
	xml_cam.setAttribute(XML_IMAGE,textureImage);
	
	calibration->saveAsXml(doc,&xml_cam);
	
	root->appendChild(xml_cam);
	
}

void Camera::loadFromXml(QDomElement *xml_cam){
	
	name = xml_cam->attribute(XML_NAME);
	textureImage  = xml_cam->attribute(XML_IMAGE);
	
	QDomElement xml_calib = xml_cam->firstChildElement(CameraCalibration::XML_CALIBRATION);
	if (!xml_calib.isNull()){
		QString ctype = xml_calib.attribute(CameraCalibration::XML_TYPE);
		//qDebug("ctype: %s\n",qPrintable(ctype));
		if (!ctype.compare("TSAI")){
			qDebug("TSAI\n");
			calibration = new TsaiCameraCalibration();
			calibration->loadFromXml(&xml_calib);
		}else{
		}
	}else{
		//qDebug("no calibration child in xmlfile \n");
	}
	
}

