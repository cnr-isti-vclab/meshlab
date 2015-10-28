#ifndef ALIGNSET_H
#define ALIGNSET_H


#include <QString>
#include <QImage>
#include <QGLFramebufferObject>

// local headers
#include "common/meshmodel.h"

// VCG headers
#include <vcg/math/shot.h>


class QGLFramebufferObject;

class Correspondence
{
public:
	vcg::Point3f Point3D;
	vcg::Point2f Point2D;
	int index;
	double error;
};


class AlignSet {
  //typedef vcg::Camera<float> Camera;
  //typedef vcg::Shot<float> Shot;
  //typedef vcg::Box3<float> Box;

 public:

  int wt,ht;
  CMeshO* mesh;
  QImage* image;
  double imageRatio;
  vcg::Shot<float> shot;
  vcg::Box3<float> box;
  std::vector<Correspondence> correspList; //List that includes corresponces involving the model
  double error; //alignment error in px

  GLuint vbo, nbo, cbo, ibo;  // vertex buffer object (vertices, normals, colors indices)

  GLint programs[6];
  
  enum RenderingMode {COMBINE=0, NORMALMAP=1, COLOR=2, SPECULAR=3, SILHOUETTE=4, SPECAMB = 5};
  RenderingMode mode;

  unsigned char *target, *render; //buffers for rendered images 

  AlignSet();
  ~AlignSet();

  void initializeGL();

  int width() { return wt; }
  int height() { return ht; }
  void resize(int max_side); // resize the fbo and the images so that the longest side is max_side
  double focal();
  bool setFocal(double f); //return false if unchanged
  void setPixelSizeMm(double ccdWidth);

  void renderScene(vcg::Shot<float> &shot, int component);
  void readRender(int component);

  void drawMeshPoints();
  void drawImagePoints();

  void undistortImage();

  void resetAlign();

 private:
  
  
 
  GLuint createShaderFromFiles(QString basename); // converted into shader/basename.vert .frag
  GLuint createShaders(const char *vert, const char *frag);

};


#endif
