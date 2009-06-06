#include "object.h"
#include "point_based_renderer.h"


Object::~Object() {

  glDeleteLists(pointsDisplayList, 1);
}

/**
 * Render object using designed rendering system.
 **/
void Object::render ( void ) const{

  glCallList(pointsDisplayList);
  

//   glBegin(GL_POINTS);

//   for (surfelVectorIterConst it = surfels.begin(); it != surfels.end(); ++it) {
// 	glColor4f((GLfloat)(it->Color()[0] / 255.0), (GLfloat)(it->Color()[1] / 255.0), (GLfloat)(it->Color()[2] / 255.0), (GLfloat)(1.0));
//     glNormal3f((GLfloat)it->Normal()[0], (GLfloat)it->Normal()[1], (GLfloat)it->Normal()[2]);
//     glVertex4f((GLfloat)it->Center()[0], (GLfloat)it->Center()[1], (GLfloat)it->Center()[2], (GLfloat)it->Radius());
//   }
//   glEnd();

  check_for_ogl_error("Primitives render");

}

/**
 * Changes the renderer type.
 * @param rtype Given renderer type.
 **/
void Object::setRendererType ( int rtype ) {

  renderer_type = rtype;

  if (rtype == PYRAMID_POINTS) {
	setPyramidPointsDisplayList();
  }
  else if (rtype == PYRAMID_POINTS_COLOR) {
	setPyramidPointsColorDisplayList();
  }

}

void Object::setPyramidPointsDisplayList ( void ) {

  pointsDisplayList = glGenLists(1);

  glNewList(pointsDisplayList, GL_COMPILE);

  glPointSize(1.0);

  glBegin(GL_POINTS);
  
  for (surfelVectorIter it = surfels.begin(); it != surfels.end(); ++it) {
    glNormal3f((GLfloat)it->Normal()[0], (GLfloat)it->Normal()[1], (GLfloat)it->Normal()[2]);
    glVertex4f((GLfloat)it->Center()[0], (GLfloat)it->Center()[1], (GLfloat)it->Center()[2], (GLfloat)it->Radius());
  }
  glEnd();

  glEndList();
}

void Object::setPyramidPointsColorDisplayList ( void ) {

  //  normalizeQuality();

  pointsDisplayList = glGenLists(1);
  glNewList(pointsDisplayList, GL_COMPILE);

  glBegin(GL_POINTS);

  for (surfelVectorIter it = surfels.begin(); it != surfels.end(); ++it) {
	glColor4f((GLfloat)(it->Color()[0] / 255.0), (GLfloat)(it->Color()[1] / 255.0), (GLfloat)(it->Color()[2] / 255.0), (GLfloat)(1.0));
    glNormal3f((GLfloat)it->Normal()[0], (GLfloat)it->Normal()[1], (GLfloat)it->Normal()[2]);
    glVertex4f((GLfloat)it->Center()[0], (GLfloat)it->Center()[1], (GLfloat)it->Center()[2], (GLfloat)it->Radius());
  }
  glEnd();

  glEndList();
}

void Object::clearSurfels ( void ) {  
  surfels.clear();
}

void Object::normalizeQuality( void ){
  double mean = 0.0;
  double standard_deviation = 0.0;
  max_quality = 0.0;
  min_quality = 10000.0;
  for (surfelVectorIter it = surfels.begin(); it != surfels.end(); ++it) {
	double q = (double)it->Quality();
	if (q < 10000)
	  mean += (double)it->Quality();
  }
  mean /= double(surfels.size());

  for (surfelVectorIter it = surfels.begin(); it != surfels.end(); ++it) {
	double q = (double)it->Quality();
	if (q < 10000)
	  standard_deviation += ((double)it->Quality() - mean) * ((double)it->Quality() - mean);
  }
  standard_deviation = sqrt ( (1.0/double(surfels.size())) * standard_deviation );

  for (surfelVectorIter it = surfels.begin(); it != surfels.end(); ++it) {

	double q = (double)it->Quality();
	//q = (q - mean) / (standard_deviation / two_pi);
	if (q < 10000) {
	  if (q > max_quality)
		max_quality = q;
	  if (q < min_quality)
		min_quality = q;
	}
  }
}
