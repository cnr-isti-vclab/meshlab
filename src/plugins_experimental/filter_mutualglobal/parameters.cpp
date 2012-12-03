#include <iostream>

#include "../../common/meshmodel.h"
#include "parameters.h"

using namespace std;
using namespace vcg;
/*double p[7];
  double scale[7];
  bool use_focal;
  bool max_norm; //use max distance as pixel diff (instead of rms)

  Shot reference;
  vcg::Point3d center;
  double rx, ry; //ratio between fbo width
*/

/*Parameters::Parameters(bool _use_focal, Shot &_reference, int w, int h, Box3d &_box:
  use_focal(_use_focal), max_norm(false), reference(_reference), box(_box) {

  rx = w/(double)reference.Intrinsics.ViewportPx[0];
  ry = h/(double)reference.Intrinsics.ViewportPx[1];
  reset();
}*/

Parameters::Parameters(bool _use_focal, Shot &_reference, int w, int h, Box &_box,
                       CMeshO &mesh,  int nsamples): 
  use_focal(_use_focal), max_norm(false), reference(_reference), box(_box) {

  rx = w/(double)reference.Intrinsics.ViewportPx[0];
  ry = h/(double)reference.Intrinsics.ViewportPx[1];
  initScale(mesh, nsamples);
}

Parameters::Parameters(bool _use_focal, Shot &_reference, int w, int h, Box &_box):
  use_focal(_use_focal), max_norm(false), reference(_reference), box(_box) {

  rx = w/(double)reference.Intrinsics.ViewportPx[0];
  ry = h/(double)reference.Intrinsics.ViewportPx[1];
  initScale();
}

int Parameters::size() { //number of used parameters
  if(use_focal) return 7;
  return 6;
}

double Parameters::norm() {
  double dist = 0.0;
  for(int i = 0; i < 7; i++) 
    dist += p[i]*p[i];
  return sqrt(dist);
}

void Parameters::reset() {
  for(int i = 0; i < 7; i++) {
    p[i] = 0.0;
    scale[i] = 1.0;
  }
}

double Parameters::random(double max, double min) {
  assert(max >= min);
  double r = (double)rand() / ((double)(RAND_MAX)+(double)(1));
  return min + r*(max - min);
}

void Parameters::randomDir(int n, double *p, double len) {
  double dist = 0.0;
  for(int i = 0; i < n; i++) {
    p[i] = random(1, -1);
    dist += p[i]*p[i];;
  }
  dist = len/sqrt(dist);
  for(int i = 0; i < n; i++) 
    p[i] *= dist;
}

void Parameters::rattle(double amount) {
  double r[7];
  randomDir(size(), r, amount);
  for(int i = 0; i < size(); i++)
    p[i] += r[i];
}

void Parameters::initScale(CMeshO &mesh, int nsamples) {
  reset();

  const double step = 0.1;
  for(int i = 0; i < size(); i++) {
    p[i] = step;
    Shot test = toShot(false);
    double diff = pixelDiff(test, mesh, nsamples)/step;
    if(diff <= 0) {
      scale[i] = 1;
      cerr << "WARNING: parameter " << i << " does not change the image. " << endl;
    } else {
      scale[i] = 1/diff;
    }
    p[i] = 0.0;
  }
  /*scale[6] = 1;
  for(int i = 0; i < 300; i++) {
    p[6] = -20 + i*0.2;
    Shot test = toShot(false);
    double diff = pixelDiff(test, mesh, nsamples)/step;
    double f = test.Intrinsics.FocalMm/(test.Intrinsics.ViewportPx[0]*test.Intrinsics.PixelSizeMm[0]);
    cout << p[6] << " " << diff << " " << f << endl;
  }
  getchar();
  p[6] = 0;*/
}

void Parameters::initScale() {
  reset();

  for(int i = 0; i < size(); i++) {
    p[i] = 1.0;
    Shot test = toShot(false);
    scale[i] = 1/pixelDiff(test);
    p[i] = 0.0;
  }
}

double Parameters::pixelDiff(CMeshO &mesh, int nsamples) {
  Shot s = toShot();
  return pixelDiff(s, mesh, nsamples);
}

double Parameters::pixelDiff(Shot &test, CMeshO &mesh, int nsamples) {
  double maxdist = 0.0;
  double avedist = 0.0;
  int count = 0;
  for(int i = 0; i < nsamples; i++) {
    double r = (double)rand() / ((double)(RAND_MAX)+(double)(1));
    double g = (double)rand() / ((double)(RAND_MAX)+(double)(1));
    r = r*16000*16000+g*16000;
    int v = ((int)r)%(mesh.vert.size());
    vcg::Point3f c;
    c.Import(mesh.vert[v].P());
    Point2f diff = pixelDiff(test, c);
    double dd = diff.Norm();
    if(dd <= 0) continue; //outside of viewpoint
    if(dd > maxdist) maxdist = dd;
    avedist += dd*dd;
    count++;
  }
  if(max_norm) return maxdist;
  return sqrt(avedist/count);
}

double Parameters::pixelDiff() {
  Shot s = toShot();
  return pixelDiff(s);
}

double Parameters::pixelDiff(Shot &test) {
  double maxdist = 0.0;
  double avedist = 0.0;

  for(int i = 0; i < 8; i++) {
    double dd = pixelDiff(test, box.P(i)).Norm();
    if(dd < 0) continue; //outside of viewpoint
    if(dd > maxdist) maxdist = dd;
    avedist += dd*dd;
  }
  if(max_norm) return maxdist;
  return sqrt(avedist/8);
}

vcg::Point2f Parameters::pixelDiff(Shot &test, vcg::Point3f p) {
  vcg::Point2f before = reference.Project(p)*rx;
  if(before[0] < 0 || before[0] > reference.Intrinsics.ViewportPx[0] ||
     before[1] < 0 || before[1] > reference.Intrinsics.ViewportPx[1])
    return vcg::Point2f(0, 0);
  vcg::Point2f after = test.Project(p)*rx;
  return (after - before);
}

/*void Parameters::ShotAndSim(vcg::Shot<double> &shot, vcg::Similarity<double, vcg::Matrix44<double> > &s) {
  double _p[7];
  scramble(_p, scale);
  s.SetIdentity();
  s.rot.FromEulerAngles(_p[3], _p[4], _p[5]);
  s.tra = vcg::Point3d(_p[0], _p[1], _p[2]);
  s.tra += center - s.rot*center;
  shot = reference;
  if(use_focal) {
    shot.Intrinsics.FocalMm = reference.Intrinsics.FocalMm + _p[6];
    if(shot.Intrinsics.FocalMm <= 0.1) 
      shot.Intrinsics.FocalMm = 0.1;
  }
}*/

vcg::Shot<float> Parameters::toShot(bool scale) {
  double _p[7];
  scramble(_p, scale);

  Similarity<float, Matrix44<float> > s;
  s.SetIdentity();
  s.rot.FromEulerAngles(_p[3], _p[4], _p[5]);
  s.tra = vcg::Point3f(_p[0], _p[1], _p[2]);

  Shot shot = reference;
  if(use_focal) {
    double fov = reference.Intrinsics.FocalMm;  
    double h = reference.Intrinsics.ViewportPx[0]*reference.Intrinsics.PixelSizeMm[0];
    //double f = h/fov;
    //f += _p[6]/100;
    //shot.Intrinsics.FocalMm = h/f;

    //double newfov = fov*exp(0.1*_p[6]);
    double newfov = fov + _p[6];
    shot.Intrinsics.FocalMm = newfov;
    if(shot.Intrinsics.FocalMm <= 1) shot.Intrinsics.FocalMm = 1;
  } else {
    shot.Intrinsics.FocalMm = reference.Intrinsics.FocalMm;
  }
  Matrix44f rot = shot.Extrinsics.Rot();
  Matrix44f irot = Inverse(rot);

  Point3f tra = shot.Extrinsics.Tra();

//rotation in camera space, remove it and we are in model space
  s.rot = irot*s.rot*rot;

  Matrix44f isrot = Inverse(s.rot);
  Point3f center = box.Center();

  shot.Extrinsics.SetRot(rot *s.rot);
  shot.Extrinsics.SetTra(irot*s.tra + isrot*(tra - center)  + center);

  return shot;
}

void Parameters::scramble(double *_p, bool rescale) {
  if(rescale) {
    for (int i = 0; i < size(); i++)
    _p[i] = p[i]*scale[i];
  } else {
    for (int i = 0; i < size(); i++)
    _p[i] = p[i];
  } 
  if(use_focal) {
    Point3f center = box.Center();
    //initial distance to the center of the object
    double dist = (center - reference.Extrinsics.Tra()).Norm();
    //initial fov
    double fov = reference.Intrinsics.FocalMm;
    double h = reference.Intrinsics.ViewportPx[0]*reference.Intrinsics.PixelSizeMm[0];

    /*double f = h/fov;
    f += _p[6]/100;
    double newfov = h/f;*/
    //double newfov = fov*exp(0.1*_p[6]);
    double newfov = fov + _p[6];
    double ratio = (newfov - fov)/fov;

    Point3f fcenter = reference.ConvertWorldToCameraCoordinates(center);
    fcenter[0] = 0; fcenter[1] = 0;
    fcenter = reference.ConvertCameraToWorldCoordinates(fcenter);

    Point3f view = reference.Extrinsics.Tra();
    //view = center + (view - center)*ratio;
    Point3f diff = view - (center + (view - center)*ratio);

    _p[2] += ratio*dist; 

/*    double z = (_p[2] + _p[6])/2;
    double f = (_p[2] - _p[6])/2;
    _p[2] = z;
    _p[6] = f; */
  }
}

