/****************************************************************************
LEVMAR METHODS
	Interface class between the levmar.lib and the project structure.
	
*****************************************************************************/

#include "levmarmethods.h" //base header

#include <vcg/math/camera.h>

/**********************************************************************
Calibrate extrinsics (if p_foc is false) or focal (if p_foc is true)
**********************************************************************/
bool LevmarMethods::calibrate( vcg::Shot<float>* shot,std::list<LevmarCorrelation>* corr, bool p_foc)
{
	bool my_ret_val=false;

	//if(corr->size() >= MIN_POINTS_FOR_CALIBRATE){

	double p[7];

	Shot2Levmar(shot,p,p_foc);

	LevmarData* data = new LevmarData();
	double* x = new double[corr->size()*2]; 
	double opts[LM_OPTS_SZ];
	double info[LM_INFO_SZ];

    if(createDataSet(corr,shot,data,x,opts,info))
	{
		int n= corr->size()*2;
		if(!p_foc){
			int m=6;

			//my_ret_val = dlevmar_dif(estimateExtr, p,x,m,n,1000,opts,info,NULL,NULL,data); OKKIO
		}
		else{
			int m=1;

			//my_ret_val = dlevmar_dif(estimateFocal, p,x,m,n,100000,opts,info,NULL,NULL,data); OKKIO
		}

		Levmar2Shot(shot,p,p_foc);
		vcg::Matrix44f rot = shot->Extrinsics.Rot();
	}

	delete data;
	delete[] x;

	return my_ret_val;
}


/*********************************************
CREATE DATA SET
Modify data,x, opts and info(?)
*********************************************/
//TOGLIERE SHOT DAI PARAMETRI!
bool LevmarMethods::createDataSet(std::list<LevmarCorrelation>* corr,vcg::Shot<float>* s, LevmarData* data, double* x,double opts[LM_OPTS_SZ],double info[LM_INFO_SZ])
{	
	bool my_ret_val=false;
	// OKKIO!!!
//	
//	//lascia lo spazio dei punti come per Tsai
//	vcg::Point3f *p1;
//	vcg::Point2d *p2;
//    int count=0;
//
//	data->points3d = new vcg::Point3d*[corr->size()];
//	data->levmarCam = s;
//
//
//    std::list<LevmarCorrelation>::iterator it_c;
//	LevmarCorrelation* c;
//
//    double ratio = s->Intrinsics.ViewportPx.X()/(double) s->Intrinsics.ViewportPx.Y();
//
//    for ( it_c= corr->begin() ; it_c !=corr->end(); it_c++ ){
//		c=&*it_c;
//		p1=&(c->point3d);
//		p2=&(c->point2d);
//
//		if(p1!=NULL && p2!=NULL)
//		{
//			(data->points3d)[count] = p1;
////                        cd.Xf[count] = (p2->X()+1)/2.0 * cp.Cx*2.0;
////                        cd.Yf[count] = ((-p2->Y())+1)/2.0 * cp.Cy*2.0;
//
//			x[count*2] = ((p2->X()/ratio) +1)/2.0 * s->Intrinsics.CenterPx.X()*2.0;
//			x[count*2+1] = ((p2->Y())+1)/2.0 * s->Intrinsics.CenterPx.Y()*2.0;
//			count++;
//		}
//		//if(count>=MAX_POINTS) break;
//
//	}//all corrs
//
//    assert(count==corr->size());
//
//	opts[0] = 1E-6;
//	opts[1] = 1E-15;
//	opts[2] = 1E-15;
//	opts[3] = 1E-15;
//	opts[4]= LM_DIFF_DELTA;
//
//    //qDebug("all points: %i",count);
//    if(count>0 ) my_ret_val = true;
//
    return my_ret_val;
}


/*********************************************
SHOT 2 TSAI
Transformation of the camera data between levmar structure and vcg structure
*********************************************/
void LevmarMethods::Shot2Levmar(vcg::Shot<float>* shot, double* p, bool p_foc){

	if(!p_foc){
		float alpha, beta, gamma;
		vcg::Matrix44f rot = shot->Extrinsics.Rot();
		rot.ToEulerAngles(alpha, beta, gamma);

		vcg::Point3f tVect = shot->Extrinsics.Tra();

		p[0] = alpha;
		p[1] = beta;
		p[2] = gamma;
		p[3] = tVect[0];
		p[4] = tVect[1];
		p[5] = tVect[2];
	}
	else
		p[0]= shot->Intrinsics.FocalMm;

	//Cam2Levmar(shot);
}

/*********************************************
TSAI 2 SHOT
Transformation of the camera data between levmar structure and vcg structure
*********************************************/
void LevmarMethods::Levmar2Shot(vcg::Shot<float>* shot,  double *p,bool p_foc) {
	
	//if(p_foc)
	//	shot->Intrinsics.FocalMm =	cc.f;//*cp.sx;// *SCALE_FACTOR;
	///* old ones
	//shot->Intrinsics.DistorCenterPx[0] = cc.p1;
	//shot->Intrinsics.DistorCenterPx[1] = cc.p2;
	//
	//shot->Intrinsics.DistorCenterPx[0] = shot->Intrinsics.CenterPx.X()+(cc.p1/shot->Intrinsics.PixelSizeMm.X());
	//shot->Intrinsics.DistorCenterPx[1] = shot->Intrinsics.CenterPx.Y()+(cc.p2/shot->Intrinsics.PixelSizeMm.Y());
	//*/
	//shot->Intrinsics.DistorCenterPx[0] = cp.Cx;
	//shot->Intrinsics.DistorCenterPx[1] = cp.Cy;

	//shot->Intrinsics.k[0]=cc.kappa1;
	if(!p_foc){

		//* ROTATION */
		vcg::Matrix44<vcg::Shot<float>::ScalarType> mat;
		mat.SetIdentity();
		mat.FromEulerAngles(p[0],p[1],p[2]);

		shot->Extrinsics.SetRot(mat);

		//* TRANSLATION */
		//vcg::Point3d tl = shot->Extrinsics.Tra();
		//
		//tl = vcg::Inverse(shot->Extrinsics.Rot())* vcg::Point3d(-cc.Tx,cc.Ty,cc.Tz);

		shot->Extrinsics.SetTra(vcg::Point3f(p[3], p[4], p[5]));
	}
	else
		shot->Intrinsics.FocalMm=p[0];
}

void LevmarMethods::Cam2Levmar(vcg::Shot<float> *s){
	
	//cp.Ncx = s->Intrinsics.ViewportPx.X();	// [sel]     Number of sensor elements in camera's x direction //
	//cp.Nfx = s->Intrinsics.ViewportPx.X();	// [pix]     Number of pixels in frame grabber's x direction   //
	//cp.dx  = s->Intrinsics.PixelSizeMm.X();//*SCALE_FACTOR;	// [mm/sel]  X dimension of camera's sensor element (in mm)    //
	//cp.dy  = s->Intrinsics.PixelSizeMm.Y();//*SCALE_FACTOR;	// [mm/sel]  Y dimension of camera's sensor element (in mm)    //
	//
	//cp.dpx = cp.dx * cp.Nfx/cp.Ncx;	// [mm/pix]  Effective X dimension of pixel in frame grabber   //
	//cp.dpy = cp.dy;	// [mm/pix]  Effective Y dimension of pixel in frame grabber   //

	//cp.Cx  = s->Intrinsics.CenterPx.X();	// [pix]     Z axis intercept of camera coordinate system      //
	//cp.Cy  = s->Intrinsics.CenterPx.Y();	// [pix]     Z axis intercept of camera coordinate system      //
	//
	//cp.sx  = 1.0;	// []        Scale factor to compensate for any error in dpx   //

	//cc.f = s->Intrinsics.FocalMm;// *SCALE_FACTOR;
	//cc.kappa1 = s->Intrinsics.k[0];
}

//Estimate only extrinsics.
void LevmarMethods::estimateExtr(double *p, double *x, int m, int n, void *data)
{
	vcg::Point3f** ptr = ((LevmarData*) data)->points3d;
	vcg::Shot<float>* levmarCam = ((LevmarData*) data)->levmarCam;

	vcg::Matrix44f matrix;
#ifdef USE_QUATERNION
	vcg::Quaterniond quaternion(p[0], p[1], p[2], p[3]);
	quaternion.ToMatrix(matrix);
	levmarCam.Extrinsics.SetRot(matrix);
	levmarCam.Extrinsics.SetTra(vcg::Point3d(p[4], p[5], p[6]));
#else
	matrix.SetIdentity();
	matrix.FromEulerAngles(p[0], p[1], p[2]);
	levmarCam->Extrinsics.SetRot(matrix);
	levmarCam->Extrinsics.SetTra(vcg::Point3f(p[3], p[4], p[5]));
#endif
	
	for (int i = 0; i < n/2; i++)
	{
		vcg::Point2f point2d = levmarCam->Project(*ptr[i]);
		x[i*2] = point2d.X();
		x[i*2 + 1] = point2d.Y();
	}
}

//Estimate only the focal.
void LevmarMethods::estimateFocal(double *p, double *x, int m, int n, void *data)
{
	vcg::Point3f** ptr = ((LevmarData*) data)->points3d;
	vcg::Shot<float>* levmarCam = ((LevmarData*) data)->levmarCam;

	levmarCam->Intrinsics.FocalMm = p[0];

	for (int i = 0; i < n/2; i++)
	{
		vcg::Point2f point2d = levmarCam->Project(*ptr[i]);
		x[i*2] = point2d.X();
		x[i*2 + 1] = point2d.Y();
	}


}