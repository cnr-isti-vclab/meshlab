#include "point_matching_scale.h"
#include <newuoa.h>

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>

using namespace vcg;


template <class Scalar>
struct RotoTranslation
{
	RotoTranslation(){}
	Scalar _v[6];
	void ToMatrix(vcg::Matrix44<Scalar> & m)
	{
		vcg::Matrix44<Scalar> rot,tra;
		rot.FromEulerAngles(_v[0],_v[1],_v[2]);
		tra.SetTranslate(vcg::Point3<Scalar>(_v[3],_v[4],_v[5]));
		m = tra * rot;
	}
};



static std::vector<Point3d> *fix;
static std::vector<Point3d> *mov;
static vcg::Box3d b;

double errorScale(int, double *x)
{
  assert(n==1);
  double dist = 0;
  std::vector<Point3d>::iterator i = mov->begin();
  std::vector<Point3d>::iterator ifix = fix->begin();
  for(; i !=  mov->end(); ++i,++ifix)
    dist += vcg::SquaredDistance(((*i)-b.Center())*(*x)+b.Center() , *ifix);

  return dist;
}


void ComputeScalingMatchMatrix(Matrix44d &res,
                               std::vector<Point3d> &Pfix,
                               std::vector<Point3d> &Pmov)
{
  fix = &Pfix;
  mov = &Pmov;
  b.SetNull();
  for(std::vector<Point3d>::iterator i = Pmov.begin(); i != Pmov.end(); ++i)
    b.Add(*i);

  double scale = 1.0;
  min_newuoa(1,&scale,errorScale);

  res.SetTranslate( b.Center()*(1.0-scale));
  res[0][0] = res[1][1] = res[2][2] = scale;
}


double errorRotoTranslationScale(int n, double *x){
  assert(n==7); (void)n;
  double dist = 0;
  std::vector<Point3d>::iterator i = mov->begin();
  std::vector<Point3d>::iterator ifix = fix->begin();

  RotoTranslation<double> rt;
  vcg::Matrix44d m;
  memcpy(&rt._v[0],&x[1],6*sizeof(double));
  rt.ToMatrix(m);

  for(; i !=  mov->end(); ++i,++ifix){
    dist += vcg::SquaredDistance(	 m*(((*i)-b.Center())*(x[0])+b.Center()),*ifix);
  }
  return dist;
}

void ComputeRotoTranslationScalingMatchMatrix(vcg::Matrix44d &res,
                                              std::vector<Point3d> &Pfix,
                                              std::vector<Point3d> &Pmov)
{
  fix = &Pfix;
  mov = &Pmov;
  b.SetNull();
  for(std::vector<Point3d>::iterator i = Pmov.begin(); i != Pmov.end(); ++i)
    b.Add(*i);

  double x[7]={1.0,0.0,0.0,0.0,0.0,0.0,0.0};
  min_newuoa(7,&x[0],errorRotoTranslationScale);

  // rtm = rototranslation
  RotoTranslation<double> rt;
  vcg::Matrix44d rtm;
  memcpy(&rt._v[0],&x[1],6*sizeof(double));
  rt.ToMatrix(rtm);

  // res= scaling w.r.t. barycenter
  res.SetTranslate( b.Center()*(1.0-x[0]));
  res[0][0] = res[1][1] = res[2][2] = x[0];
  res = rtm*res;
}

