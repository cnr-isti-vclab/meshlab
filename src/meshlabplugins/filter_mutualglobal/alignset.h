#ifndef ALIGNSET_H
#define ALIGNSET_H


#include <QString>
#include <QImage>
#include <QGLFramebufferObject>

// local headers
#include "common/meshmodel.h"
#include "alignGlobal.h"

// VCG headers
#include <vcg/math/shot.h>


//#include "fbo.h"

#include "pointCorrespondence.h"

class QGLFramebufferObject;

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
  vcg::Shot<float> shotPro;
  QImage* imagePro;
  vcg::Matrix44<float> shadPro;
  QList<PointCorrespondence*> *correspList; //List that includes corresponces involving the model
  double error; //alignment error in px
  QImage rend;
  QImage comb;
  Node* node;
  std::vector<QImage*> arcImages;
  std::vector<vcg::Shot<float>*> arcShots;
  std::vector<float> arcMI;
  std::vector<vcg::Matrix44<float>> prjMats;
  

  GLuint vbo, nbo, cbo, ibo;  // vertex buffer object (vertices, normals, colors indices)

  
  enum RenderingMode {
	  COMBINE =0,
	  NORMALMAP,
	  COLOR,
	  SPECULAR,
	  SILHOUETTE,
	  SPECAMB,
	  PROJIMG,
	  PROJMULTIIMG,
	  NODE,
	  RENDERING_MODE_LAST
	};

  RenderingMode mode;

  GLint programs[RENDERING_MODE_LAST];

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

  void renderScene(vcg::Shot<float> &shot, int component, bool save=false);
  void readRender(int component);

  void drawMeshPoints();
  void drawImagePoints();

  void undistortImage();

  void resetAlign();

  bool ProjectedImageChanged(const QImage & img);
  bool ProjectedMultiImageChanged();

  bool RenderShadowMap(void);
  bool RenderMultiShadowMap(void);


 private:
  
  
 
  GLuint createShaderFromFiles(QString basename); // converted into shader/basename.vert .frag
  GLuint createShaders(const char *vert, const char *frag);

  GLuint depthFB;
  GLuint depthTex;
  GLuint prjTex;
  GLuint depthFB2;
  GLuint depthTex2;
  GLuint prjTex2;
  GLuint depthFB3;
  GLuint depthTex3;
  GLuint prjTex3;
  GLuint depthPrg;
  int    depthW;
  int    depthH;

	
	
};

#endif
