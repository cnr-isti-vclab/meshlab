#include <src/Tsai/TsaiCameraCalibration.h>

#include <vcg/math/matrix44.h>
#include <string>
#include <sstream>

const QString TsaiCameraCalibration::XML_TSAICALIB_TYPE = "TSAI";
const QString TsaiCameraCalibration::XML_TSAICALIB_OPTIMIZED = "optimized";
	
const QString TsaiCameraCalibration::XML_TSAICALIB_SENSOR = "Sensor";
const QString TsaiCameraCalibration::XML_TSAICALIB_DPX = "dpx";
const QString TsaiCameraCalibration::XML_TSAICALIB_DPY = "dpy";
const QString TsaiCameraCalibration::XML_TSAICALIB_SX = "sx";
const QString TsaiCameraCalibration::XML_TSAICALIB_CX = "Cx";
const QString TsaiCameraCalibration::XML_TSAICALIB_CY = "Cy";
const QString TsaiCameraCalibration::XML_TSAICALIB_CC = "CC";
const QString TsaiCameraCalibration::XML_TSAICALIB_F = "f";
const QString TsaiCameraCalibration::XML_TSAICALIB_KAPPA1 = "kappa1";
const QString TsaiCameraCalibration::XML_TSAICALIB_P1 = "p1";
const QString TsaiCameraCalibration::XML_TSAICALIB_P2 = "p2";
	
const QString TsaiCameraCalibration::XML_TSAICALIB_ROTATION = "Rotation";
const QString TsaiCameraCalibration::XML_TSAICALIB_RX = "Rx";
const QString TsaiCameraCalibration::XML_TSAICALIB_RY = "Ry";
const QString TsaiCameraCalibration::XML_TSAICALIB_RZ = "Rz";
	
const QString TsaiCameraCalibration::XML_TSAICALIB_TRANSLATION = "Translation";
const QString TsaiCameraCalibration::XML_TSAICALIB_TX = "Tx";
const QString TsaiCameraCalibration::XML_TSAICALIB_TY = "Ty";
const QString TsaiCameraCalibration::XML_TSAICALIB_TZ = "Tz";
	
const QString TsaiCameraCalibration::XML_TSAICALIB_MATRIX = "Matrix";
const QString TsaiCameraCalibration::XML_TSAICALIB_R1 = "r1";
const QString TsaiCameraCalibration::XML_TSAICALIB_R2 = "r2";
const QString TsaiCameraCalibration::XML_TSAICALIB_R3 = "r3";
const QString TsaiCameraCalibration::XML_TSAICALIB_R4 = "r4";
const QString TsaiCameraCalibration::XML_TSAICALIB_R5 = "r5";
const QString TsaiCameraCalibration::XML_TSAICALIB_R6 = "r6";
const QString TsaiCameraCalibration::XML_TSAICALIB_R7 = "r7";
const QString TsaiCameraCalibration::XML_TSAICALIB_R8 = "r8";
const QString TsaiCameraCalibration::XML_TSAICALIB_R9 = "r9";

TsaiCameraCalibration::TsaiCameraCalibration(){
	type = TsaiCameraCalibration::XML_TSAICALIB_TYPE;
	optimized = false;
}
TsaiCameraCalibration::~TsaiCameraCalibration(){

}


void TsaiCameraCalibration::getUVforPoint(double x, double y, double z, double *u, double *v){
	world_coord_to_image_coord_cp_cc(&cam_para,&calib_const, x,y,z,u,v);
	//qDebug("u: %f\tv: %f\n",*u,*v);
}

void TsaiCameraCalibration::loadFromXml(QDomElement *xml_cam){
	
	QString soptimized = xml_cam->attribute(TsaiCameraCalibration::XML_TSAICALIB_OPTIMIZED);
	if(soptimized.toInt()==1){
		optimized = true;
	}else{
		optimized = false;
	}
	
	
	QDomElement xml_sensor = xml_cam->firstChildElement(TsaiCameraCalibration::XML_TSAICALIB_SENSOR);
	QString dpx = xml_sensor.attribute(TsaiCameraCalibration::XML_TSAICALIB_DPX);
	QString dpy = xml_sensor.attribute(TsaiCameraCalibration::XML_TSAICALIB_DPY);
	QString sx = xml_sensor.attribute(TsaiCameraCalibration::XML_TSAICALIB_SX);
	QString Cx = xml_sensor.attribute(TsaiCameraCalibration::XML_TSAICALIB_CX);
	QString Cy = xml_sensor.attribute(TsaiCameraCalibration::XML_TSAICALIB_CY);

	cam_para.dpx = dpx.toDouble();
	cam_para.dpy = dpy.toDouble();
	cam_para.sx = sx.toDouble();
	cam_para.Cx = Cx.toDouble();
	cam_para.Cy = Cy.toDouble();

	QDomElement xml_cc = xml_cam->firstChildElement(TsaiCameraCalibration::XML_TSAICALIB_CC);
	QString f = xml_cc.attribute(TsaiCameraCalibration::XML_TSAICALIB_F);
	QString kappa1 = xml_cc.attribute(TsaiCameraCalibration::XML_TSAICALIB_KAPPA1);
	QString p1 = xml_cc.attribute(TsaiCameraCalibration::XML_TSAICALIB_P1);
	QString p2 = xml_cc.attribute(TsaiCameraCalibration::XML_TSAICALIB_P2);

	calib_const.f = f.toDouble();
	calib_const.kappa1 = kappa1.toDouble();
	//calib_const.p1 = p1.toDouble();
	//calib_const.p2 = p2.toDouble();

	QDomElement xml_rotation = xml_cam->firstChildElement(TsaiCameraCalibration::XML_TSAICALIB_ROTATION);
	QString rx = xml_rotation.attribute(TsaiCameraCalibration::XML_TSAICALIB_RX);
	QString ry = xml_rotation.attribute(TsaiCameraCalibration::XML_TSAICALIB_RY);
	QString rz = xml_rotation.attribute(TsaiCameraCalibration::XML_TSAICALIB_RZ);

	calib_const.Rx = rx.toDouble();
	calib_const.Ry = ry.toDouble();
	calib_const.Rz = rz.toDouble();

	QDomElement xml_translation = xml_cam->firstChildElement(TsaiCameraCalibration::XML_TSAICALIB_TRANSLATION);
	QString tx = xml_translation.attribute(TsaiCameraCalibration::XML_TSAICALIB_TX);
	QString ty = xml_translation.attribute(TsaiCameraCalibration::XML_TSAICALIB_TY);
	QString tz = xml_translation.attribute(TsaiCameraCalibration::XML_TSAICALIB_TZ);

	calib_const.Tx = tx.toDouble();
	calib_const.Ty = ty.toDouble();
	calib_const.Tz = tz.toDouble();

	QDomElement xml_matrix = xml_cam->firstChildElement(TsaiCameraCalibration::XML_TSAICALIB_MATRIX);
	QString r1 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R1);
	QString r2 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R2);
	QString r3 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R3);
	QString r4 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R4);
	QString r5 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R5);
	QString r6 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R6);
	QString r7 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R7);
	QString r8 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R8);
	QString r9 = xml_matrix.attribute(TsaiCameraCalibration::XML_TSAICALIB_R9);

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
	QDomElement xml_calib = doc->createElement(CameraCalibration::XML_CALIBRATION);
	xml_calib.setAttribute(CameraCalibration::XML_TYPE,TsaiCameraCalibration::XML_TSAICALIB_TYPE);
	if(optimized){
		xml_calib.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_OPTIMIZED,"1");
	}else{
		xml_calib.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_OPTIMIZED,"0");
	}
	QDomElement xml_sensor = doc->createElement(TsaiCameraCalibration::XML_TSAICALIB_SENSOR);
	xml_sensor.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_DPX,QString::number(cam_para.dpx));
	xml_sensor.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_DPY,QString::number(cam_para.dpy));
	xml_sensor.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_CX,QString::number(cam_para.Cx));
	xml_sensor.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_CY,QString::number(cam_para.Cy));
	xml_sensor.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_SX,QString::number(cam_para.sx));
	xml_calib.appendChild(xml_sensor);

	QDomElement xml_cc = doc->createElement(TsaiCameraCalibration::XML_TSAICALIB_CC);
	xml_cc.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_F,QString::number(calib_const.f));
	xml_cc.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_KAPPA1,QString::number(calib_const.kappa1));
	xml_cc.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_P1,QString::number(calib_const.p1));
	xml_cc.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_P2,QString::number(calib_const.p2));
	xml_calib.appendChild(xml_cc);

	QDomElement xml_translation = doc->createElement(TsaiCameraCalibration::XML_TSAICALIB_TRANSLATION);
	xml_translation.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_TX,QString::number(calib_const.Tx));
	xml_translation.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_TY,QString::number(calib_const.Ty));
	xml_translation.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_TZ,QString::number(calib_const.Tz));
	xml_calib.appendChild(xml_translation);

	QDomElement xml_rotation = doc->createElement(TsaiCameraCalibration::XML_TSAICALIB_ROTATION);
	xml_rotation.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_RX,QString::number(calib_const.Rx));
	xml_rotation.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_RY,QString::number(calib_const.Ry));
	xml_rotation.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_RZ,QString::number(calib_const.Rz));
	xml_calib.appendChild(xml_rotation);

	QDomElement xml_matrix = doc->createElement(TsaiCameraCalibration::XML_TSAICALIB_MATRIX);
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R1,QString::number(calib_const.r1));
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R2,QString::number(calib_const.r2));
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R3,QString::number(calib_const.r3));
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R4,QString::number(calib_const.r4));
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R5,QString::number(calib_const.r5));
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R6,QString::number(calib_const.r6));
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R7,QString::number(calib_const.r7));
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R8,QString::number(calib_const.r8));
	xml_matrix.setAttribute(TsaiCameraCalibration::XML_TSAICALIB_R9,QString::number(calib_const.r9));

	xml_calib.appendChild(xml_matrix);
	root->appendChild(xml_calib);

}

void TsaiCameraCalibration::calibrate(QList<CameraCalibrationData*> &ccd ){
	calibrate(ccd, true);
}

void TsaiCameraCalibration::calibrate(QList<CameraCalibrationData*> &ccd, bool optimize){

	optimized = optimize;
	
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

	//
	if(optimize){
		noncoplanar_calibration_with_full_optimization ();
	}else{
		noncoplanar_calibration ();
	}
	//print_cp_cc_data (stderr, &cp, &cc);
	print_error_stats (stderr);
	//dump_cp_cc_data (stdout, &cp, &cc);
	cam_para.Cx = cp.Cx;
	cam_para.Cy = cp.Cy;
	cam_para.Ncx = cp.Ncx;
	cam_para.Nfx = cp.Nfx;
	cam_para.dpx = cp.dpx;
	cam_para.dpy = cp.dpy;
	cam_para.dx = cp.dx;
	cam_para.dy = cp.dy;
	cam_para.sx = cp.sx;

	calib_const.Rx = cc.Rx;
	calib_const.Ry = cc.Ry;
	calib_const.Rz = cc.Rz;
	calib_const.Tx = cc.Tx;
	calib_const.Ty = cc.Ty;
	calib_const.Tz = cc.Tz;
	calib_const.f = cc.f;
	calib_const.kappa1 = cc.kappa1;
	calib_const.p1 = cc.p1;
	calib_const.p2 = cc.p2;
	calib_const.r1 = cc.r1;
	calib_const.r2 = cc.r2;
	calib_const.r3 = cc.r3;
	calib_const.r4 = cc.r4;
	calib_const.r5 = cc.r5;
	calib_const.r6 = cc.r6;
	calib_const.r7 = cc.r7;
	calib_const.r8 = cc.r8;
	calib_const.r9 = cc.r9;



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

CameraCalibration* TsaiCameraCalibration::calibrateToTsai(MeshModel *mm, bool optimize){
	QList<CameraCalibrationData*> ccd_list;

	unsigned int i;
	for (i=0;i<mm->cm.vert.size();i++){
		CameraCalibrationData* ccd = new CameraCalibrationData();

		double u,v;

		getUVforPoint(mm->cm.vert[i].P()[0],mm->cm.vert[i].P()[1],mm->cm.vert[i].P()[0],&u,&v);
		
		ccd->wX = mm->cm.vert[i].P()[0];
		ccd->wY = mm->cm.vert[i].P()[1];
		ccd->wZ = mm->cm.vert[i].P()[2];
		ccd->iX = u;
		ccd->iY = v;
		if (((int)u >=0 && (int)u<= resolution[0])&& ((int)v >=0 && (int)v<= resolution[1]))
		ccd_list.push_back(ccd);
	}

	TsaiCameraCalibration *tsai = new TsaiCameraCalibration();
	tsai->calibrate(ccd_list,optimize);
	return tsai;
}


