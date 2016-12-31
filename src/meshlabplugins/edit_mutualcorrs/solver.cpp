#include <QFile>
#include <QTextStream>
#include "edit_mutualcorrs.h"
#include <vcg/math/shot.h>
#include "../../external/newuoa/include/newuoa.h"


#include "solver.h"
#include "mutual.h"

//#include <wrap/tsai/tsaimethods.h>
#include "levmarmethods.h"

using namespace std;
using namespace vcg;

Solver::Solver():
  align(NULL), mutual(NULL), optimize_focal(true),
  variance(2), tolerance(0.01), maxiter(600), mIweight(1) {

  start = 1e20; end = 0;
  f_evals = 0;
}

//function that evaluate error
// it is a weighted function. 
// mIweight=1 consider only the MI evaluetion function; 
// mIweight=0 consider only the error function through the correspondences;
double Solver::operator()(int ndim, double *x) {
  f_evals++;

  //For test
  f_evals_total++;
  
  for(int i = 0; i < ndim; i++) {
    p[i] = x[i];
    //cout << p[i] << "\t";
  }
  //cout << endl;
  static int iter = 0;
  iter++;
/*  double orig = p.scale[6];
  //p.scale[6] *= pow(iter/(double)maxiter, 4);
  double v = 4*(iter/(double)maxiter) - 2;
  p.scale[6] *= v / sqrt(1 + v*v) + 1;
  double forig = p.reference.Intrinsics.FocalMm;
  p.reference.Intrinsics.FocalMm += p[6]*(orig - p.scale[6]);
  cout << "v: " << v <<  "  Scale: " << p.scale[6] << endl;*/
  
  //cout << iter << " " << v / sqrt(1 + v*v) << endl;

  Shot shot = p.toShot();
  //p.scale[6] = orig;
  //p.reference.Intrinsics.FocalMm = forig;

  align->shot = shot;
  //align->renderScene(shot);

  int w = align->width();
  int h = align->height();
 
  double info = 0;

#define TEST
#ifndef TEST
  switch(align->mode) {

   case AlignSet::NORMALMAP:
   case AlignSet::COMBINE:
   case AlignSet::SPECULAR:
    align->readRender(1);
    info += 2 - mutual->info(w, h, align->target, align->render);
   case AlignSet::COLOR:
   case AlignSet::SILHOUETTE: {
    align->readRender(0);
    info += 2 - mutual->info(w, h, align->target, align->render);
   }
  }
#else
   int wstep = w;//w/4;
   int hstep = h;//h/4;

   if (mIweight!=0){
   switch(align->mode) {

   case AlignSet::NORMALMAP:
   case AlignSet::COMBINE:
   case AlignSet::SPECULAR:
   case AlignSet::SPECAMB:
     align->renderScene(shot,1);
     for(int i = 0; i < w; i+= wstep) {
       int iend = i + wstep;
       if(iend > w) iend = w;
       for(int j =0; j < h; j+= hstep) {
         int jend = j+hstep;
         if(jend > h) jend = h;

         double m = 2 - mutual->info(w, h, align->target, align->render, i, iend, j, jend);
         info += m;
       }
     }
   case AlignSet::COLOR:
   case AlignSet::SILHOUETTE: {
    align->renderScene(shot,0);
    for(int i = 0; i < w; i+= wstep) {
       int iend = i + wstep;
       if(iend > w) iend = w;
       for(int j =0; j < h; j+= hstep) {
         int jend = j+hstep;
         if(jend > h) jend = h;
         double m = 2 - mutual->info(w, h, align->target, align->render, i, iend, j, jend);
         info += m;
       }
     }
   }
  }
  }
#endif

  if(start == 0) start = info;
  if(start == 1e20)
    start = info;
  end = info;
  //return info;

  double k = mIweight;
  double error =0;
  if(align->correspList.size()>0)
    error = calculateError2(shot);
  align->error = error;

  double result = k*info + (1-k)*error;

  /*myfile << "error " << error <<"\n";
  myfile << "MI " << info <<"\n";
  myfile << "result " << result <<"\n \n";*/

  myfile << f_evals_total <<" ";
  myfile << (1-k)*error <<" ";
  myfile << k*info <<" ";
  myfile << result <<" ";
  myfile << endl;

  return result;
}

int Solver::optimize(AlignSet *_align, MutualInfo *_mutual, Shot &shot) {
  align = _align;
  mutual = _mutual;

  p = Parameters(optimize_focal, shot, align->width(), align->height(),
                 align->box, *align->mesh);

  f_evals = 0;
  start = 1e20;
  end = 0;
  //TODO optimize fbo binding here
  end = min_newuoa<double, Solver>(p.size(), p.p, *this, 
                                   variance, tolerance, maxiter);

  align->shot = p.toShot();
  return f_evals;
}

//For Ponchio's Levmar
void Solver::value(double *_p, double *x, int m, int n, void *data) {
  Solver &solver = *(Solver *)data;
  solver.f_evals++;
  Parameters &p = solver.p;
  AlignSet *align = solver.align;
  MutualInfo *mutual = solver.mutual;

  for(int i = 0; i < m; i++) {
    //cout << _p[i] << " ";
    p[i] = _p[i];
  }
  //cout << endl;

  Shot shot = p.toShot();

  align->shot = shot;
  //align->renderScene(shot);

  int w = align->width();
  int h = align->height();

  for(int i = 0; i < n; i++) x[i] = 0;
  int blocks = (int)sqrt((double)n);

  int wstep = w/blocks+1;
  int hstep = h/blocks+1;

  switch(align->mode) {

   case AlignSet::NORMALMAP:
   case AlignSet::COMBINE:
   case AlignSet::SPECULAR:
   case AlignSet::SPECAMB:
     align->renderScene(shot,1);
     for(int i = 0; i < w; i+= wstep) {
       int iend = i + wstep;
       if(iend > w) iend = w;
       for(int j =0; j < h; j+= hstep) {
         int jend = j+hstep;
         if(jend > h) jend = h;
         double info = 2 - mutual->info(w, h, align->target, align->render, i, iend, j, jend);
         x[j + 3*i] = info;
         //cout << "info: " << info << endl;
       }
     }
   case AlignSet::COLOR:
   case AlignSet::SILHOUETTE: {
    align->renderScene(shot,0);
    for(int i = 0; i < w; i+= wstep) {
       int iend = i + wstep;
       if(iend > w) iend = w;
       for(int j =0; j < h; j+= hstep) {
         int jend = j+hstep;
         if(jend > h) jend = h;
         double info = 2 - mutual->info(w, h, align->target, align->render, i, iend, j, jend);
         x[j + 3*i] += info;
       }
     }
   }
  }
  double totinfo = 0;
  for(int i = 0; i < n; i++)
     totinfo += x[i];

  double &start = solver.start;
  double &end = solver.end;
  if(start == 0) start = totinfo;
  if(start == 1e20)  start = totinfo;
  end = totinfo;
  
}

int Solver::levmar(AlignSet *_align, MutualInfo *_mutual, Shot &shot) {
  align = _align;
  mutual = _mutual;

  p = Parameters(optimize_focal, shot, align->width(), align->height(),
                 align->box, *align->mesh);

  f_evals = 0;
  start = 1e20;
  end = 0;

  double *_p = new double[p.size()];
  for(int i = 0; i < p.size(); i++) _p[i] = p[i];
  int sides = 16;
  double *x = new double[sides];
  for(int i = 0; i < sides; i++) x[i] = 0;

  opts[0] = 1;
  opts[1] = 1e-8;
  opts[2] = 1e-8;
  opts[3] = 1e-8;
  opts[4] = 1;

 //double opts[5]; //0 -> initial mu              
                  //1 -> minimum JTe             
                  //2 -> minimum Dp              
                  //3 -> minimum sum(d*d)        
                  //4 -> delta for finite differe

  //int niter=0;
  int niter = dlevmar_dif(Solver::value, _p, x, p.size(), sides,
                          maxiter, opts, info,
                          NULL, NULL, this); 

  //std::cout << "DEBUG: terminated because ";
  switch((int)info[6]) {
    case 1: std::cout << "stopped by small gradient J^T e\n"; break;
    case 2: std::cout << "stopped by small Dp\n"; break;
    case 3: std::cout << "stopped by itmax\n"; break;
    case 4: std::cout << "singular matrix. Restart from current p with increased mu \n"; break;
    case 5: std::cout << "no further error reduction is possible. Restart with increased mu\n"; break;
    case 6: std::cout << "stopped by small ||e||_2 \n"; break;
  }
  //std::cout << "iterations: " << info[5] << " distseval: " << info[7] << " jaceval: " << info[8] << std::endl;

  for(int i = 0; i < p.size(); i++) p[i] = _p[i];
  align->shot = p.toShot();

  delete[] x;
  delete[] _p;


  return niter;
}


//bool Solver::tsai(AlignSet *_align, Shot &shot){
//	align = _align;
//	QList<PointCorrespondence*> *correspList = align->correspList;
//
//    std::list<TsaiCorrelation> *corrs = new std::list<TsaiCorrelation>();
//    for( int i=0; i<correspList->count(); i++){
//        PointCorrespondence *corr = correspList->at(i);
//        PointOnLayer currentPointOnLayer1= corr->getPointAt(0);
//        PointOnLayer currentPointOnLayer2= corr->getPointAt(1);
//        PointType type1 = currentPointOnLayer1.getType();
//        TsaiCorrelation *corrTsai = new TsaiCorrelation();
//		if(type1==_3D_POINT){
//			vcg::Point3d currentPoint3d(currentPointOnLayer1.pX, currentPointOnLayer1.pY,currentPointOnLayer1.pZ );
//			corrTsai->point3d = currentPoint3d;
//			vcg::Point2d currentPoint2d(currentPointOnLayer2.pX, currentPointOnLayer2.pY);
//			corrTsai->point2d = currentPoint2d;
//		}
//		else{
//			vcg::Point3d currentPoint3d(currentPointOnLayer2.pX, currentPointOnLayer2.pY,currentPointOnLayer2.pZ );
//			corrTsai->point3d = currentPoint3d;
//			vcg::Point2d currentPoint2d(currentPointOnLayer1.pX, currentPointOnLayer1.pY);
//			corrTsai->point2d = currentPoint2d;
//		}
//        qDebug("Point3d %f %f %f",(float)corrTsai->point3d.X(),(float)corrTsai->point3d.Y(),(float)(float)corrTsai->point3d.Z());
//        qDebug("Point2d %f %f %f",(float)corrTsai->point2d.X(),(float)corrTsai->point2d.Y());
//
//        corrs->push_back(*corrTsai);
//    }
//
//     
//    vcg::Camera<float> &cam = shot.Intrinsics;
//
//	//DEBUG
//    qDebug("\n TEST BEFORE CALIBRATION \n");
//    qDebug("Focal %f",cam.FocalMm);
//    qDebug("ViewportPx.X %i",cam.ViewportPx.X());
//    qDebug("ViewportPx.Y %i",cam.ViewportPx.Y());
//    qDebug("CenterPx.X %f",cam.CenterPx[0]);
//    qDebug("CenterPx.Y %f",cam.CenterPx[1]);
//    qDebug("DistorntedCenterPx.X %f",cam.DistorCenterPx[0]);
//    qDebug("DistorntedCenterPx.Y %f",cam.DistorCenterPx[1]);
//    qDebug("PixelSizeMm.X %f",cam.PixelSizeMm[0]);
//    qDebug("PixelSizeMm.Y %f",cam.PixelSizeMm[1]);
//    qDebug("k1 %f",cam.k[0]);
//    qDebug("k2 %f",cam.k[1]);
//    qDebug("Tra %f %f %f",shot.Extrinsics.Tra().X(),shot.Extrinsics.Tra().Y(),shot.Extrinsics.Tra().Z());
//    for(int i=0;i<4;i++)
//         qDebug("Rot %f %f %f %f", (shot.Extrinsics.Rot())[i][0],(shot.Extrinsics.Rot())[i][1],(shot.Extrinsics.Rot())[i][2],(shot.Extrinsics.Rot())[i][3] );
//
//    //bool result = TsaiMethods::calibrate(&shot,corrs,optimize_focal);
//	bool result= false; //OKKIO!!!
//    qDebug("End calibration");
//
//	align->error = calculateError(corrs,shot);
//
//   //DA SPEIGARE PERCHE'
//   //cam.CenterPx[0] = cam.CenterPx[0] + (cam.CenterPx[0] - cam.DistorCenterPx[0]);
//   //cam.CenterPx[1] = cam.CenterPx[1] + (cam.CenterPx[1] - cam.DistorCenterPx[1]);
//
//   //TEMPORANEO
//    //resetView();
//
////    //PAOLO BRIVIO
////    Shot idShot;
////        idShot.Intrinsics = shot.Intrinsics;
////    idShot.Extrinsics.SetTra(Point3d(0,0,1000));
////        //idShot = shot;
////    Shot2Track<Shot::ScalarType>(shot,idShot,track);
////    //shot.Extrinsics.SetIdentity();
////        shot = idShot;
//   
////     //PAOLO BRIVIO
////     Shot idShot;
////     idShot.Intrinsics = shot.Intrinsics;
////
////     Shot2Track<Shot::ScalarType>(shot,previousShot,track);
////      shot = idShot;
//
//
//   //DEBUG
//        //vcg::Camera<double> &cam = shot.Intrinsics;
//    qDebug("\n TEST AFTER CALIBRATION \n");
//    qDebug("Focal %f",cam.FocalMm);
//    qDebug("ViewportPx.X %i",cam.ViewportPx.X());
//    qDebug("ViewportPx.Y %i",cam.ViewportPx.Y());
//    qDebug("CenterPx.X %f",cam.CenterPx[0]);
//    qDebug("CenterPx.Y %f",cam.CenterPx[1]);
//    qDebug("DistortedCenterPx.X %f",cam.DistorCenterPx[0]);
//    qDebug("DistortedCenterPx.Y %f",cam.DistorCenterPx[1]);
//    qDebug("PixelSizeMm.X %f",cam.PixelSizeMm[0]);
//    qDebug("PixelSizeMm.Y %f",cam.PixelSizeMm[1]);
//    qDebug("k1 %f",cam.k[0]);
//    qDebug("k2 %f",cam.k[1]);
//    qDebug("Tra %f %f %f",shot.Extrinsics.Tra().X(),shot.Extrinsics.Tra().Y(),shot.Extrinsics.Tra().Z());
//    for(int i=0;i<4;i++)
//         qDebug("Rot %f %f %f %f", (shot.Extrinsics.Rot())[i][0],(shot.Extrinsics.Rot())[i][1],(shot.Extrinsics.Rot())[i][2],(shot.Extrinsics.Rot())[i][3] );
//	
//	align->shot=shot;
//	return result;
//}

//template<class Correlation>
//double Solver::calculateError(std::list<Correlation> *corrs, Shot &shot){
//        typename std::list<Correlation>::iterator it_c;
//
//    //shot.Intrinsics.ViewportPx.X()/(double) shot.Intrinsics.ViewportPx.Y();
//
//    int count=0;
//
//    double error = 0;
//// OKKIO!!
// //   for ( it_c= corrs->begin() ; it_c !=corrs->end(); it_c++ ){
//    //	c=&*it_c;
//    //	p1=&(c->point3d);
//    //	p2=&(c->point2d);
//
//    //	if(p1!=NULL && p2!=NULL)
//    //	{
//    //		//Adjust 2D point
//    //		vcg::Point2d p2adj(((p2->X()/ratio) +1)/2.0 * shot.Intrinsics.CenterPx.X()*2.0,((p2->Y())+1)/2.0 * shot.Intrinsics.CenterPx.Y()*2.0);
//    //		//Project 3D point
//    //		vcg::Point2f p1proj = shot.Project(*p1);
//    //		//Calculate disntance
//    //		float dist = vcg::Distance<float>(p1proj,p2adj);
//    //		error += dist;
//    //		count++;
//    //	}
//    //}//all corrs
//
//    ////Normalize error
//    return error /= count;
//}

double Solver::calculateError2( Shot &shot){
    //E' una pezza, andrebbe meglio pensato. Va a beccare direttamente le strutture dati PointCorrespondence di base.
    //align già è sicuramente settato perchè lo chiami da optimize (poi dovrai distinguere le due cose, p.e. fare un optimize2)
    std::vector<Correspondence> correspList = align->correspList;
    double error = 0;
   
    for( int i=0; i<correspList.size(); i++){
        Correspondence corr = correspList[i];
        vcg::Point2f projected = shot.Project(corr.Point3D);
        
        float dist = vcg::Distance<float>(projected,corr.Point2D);
        error += dist;
		align->correspList[i].error = dist;
    }

    //Normalize error
	return error /= correspList.size();

}

bool Solver::levmar(AlignSet *_align, Shot &shot){
	//return true;
    align = _align;
   // QList<PointCorrespondence*> *correspList = align->correspList;

    std::list<LevmarCorrelation> *corrs = new std::list<LevmarCorrelation>();
	for (int i = 0; i<align->correspList.size(); i++){
        //PointCorrespondence *corr = correspList->at(i);
        //PointOnLayer currentPointOnLayer1= corr->getPointAt(0);
        //PointOnLayer currentPointOnLayer2= corr->getPointAt(1);
        //PointType type1 = currentPointOnLayer1.getType();
        LevmarCorrelation *corrLevmar = new LevmarCorrelation();
		corrLevmar->point3d = align->correspList[i].Point3D;
		corrLevmar->point2d = vcg::Point2d(align->correspList[i].Point2D.X(), align->correspList[i].Point2D.Y());

        qDebug("Point3d %f %f %f",(float)corrLevmar->point3d.X(),(float)corrLevmar->point3d.Y(),(float)(float)corrLevmar->point3d.Z());
        qDebug("Point2d %f %f %f",(float)corrLevmar->point2d.X(),(float)corrLevmar->point2d.Y());

        corrs->push_back(*corrLevmar);
    }

	vcg::Camera<float> &cam = shot.Intrinsics;

	//DEBUG
	qDebug("\n TEST BEFORE CALIBRATION \n");
	qDebug("Focal %f", cam.FocalMm);
	qDebug("ViewportPx.X %i", cam.ViewportPx.X());
	qDebug("ViewportPx.Y %i", cam.ViewportPx.Y());
	qDebug("CenterPx.X %f", cam.CenterPx[0]);
	qDebug("CenterPx.Y %f", cam.CenterPx[1]);
	qDebug("DistorntedCenterPx.X %f", cam.DistorCenterPx[0]);
	qDebug("DistorntedCenterPx.Y %f", cam.DistorCenterPx[1]);
	qDebug("PixelSizeMm.X %f", cam.PixelSizeMm[0]);
	qDebug("PixelSizeMm.Y %f", cam.PixelSizeMm[1]);
	qDebug("k1 %f", cam.k[0]);
	qDebug("k2 %f", cam.k[1]);
	qDebug("Tra %f %f %f", shot.Extrinsics.Tra().X(), shot.Extrinsics.Tra().Y(), shot.Extrinsics.Tra().Z());
	for (int i = 0; i<4; i++)
		qDebug("Rot %f %f %f %f", (shot.Extrinsics.Rot())[i][0], (shot.Extrinsics.Rot())[i][1], (shot.Extrinsics.Rot())[i][2], (shot.Extrinsics.Rot())[i][3]);

	Shot previousShot = shot;
	//First calculate only extrinsics
	bool result = LevmarMethods::calibrate(&shot, corrs, false);
	//If user wants calibrate the focal (these have to be two different steps)
	if (optimize_focal)
		result = LevmarMethods::calibrate(&shot, corrs, optimize_focal);

	qDebug("End calibration");

	align->error = calculateError2(shot);

	//DA SPEIGARE PERCHE'
	//cam.CenterPx[0] = cam.CenterPx[0] + (cam.CenterPx[0] - cam.DistorCenterPx[0]);
	//cam.CenterPx[1] = cam.CenterPx[1] + (cam.CenterPx[1] - cam.DistorCenterPx[1]);

	//    //PAOLO BRIVIO
	//    Shot idShot;
	//        idShot.Intrinsics = shot.Intrinsics;
	//    idShot.Extrinsics.SetTra(Point3d(0,0,1000));
	//        //idShot = shot;
	//    Shot2Track<Shot::ScalarType>(shot,idShot,track);
	//    //shot.Extrinsics.SetIdentity();
	//        shot = idShot;

	//     //PAOLO BRIVIO
	//     Shot idShot;
	//     idShot.Intrinsics = shot.Intrinsics;
	//
	//     Shot2Track<Shot::ScalarType>(shot,previousShot,track);
	//      shot = idShot;


	//DEBUG
	//vcg::Camera<double> &cam = shot.Intrinsics;
	qDebug("\n TEST AFTER CALIBRATION \n");
	qDebug("Focal %f", cam.FocalMm);
	qDebug("ViewportPx.X %i", cam.ViewportPx.X());
	qDebug("ViewportPx.Y %i", cam.ViewportPx.Y());
	qDebug("CenterPx.X %f", cam.CenterPx[0]);
	qDebug("CenterPx.Y %f", cam.CenterPx[1]);
	qDebug("DistortedCenterPx.X %f", cam.DistorCenterPx[0]);
	qDebug("DistortedCenterPx.Y %f", cam.DistorCenterPx[1]);
	qDebug("PixelSizeMm.X %f", cam.PixelSizeMm[0]);
	qDebug("PixelSizeMm.Y %f", cam.PixelSizeMm[1]);
	qDebug("k1 %f", cam.k[0]);
	qDebug("k2 %f", cam.k[1]);
	qDebug("Tra %f %f %f", shot.Extrinsics.Tra().X(), shot.Extrinsics.Tra().Y(), shot.Extrinsics.Tra().Z());
	for (int i = 0; i<4; i++)
		qDebug("Rot %f %f %f %f", (shot.Extrinsics.Rot())[i][0], (shot.Extrinsics.Rot())[i][1], (shot.Extrinsics.Rot())[i][2], (shot.Extrinsics.Rot())[i][3]);

	align->shot = shot;
	return result;
}

