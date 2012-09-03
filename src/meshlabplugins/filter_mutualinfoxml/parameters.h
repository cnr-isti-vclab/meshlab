#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vcg/space/point3.h>
#include <vcg/math/shot.h>
#include <vcg/space/box3.h>

class CMeshO;

class Parameters {
 typedef vcg::Shot<float> Shot;
 typedef vcg::Box3<float> Box;

 public:
  double p[7];
  double scale[7];
  bool use_focal;
  bool max_norm; //use max distance as pixel diff (instead of rms)

  vcg::Shot<float> reference;
  vcg::Box3f box;
  //vcg::Point3d center;
  double rx, ry; //ratio between fbo width and height and reference values

  Parameters(): use_focal(true), max_norm(false) {}
//  Parameters(bool _use_focal, Shot &_reference, int w, int h, vcg::Box3d &_box);
  Parameters(bool _use_focal, Shot &_reference, int w, int h, Box &_box,
             CMeshO &mesh,  int nsamples = 1000); 
  Parameters(bool _use_focal, Shot &_reference, int w, int h, Box &_box);

  int size(); //number of used parameters
  double &operator[](int n) { return p[n]; }
  double norm();
  void reset();
  double random(double max = 1.0, double min = 0.0);
  void randomDir(int n, double *p, double len = 1.0);
  void rattle(double amount);

  void initScale(CMeshO &mesh, int nsamples);
  void initScale();

  //estimate pixeldifference 
  double pixelDiff(CMeshO &mesh, int samples = 1000);
  double pixelDiff(vcg::Shot<float> &test, CMeshO &mesh, int samples = 1000);

  double pixelDiff();
  double pixelDiff(vcg::Shot<float> &test);

  vcg::Point2f pixelDiff(vcg::Shot<float> &test, vcg::Point3f p);

  vcg::Shot<float> toShot(bool scale = true);
  //void ShotAndSim(vcg::Shot<double> &shot, vcg::Similarity<double, vcg::Matrix44<double> > &s);

 private:
  void scramble(double *_p, bool rescale);
};

#endif
