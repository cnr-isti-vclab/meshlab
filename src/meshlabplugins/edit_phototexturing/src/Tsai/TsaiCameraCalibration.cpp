#include <src/Tsai/TsaiCameraCalibration.h>

#include <vcg/math/matrix44.h>
#include <string>
#include <sstream>


TsaiCameraCalibration::TsaiCameraCalibration(){

}
TsaiCameraCalibration::~TsaiCameraCalibration(){

}

void TsaiCameraCalibration::loadCalibration(QString calibFile){

	printf("TsaiCameraCalibration::loadCalibration\n");

	printf("load dona\n");
}

void TsaiCameraCalibration::getUVforPoint(double x, double y, double z, double *u, double *v){
	world_coord_to_image_coord_cp_cc(&cam_para,&calib_const, x,y,z,u,v);
	//qDebug("u: %f\tv: %f\n",*u,*v);
}

void TsaiCameraCalibration::loadFromXml(QDomElement *xml_cam){

	QDomElement xml_sensor = xml_cam->firstChildElement("sensor");
	QString dpx = xml_sensor.attribute("dpx");
	QString dpy = xml_sensor.attribute("dpy");
	QString sx = xml_sensor.attribute("sx");
	QString Cx = xml_sensor.attribute("Cx");
	QString Cy = xml_sensor.attribute("Cy");

	cam_para.dpx = dpx.toDouble();
	cam_para.dpy = dpy.toDouble();
	cam_para.sx = sx.toDouble();
	cam_para.Cx = Cx.toDouble();
	cam_para.Cy = Cy.toDouble();

	QDomElement xml_cc = xml_cam->firstChildElement("cc");
	QString f = xml_cc.attribute("f");
	QString kappa1 = xml_cc.attribute("kappa1");
	QString p1 = xml_cc.attribute("p1");
	QString p2 = xml_cc.attribute("p2");

	calib_const.f = f.toDouble();
	calib_const.kappa1 = kappa1.toDouble();
	//calib_const.p1 = p1.toDouble();
	//calib_const.p2 = p2.toDouble();

	QDomElement xml_rotation = xml_cam->firstChildElement("rotation");
	QString rx = xml_rotation.attribute("x");
	QString ry = xml_rotation.attribute("y");
	QString rz = xml_rotation.attribute("z");

	calib_const.Rx = rx.toDouble();
	calib_const.Ry = ry.toDouble();
	calib_const.Rz = rz.toDouble();

	QDomElement xml_translation = xml_cam->firstChildElement("translation");
	QString tx = xml_translation.attribute("x");
	QString ty = xml_translation.attribute("y");
	QString tz = xml_translation.attribute("z");

	calib_const.Tx = tx.toDouble();
	calib_const.Ty = ty.toDouble();
	calib_const.Tz = tz.toDouble();

	QDomElement xml_matrix = xml_cam->firstChildElement("matrix");
	QString r1 = xml_matrix.attribute("r1");
	QString r2 = xml_matrix.attribute("r2");
	QString r3 = xml_matrix.attribute("r3");
	QString r4 = xml_matrix.attribute("r4");
	QString r5 = xml_matrix.attribute("r5");
	QString r6 = xml_matrix.attribute("r6");
	QString r7 = xml_matrix.attribute("r7");
	QString r8 = xml_matrix.attribute("r8");
	QString r9 = xml_matrix.attribute("r9");

	calib_const.r1 = r1.toDouble();
	calib_const.r2 = r2.toDouble();
	calib_const.r3 = r3.toDouble();
	calib_const.r4 = r4.toDouble();
	calib_const.r5 = r5.toDouble();
	calib_const.r6 = r6.toDouble();
	calib_const.r7 = r7.toDouble();
	calib_const.r8 = r8.toDouble();
	calib_const.r9 = r9.toDouble();

	double dmatrix[16];
	dmatrix[0]= calib_const.r1;
	dmatrix[1]= calib_const.r2;
	dmatrix[2]= calib_const.r3;
	dmatrix[3]= calib_const.Tx;

	dmatrix[4]= calib_const.r4;
	dmatrix[5]= calib_const.r5;
	dmatrix[6]= calib_const.r6;
	dmatrix[7]= calib_const.Ty;

	dmatrix[8]= calib_const.r7;
	dmatrix[9]= calib_const.r8;
	dmatrix[10]= calib_const.r9;
	dmatrix[11]= calib_const.Tz;

	dmatrix[12]= 0;
	dmatrix[13]= 0;
	dmatrix[14]= 0;
	dmatrix[15]= 1;

	vcg::Matrix44<double> *matrix;
	matrix = new vcg::Matrix44<double>(dmatrix);

	printf("matrix: %f\n",matrix->ElementAt(2,2));


	vcg::Matrix44<double> invmatrix = vcg::Inverse(*matrix);
	double dcam_cam_pos[] = {0.0,0.0,0.0,1.0};
	vcg::Point4<double> cam_cam_pos = vcg::Point4<double>(dcam_cam_pos);

	vcg::Point4<double> cam_pos = invmatrix*cam_cam_pos;

	printf("camera position: %f\t%f\t%f\t%f\n",cam_pos[0],cam_pos[1],cam_pos[2],cam_pos[3]);

	double dcam_cam_direction[] = {0.0,0.0,1.0,1.0};
	vcg::Point4<double> cam_cam_direction = vcg::Point4<double>(dcam_cam_direction);

	vcg::Point4<double> cam_direction = invmatrix*cam_cam_direction;
	cam_direction -= cam_pos;
	printf("camera direction: %f\t%f\t%f\t%f\n",cam_direction[0],cam_direction[1],cam_direction[2],cam_direction[3]);

	cameraPosition[0]=cam_pos[0];
	cameraPosition[1]=cam_pos[1];
	cameraPosition[2]=cam_pos[2];

	cameraDirection[0]=cam_direction[0];
	cameraDirection[1]=cam_direction[1];
	cameraDirection[2]=cam_direction[2];

}

void TsaiCameraCalibration::saveAsXml(QDomDocument* doc,QDomElement *root){
	QDomElement xml_calib = doc->createElement("calibration");
	xml_calib.setAttribute("type","TSAI");

	QDomElement xml_sensor = doc->createElement("sensor");
	xml_sensor.setAttribute("dpx",QString::number(cam_para.dpx));
	xml_sensor.setAttribute("dpy",QString::number(cam_para.dpy));
	xml_sensor.setAttribute("Cx",QString::number(cam_para.Cx));
	xml_sensor.setAttribute("Cy",QString::number(cam_para.Cy));
	xml_sensor.setAttribute("sx",QString::number(cam_para.sx));
	xml_calib.appendChild(xml_sensor);

	QDomElement xml_cc = doc->createElement("cc");
	xml_cc.setAttribute("f",QString::number(calib_const.f));
	xml_cc.setAttribute("kappa1",QString::number(calib_const.kappa1));
	xml_cc.setAttribute("p1",QString::number(calib_const.p1));
	xml_cc.setAttribute("p2",QString::number(calib_const.p2));
	xml_calib.appendChild(xml_cc);

	QDomElement xml_translation = doc->createElement("translation");
	xml_translation.setAttribute("x",QString::number(calib_const.Tx));
	xml_translation.setAttribute("y",QString::number(calib_const.Ty));
	xml_translation.setAttribute("z",QString::number(calib_const.Tz));
	xml_calib.appendChild(xml_translation);

	QDomElement xml_rotation = doc->createElement("rotation");
	xml_rotation.setAttribute("x",QString::number(calib_const.Rx));
	xml_rotation.setAttribute("y",QString::number(calib_const.Ry));
	xml_rotation.setAttribute("z",QString::number(calib_const.Rz));
	xml_calib.appendChild(xml_rotation);

	QDomElement xml_matrix = doc->createElement("matrix");
	xml_matrix.setAttribute("r1",QString::number(calib_const.r1));
	xml_matrix.setAttribute("r2",QString::number(calib_const.r2));
	xml_matrix.setAttribute("r3",QString::number(calib_const.r3));
	xml_matrix.setAttribute("r4",QString::number(calib_const.r4));
	xml_matrix.setAttribute("r5",QString::number(calib_const.r5));
	xml_matrix.setAttribute("r6",QString::number(calib_const.r6));
	xml_matrix.setAttribute("r7",QString::number(calib_const.r7));
	xml_matrix.setAttribute("r8",QString::number(calib_const.r8));
	xml_matrix.setAttribute("r9",QString::number(calib_const.r9));

	xml_calib.appendChild(xml_matrix);
	root->appendChild(xml_calib);

}

void TsaiCameraCalibration::calibrate(QList<CameraCalibrationData*> &ccd ){

	initialize_eos400D_parms();

	cd.point_count = 0;
	int max_points;

	if (ccd.size()<MAX_POINTS){
		max_points = ccd.size();
	}else{
		max_points= MAX_POINTS;
	}
	int k = 0;
	//printf("malloc\n");
	int* done = (int*)malloc(ccd.size()*sizeof(int));
	memset(done,0,ccd.size());
	printf("try to find points\n");
	while (k<max_points){
		int ran = rand() % ccd.size();
		//printf("max_points: %d\tk: %d\n",max_points,k);
		if (done[ran]==0){
			done[ran]=1;
			CameraCalibrationData* tmp_ccd;
			tmp_ccd = ccd.at(ran);
			cd.xw[k] = tmp_ccd->wX;
			cd.yw[k] = tmp_ccd->wY;
			cd.zw[k] = tmp_ccd->wZ;
			cd.Xf[k] = tmp_ccd->iX;
			cd.Yf[k] = tmp_ccd->iY;
			cd.point_count++;
			k++;
		}
	}

	noncoplanar_calibration_with_full_optimization ();

	//noncoplanar_calibration ();
	//print_cp_cc_data (stderr, &cp, &cc);
	//print_error_stats (stderr);
	//dump_cp_cc_data (stdout, &cp, &cc);


	double dmatrix[16];
	dmatrix[0]= cc.r1;
	dmatrix[1]= cc.r2;
	dmatrix[2]= cc.r3;
	dmatrix[3]= cc.Tx;

	dmatrix[4]= cc.r4;
	dmatrix[5]= cc.r5;
	dmatrix[6]= cc.r6;
	dmatrix[7]= cc.Ty;

	dmatrix[8]= cc.r7;
	dmatrix[9]= cc.r8;
	dmatrix[10]= cc.r9;
	dmatrix[11]= cc.Tz;

	dmatrix[12]= 0;
	dmatrix[13]= 0;
	dmatrix[14]= 0;
	dmatrix[15]= 1;

	vcg::Matrix44<double> *matrix;
	matrix = new vcg::Matrix44<double>(dmatrix);

	printf("matrix: %f\n",matrix->ElementAt(2,2));


	vcg::Matrix44<double> invmatrix = vcg::Inverse(*matrix);
	double dcam_cam_pos[] = {0.0,0.0,0.0,1.0};
	vcg::Point4<double> cam_cam_pos = vcg::Point4<double>(dcam_cam_pos);

	vcg::Point4<double> cam_pos = invmatrix*cam_cam_pos;

	printf("camera position: %f\t%f\t%f\t%f\n",cam_pos[0],cam_pos[1],cam_pos[2],cam_pos[3]);

}


