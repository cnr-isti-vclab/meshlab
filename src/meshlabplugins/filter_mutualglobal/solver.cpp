#include <QFile>
#include <QTextStream>
#include "filter_mutualglobal.h"
#include <vcg/math/shot.h>
#include "../../external/newuoa/include/newuoa.h"


#include "solver.h"
#include "mutual.h"

#include "pointCorrespondence.h"
//#include <wrap/tsai/tsaimethods.h>
#include "levmarmethods.h"

using namespace std;
using namespace vcg;

Solver::Solver():
  align(NULL), mutual(NULL), optimize_focal(true),
  variance(4), tolerance(0.01), maxiter(200), mIweight(1) {

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
	 break;
	
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
	break;
   }
   case AlignSet::NODE: {
		assert(glGetError() == 0);
		//QImage comb; std::vector<QImage> projimg;
		/*align->mode=AlignSet::COMBINE;
		align->renderScene(shot,1,true);
		assert(glGetError() == 0);
		comb=align->rend;*/
		align->mode=AlignSet::PROJMULTIIMG;
		
		//for (int n=0; n<align->arcShots.size(); n++)
		{
			/*align->imagePro=align->arcImages[0];
			align->shotPro=*align->arcShots[0];*/
			//align->ProjectedMultiImageChanged();
			align->RenderMultiShadowMap();
			align->renderScene(shot,2,true);
			//projimg.push_back(align->rend);
			//align->rend.save("combi.jpg");
		}
		/*for (int x=0; x<align->wt; x++)
			for (int y=0; y<align->ht; y++)
			{
				float totMI=0.0;
				QColor color;
				color.setRgb(comb.pixel(x,y));
				if (color!=qRgb(0,0,0))
				{
					std::vector<QColor> cols;
					QColor c;
					for (int n=0; n<projimg.size(); n++)
					{
						
						QColor color2;
						color2.setRgb(projimg[n].pixel(x,y));
						if (color!=color2)
						{
							totMI+=align->arcMI[n];
							cols.push_back(color2);
						}
					}
					if (totMI!=0.0)
					{	
						float red=0.0;
						float green=0.0;
						float blue=0.0;
						for (int i=0; i<cols.size(); i++)
						{
							red+=cols[i].red()*(align->arcMI[i]/totMI);
							green+=cols[i].green()*(align->arcMI[i]/totMI);
							blue+=cols[i].blue()*(align->arcMI[i]/totMI);
						}
						comb.setPixel(x,y,qRgb((int)red,(int)green,(int)blue));
	
					}

						


				}

			}*/

		align->mode=AlignSet::NODE;
		//comb.save("combi.jpg");
		/*for (int n=0; n<projimg.size(); n++)
		{
			delete projimg[n];
			
		}*/
////////
		QColor color;
		int offset = 0;
		//equalize image
		int histo[256];
		memset(histo, 0, 256*sizeof(int));
		for (int y = h-1; y >= 0; y--) {
		for (int x = 0; x < w; x++) {
		color.setRgb(align->rend.pixel(x, y));
		unsigned char c = (unsigned char)(color.red() * 0.3f + color.green() * 0.59f + color.blue() * 0.11f);
		align->render[offset] = c;
		histo[c]++;
		offset++;
			}
		  }
///////
	}
    //align->renderScene(shot,0);
    for(int i = 0; i < w; i+= wstep) {
       int iend = i + wstep;
       if(iend > w) iend = w;
       for(int j =0; j < h; j+= hstep) {
         int jend = j+hstep;
         if(jend > h) jend = h;
         double m =8-mutual->info(w, h, align->target, align->render, i, iend, j, jend);
         info += m;
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
  if(align->correspList->size()>0)
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

int Solver::iterative(AlignSet *_align, MutualInfo *_mutual, Shot &shot) {
  //QString fileName = _align->projectName;
  //fileName.append(".txt");
  ////myfile.open ("result.txt");
  //myfile.open (fileName.toLatin1());
  //assert(myfile.is_open());
  ////myfile << " k=" << mIweight << "\n";
  //assert(myfile.badbit);

  f_evals = 0;
  f_evals_total =0;
  double var = variance;
  double tol = tolerance;
  double niter = maxiter;
  maxiter = niter/4;
  variance = 6;
  tolerance = variance/10;
  while(f_evals < niter && variance > 0.1) {
    f_evals += optimize(_align, _mutual, shot);
    shot = _align->shot;
    //cout << "Iter: " << f_evals << endl;
    //get max p:
    double max = 0;
    for(int i = 0; i < p.size(); i++)
      if(fabs(p[i]) > max) max = fabs(p[i]);
    //cout << "Max: " << max << endl;
    variance = max/8;
    if(variance > 20) variance = 20;
    tolerance = variance/10;
  }
  variance = var;
  tolerance = tol;
  maxiter = niter;

  //myfile.close();

  return f_evals;
}

//int Solver::convergence(AlignSet *_align, MutualInfo *_mutual, Shot &shot, QTextStream &stream) {
//  p = Parameters(optimize_focal, shot, _align->width(), _align->height(),
//                 _align->box, _align->mesh);
//
//  double mindist = 10;
//  double maxdist = 60;
//  int niter = 300;
//  int seed = 0;
//  int n = 6;
//  if(p.use_focal) n = 7;
//  Shot initial = shot;
//  for(int i = 0; i < niter; i++) {
//    srand(seed++);
//    double len = mindist + i*(maxdist-mindist)/(niter);
//    p.randomDir(n, p.p, len);
//
//    double max_step = len;
//    double initial_dist = p.pixelDiff(_align->mesh);
//    Shot test = p.toShot();
//    int evals = iterative(_align, _mutual, test);
//    double final_dist = p.pixelDiff(initial, _align->mesh);
//    stream << initial_dist << " " << final_dist << " " << len << " " << p.norm()
//          << " " << evals;
//
//    for(int i = 0; i < n; i++)
//      stream << " " << p[i];
//    stream << endl;
//    p.reference = initial;
//  }
//  _align->shot = initial;
//  return 0;
//}
//
//int Solver::graphs(AlignSet *_align, MutualInfo *_mutual, Shot &shot, QTextStream &stream) {
//  align = _align;
//  mutual = _mutual;
//
//  p = Parameters(optimize_focal, shot, align->width(), align->height(),
//                 align->box, align->mesh);
//
//  double maxdist = 10;
//  int niter = 30;
//  const int naxis = 30;
//  double step = 2*maxdist/niter;
//  //n directions of norm 1
//  int n = 6;
//  if(p.use_focal) n = 7;
//  double *dirs = new double[naxis * n];
//
//  for(int  k =0 ; k < naxis; k++) {
//    p.randomDir(n, &(dirs[k*n]));
//    if(dirs[k*n+2] < 0) {
//      for(int i = 0; i < n; i++)
//        dirs[k*n+i] *= -1;
//    }
//  }  
//  for(double j = -maxdist; j <= maxdist; j += step) {
//    stream << j << " ";
//    for(int k = 0; k < naxis; k++) {
//
//      for(int i = 0; i < n; i++) {
//        p[i] = dirs[k*n+i]*j;
//      }
//
//      Shot shot = p.toShot();
//
//      align->shot = shot;
//      align->renderScene(shot);
//
//      int w = align->width();
//      int h = align->height();
//
//      double info = 0;
//
//      switch(align->mode) {
//      case AlignSet::NORMALMAP:
//      case AlignSet::COMBINE:
//      case AlignSet::SPECULAR:
//      case AlignSet::SPECAMB:
//        align->readRender(1);
//        info += 2 - mutual->info(w, h, align->target, align->render);
//      case AlignSet::COLOR:
//      case AlignSet::SILHOUETTE: {
//        align->readRender(0);
//        info += 2 - mutual->info(w, h, align->target, align->render);
//        }
//      }
//      
//	  //Per considerare k (il peso delle MI)
//	  double weight = mIweight;
//	  double error =0;
//      if(align->correspList->size()>0 && weight!=1)
//	    error = calculateError2(shot);
//      info = weight*info + (1-weight)*error;
//
//      stream << info << " ";
//	}
//    stream << endl;
//    for(int i = 0; i < n; i++)
//      p[i] = 0;
//    Shot shot = p.toShot();
//    align->shot = shot;
//    align->renderScene(shot);
//	//delete []dirs;
//  }  
//  return 0;
//}

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

  int niter=0;
  /*int niter = dlevmar_dif(Solver::value, _p, x, p.size(), sides,
                          maxiter, opts, info,
                          NULL, NULL, this); OKKIO */ 

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

  delete []x;
  delete _p;


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

template<class Correlation>
double Solver::calculateError(std::list<Correlation> *corrs, Shot &shot){
        typename std::list<Correlation>::iterator it_c;
	Correlation* c;

    double ratio = shot.Intrinsics.ViewportPx.X()/(double) shot.Intrinsics.ViewportPx.Y();
	vcg::Point3f *p1;
	vcg::Point2d *p2;
    int count=0;

	double error = 0;
// OKKIO!!
 //   for ( it_c= corrs->begin() ; it_c !=corrs->end(); it_c++ ){
	//	c=&*it_c;
	//	p1=&(c->point3d);
	//	p2=&(c->point2d);

	//	if(p1!=NULL && p2!=NULL)
	//	{
	//		//Adjust 2D point
	//		vcg::Point2d p2adj(((p2->X()/ratio) +1)/2.0 * shot.Intrinsics.CenterPx.X()*2.0,((p2->Y())+1)/2.0 * shot.Intrinsics.CenterPx.Y()*2.0);
	//		//Project 3D point
	//		vcg::Point2f p1proj = shot.Project(*p1);
	//		//Calculate disntance
	//		float dist = vcg::Distance<float>(p1proj,p2adj);
	//		error += dist;
	//		count++;
	//	}
	//}//all corrs

	////Normalize error
	return error /= count;
}

double Solver::calculateError2( Shot &shot){
	//E' una pezza, andrebbe meglio pensato. Va a beccare direttamente le strutture dati PointCorrespondence di base.
	//align già è sicuramente settato perchè lo chiami da optimize (poi dovrai distinguere le due cose, p.e. fare un optimize2)
	QList<PointCorrespondence*> *correspList = align->correspList;
	double error = 0;
	int count=0;

    for( int i=0; i<correspList->count(); i++){
        PointCorrespondence *corr = correspList->at(i);
        PointOnLayer currentPointOnLayer1= corr->getPointAt(0);
        PointOnLayer currentPointOnLayer2= corr->getPointAt(1);
        PointType type1 = currentPointOnLayer1.getType();
        vcg::Point3f currentPoint1(currentPointOnLayer1.pX, currentPointOnLayer1.pY,currentPointOnLayer1.pZ );
        vcg::Point3f currentPoint2(currentPointOnLayer2.pX, currentPointOnLayer2.pY,currentPointOnLayer2.pZ );
        vcg::Point2f p2adj;
		vcg::Point2f p1proj;
        if(type1==_3D_POINT){
            //Adjust 2D point
			p2adj = vcg::Point2f(((currentPoint2.X()/align->imageRatio) +1)/2.0 * shot.Intrinsics.CenterPx.X()*2.0,((currentPoint2.Y())+1)/2.0 * shot.Intrinsics.CenterPx.Y()*2.0);
			//Project 3D point
			p1proj = shot.Project(currentPoint1);
        }
        else{
           //Adjust 2D point
			p2adj = vcg::Point2f(((currentPoint1.X()/align->imageRatio) +1)/2.0 * shot.Intrinsics.CenterPx.X()*2.0,((currentPoint1.Y())+1)/2.0 * shot.Intrinsics.CenterPx.Y()*2.0);
			//Project 3D point
			p1proj = shot.Project(currentPoint2);
        }

		float dist = vcg::Distance<float>(p1proj,p2adj);
		error += dist;
		count++;
	}

	//Normalize error
	return error /= count;

}

bool Solver::levmar(AlignSet *_align, Shot &shot){
	align = _align;
    QList<PointCorrespondence*> *correspList = align->correspList;

	std::list<LevmarCorrelation> *corrs = new std::list<LevmarCorrelation>();
	for( int i=0; i<correspList->count(); i++){
		PointCorrespondence *corr = correspList->at(i);
		PointOnLayer currentPointOnLayer1= corr->getPointAt(0);
		PointOnLayer currentPointOnLayer2= corr->getPointAt(1);
		PointType type1 = currentPointOnLayer1.getType();
		LevmarCorrelation *corrLevmar = new LevmarCorrelation();
		if(type1==_3D_POINT){
			vcg::Point3f currentPoint3d(currentPointOnLayer1.pX, currentPointOnLayer1.pY,currentPointOnLayer1.pZ );
			corrLevmar->point3d = currentPoint3d;
			vcg::Point2d currentPoint2d(currentPointOnLayer2.pX, currentPointOnLayer2.pY);
			corrLevmar->point2d = currentPoint2d;
		}
		else{
			vcg::Point3f currentPoint3d(currentPointOnLayer2.pX, currentPointOnLayer2.pY,currentPointOnLayer2.pZ );
			corrLevmar->point3d = currentPoint3d;
			vcg::Point2d currentPoint2d(currentPointOnLayer1.pX, currentPointOnLayer1.pY);
			corrLevmar->point2d = currentPoint2d;
		}
		qDebug("Point3d %f %f %f",(float)corrLevmar->point3d.X(),(float)corrLevmar->point3d.Y(),(float)(float)corrLevmar->point3d.Z());
		qDebug("Point2d %f %f %f",(float)corrLevmar->point2d.X(),(float)corrLevmar->point2d.Y());

		corrs->push_back(*corrLevmar);
	}

	vcg::Camera<float> &cam = shot.Intrinsics;

	//DEBUG
	qDebug("\n TEST BEFORE CALIBRATION \n");
	qDebug("Focal %f",cam.FocalMm);
	qDebug("ViewportPx.X %i",cam.ViewportPx.X());
	qDebug("ViewportPx.Y %i",cam.ViewportPx.Y());
	qDebug("CenterPx.X %f",cam.CenterPx[0]);
	qDebug("CenterPx.Y %f",cam.CenterPx[1]);
	qDebug("DistorntedCenterPx.X %f",cam.DistorCenterPx[0]);
	qDebug("DistorntedCenterPx.Y %f",cam.DistorCenterPx[1]);
	qDebug("PixelSizeMm.X %f",cam.PixelSizeMm[0]);
	qDebug("PixelSizeMm.Y %f",cam.PixelSizeMm[1]);
	qDebug("k1 %f",cam.k[0]);
	qDebug("k2 %f",cam.k[1]);
	qDebug("Tra %f %f %f",shot.Extrinsics.Tra().X(),shot.Extrinsics.Tra().Y(),shot.Extrinsics.Tra().Z());
	for(int i=0;i<4;i++)
		qDebug("Rot %f %f %f %f", (shot.Extrinsics.Rot())[i][0],(shot.Extrinsics.Rot())[i][1],(shot.Extrinsics.Rot())[i][2],(shot.Extrinsics.Rot())[i][3] );

	Shot previousShot = shot;
	//First calculate only extrinsics
	bool result = LevmarMethods::calibrate(&shot,corrs,false);
	//If user wants calibrate the focal (these have to be two different steps)
	if(optimize_focal)
		result = LevmarMethods::calibrate(&shot,corrs,optimize_focal);

	qDebug("End calibration");
	
	align->error = calculateError(corrs,shot);

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
	qDebug("Focal %f",cam.FocalMm);
	qDebug("ViewportPx.X %i",cam.ViewportPx.X());
	qDebug("ViewportPx.Y %i",cam.ViewportPx.Y());
	qDebug("CenterPx.X %f",cam.CenterPx[0]);
	qDebug("CenterPx.Y %f",cam.CenterPx[1]);
	qDebug("DistortedCenterPx.X %f",cam.DistorCenterPx[0]);
	qDebug("DistortedCenterPx.Y %f",cam.DistorCenterPx[1]);
	qDebug("PixelSizeMm.X %f",cam.PixelSizeMm[0]);
	qDebug("PixelSizeMm.Y %f",cam.PixelSizeMm[1]);
	qDebug("k1 %f",cam.k[0]);
	qDebug("k2 %f",cam.k[1]);
	qDebug("Tra %f %f %f",shot.Extrinsics.Tra().X(),shot.Extrinsics.Tra().Y(),shot.Extrinsics.Tra().Z());
	for(int i=0;i<4;i++)
		qDebug("Rot %f %f %f %f", (shot.Extrinsics.Rot())[i][0],(shot.Extrinsics.Rot())[i][1],(shot.Extrinsics.Rot())[i][2],(shot.Extrinsics.Rot())[i][3] );
	
	align->shot=shot;
	return result;
}

