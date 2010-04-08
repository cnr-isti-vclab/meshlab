/**
 * Interface independent application class
 * 
 * Author : Ricardo Marroquim
 *
 * Date created : 20-12-2007
 *
 **/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "splat_pyramid.h"

#include <QGLWidget>
#include <QTextStream>
#include <wrap/gl/trimesh.h>

//#include <QGLFramebufferObject>

//#include "GL/glut.h"

using namespace std;
using namespace vcg;

void PyramidPointRendererPlugin::Init(QAction *, MeshDocument &md, RenderMode &, QGLWidget *gla)
{
	gla->makeCurrent();
	glewInit();

	const char* rs = (const char*)glGetString(GL_RENDERER);
	
	cout << "GL_RENDERER : " << rs << endl;	
	if (!GLEW_ARB_texture_float)
		std::cout << "SplatPyramid: warning floating point textures are not supported.\n";

	canvas_width = gla->width();
	canvas_height = gla->height();

	objects.clear();

	bool color_per_vertex = false;

	foreach(MeshModel * mp, md.meshList) {
    if(mp->hasDataMask(MeshModel::MM_VERTRADIUS))
    {
    // Create a new primitive from given file
    objects.push_back( Object( objects.size() ) );

    vector<Surfeld> *surfels = (objects.back()).getSurfels();

    if (mp->hasDataMask( MeshModel::MM_VERTCOLOR ) )
      color_per_vertex = true;

    Color4b c (180, 180, 180, 255);
    float quality = 0.0001;
    double radius = 1.0;

    int pos = 0;
    CMeshO::VertexIterator vi;

    for(vi=mp->cm.vert.begin(); vi!=mp->cm.vert.end(); ++vi)
      if(!(*vi).IsD())
      {
        Point3f p = (*vi).P();
        Point3f n = (*vi).N();
        radius = (*vi).R();
        if (color_per_vertex)
        c = (*vi).C();

        surfels->push_back ( Surfeld (p, n, c, quality, radius, pos) );
        ++pos;
      }
    }
  }
	if (color_per_vertex)
		render_mode =  PYRAMID_POINTS_COLOR;
	else
		render_mode =  PYRAMID_POINTS;
			
	/// Sets the default rendering algorithm and loads display lists
	for (unsigned int i = 0; i < objects.size(); ++i)
		objects[i].setRendererType( render_mode );

  	/// Sets the path to the meshlab's shaders directory
  	QDir shadersDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release" || shadersDir.dirName() == "plugins"  )
		shadersDir.cdUp();
#elif defined(Q_OS_MAC)
	if (shadersDir.dirName() == "MacOS") {
		for(int i=0;i<6;++i){
			if(shadersDir.exists("shaders")) break;
			shadersDir.cdUp();
		}
	}
#endif
	
	QDir::setCurrent(shadersDir.absolutePath());

	createPointRenderer( );  

	if (sDialog) {
		sDialog->close();
	delete sDialog;
		sDialog=0;
	}

	sDialog = new Dialog(point_based_render, gla);
	sDialog->move(10,100);

	sDialog->show();
}

/**
 * Initialize opengl and application state variables.
 * @param default_mode Defines the initial rendering mode.
 **/
PyramidPointRendererPlugin::PyramidPointRendererPlugin() {

  render_mode = PYRAMID_POINTS;
  point_based_render = NULL;

  fps_loop = 0;

  rotating = 0;
  show_points = false;
  selected = 0;
  scale_factor = 1.0;

  sDialog = 0;

  //check_for_ogl_error("Init");
}

void PyramidPointRendererPlugin::initActionList()
{
  actionList << new QAction("SplatPyramid", this);
}

PyramidPointRendererPlugin::~PyramidPointRendererPlugin( ) {
  objects.clear();
  delete point_based_render;
}

/// Render all points with OpenGL
void PyramidPointRendererPlugin::drawPoints(void) {
  glPointSize(1.0);
  glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

  glBegin(GL_POINTS);
  
  for (surfelVectorIter it = objects[0].getSurfels()->begin(); it != objects[0].getSurfels()->end(); ++it) {
	// 	Color4b c = it->Color();
	// 	glColor4f(c[0], c[1], c[2], 1.0f);  
	Point3f p = it->Center();
	//glVertex3f(p[0], p[1], p[2]);
  }
  glEnd();
}

/** 
 * Display method to render the models.
 **/
//void PyramidPointRendererPlugin::draw( void ) {
void PyramidPointRendererPlugin::Render(QAction *, MeshDocument &, RenderMode &, QGLWidget* gla )
{
	if (objects.size() == 0)
		return;

	point_based_render->clearBuffers();

	glPushMatrix();

	Matrix44f mv;
	glGetv(GL_MODELVIEW_MATRIX,mv);

	scale_factor = (mv.GetColumn3(0)).Norm();

	float fov_scale = ((GLArea*)gla)->getFov();
	if (fov_scale == 5)
		fov_scale = 1.0;
	else
		fov_scale = 1.0 / (tanf(vcg::math::ToRad(fov_scale*.5f)) * 1.0);

	// Set factor for scaling projected radii of samples in projection phase
	scale_factor = fov_scale;
	point_based_render->setScaleFactor( scale_factor );

	Invert(mv);
	Point3f eye = mv * Point3f(0, 0, 0);

	// Set eye for back face culling in vertex shader of projection phase
	point_based_render->setEye( Point3f(eye[0], eye[1], eye[2]) );

	for (unsigned int i = 0; i < objects.size(); ++i)
		point_based_render->projectSamples( &objects[i] );

	point_based_render->interpolate();

	point_based_render->draw();

	//   if (show_points)
	//     drawPoints();

	glPopMatrix();

	//glFinish();
}

/// Reshape func
/// @param w New window width
/// @param h New window height
void PyramidPointRendererPlugin::reshape(int, int) {
  //  camera->reshape(w, h);
}

/**
 * Changes the rendering algorithm.
 * @param type Rendering mode.
 **/
void PyramidPointRendererPlugin::changeRendererType( int type ) {
  for (unsigned int i = 0; i < objects.size(); ++i)
	objects[i].setRendererType((point_render_type_enum) type);
  render_mode = type;
  createPointRenderer( );
}

/**
 * Defines the rendering algorithm.
 * point_based_render is of generic type, depending on
 * the choice one of the inherited classes is instanced.
 **/
void PyramidPointRendererPlugin::createPointRenderer( void ) {  

	if (point_based_render)
		delete point_based_render;

	if (render_mode == PYRAMID_POINTS)
		point_based_render = new PyramidPointRenderer(canvas_width, canvas_height);
	else if (render_mode == PYRAMID_POINTS_COLOR)
		point_based_render = new PyramidPointRendererColor(canvas_width, canvas_height);

	assert (point_based_render);

	((PyramidPointRendererBase*)point_based_render)->setShadersDir(QDir::currentPath());
	((PyramidPointRendererBase*)point_based_render)->createShaders();
}


/**
 * Returns the model's number of points.
 * @return Number of points.
 **/
int PyramidPointRendererPlugin::getNumberPoints ( void ) {
  
  int num_pts = 0;
  for (unsigned int i = 0; i < objects.size(); ++i)
	num_pts += objects[i].numberPoints();

  return num_pts;
}

/**
 * Sets the reconstruction filter size.
 * Multiplicator of the radius size.
 * @param s Reconstruction filter size.
 **/
void PyramidPointRendererPlugin::setReconstructionFilter ( double s ) { 
  if (point_based_render)
    point_based_render->setReconstructionFilterSize(s);
}

/**
 * Sets the quality threshold for interpolating samples.
 * @param q Quality threshold.
 **/
void PyramidPointRendererPlugin::setQualityThreshold ( double q ) { 
  if (point_based_render)
    point_based_render->setQualityThreshold(q);
}

/**
 * Sets the prefilter size.
 * @param s Prefilter size.
 **/
void PyramidPointRendererPlugin::setPrefilter ( double s ) { 
  if (point_based_render)
    point_based_render->setPrefilterSize(s);
}

/**
 * Sets the kernel size for the template rendering algorithm only.
 * @param m Kernel size mxm.
 **/
void PyramidPointRendererPlugin::setGpuMask ( int m ) {
  point_based_render->setGpuMaskSize( m );
}

/**
 * Sets per-vertex color on/off if the model has
 * color information per point.
 * @param c Per-vertex color state.
 **/
void PyramidPointRendererPlugin::setPerVertexColor ( bool ) {
  //point_based_render->setVertexPerColor( c );
  for (unsigned int i = 0; i < objects.size(); ++i) {
	// Reset renderer type to load per vertex color or default color in vertex array
	objects[i].setRendererType( objects[i].getRendererType() );
  }
}

/**
 * Turns auto-rotate on/off.
 * Auto-rotate turns the model continuously.
 * @param r Auto-rotate state.
 **/
void PyramidPointRendererPlugin::setAutoRotate ( bool r ) {
  rotating = r;
}

/**
 * Turns depth test on/off.
 * @param d Depth test state.
 **/
void PyramidPointRendererPlugin::setDepthTest ( bool d ) {
  if (point_based_render)
    point_based_render->setDepthTest(d);
}

/**
 * Change model material properties.
 * @param mat Id of material (see materials.h for list)
 **/
void PyramidPointRendererPlugin::changeMaterial( int mat ) {  
  point_based_render->setMaterial( mat );
}

/**
 * Turns backface culling on/off.
 * @param b Backface culling state.
 **/
void PyramidPointRendererPlugin::setBackFaceCulling ( bool c ) {
  if (point_based_render)
	point_based_render->setBackFaceCulling(c);
}

/**
 * Turns elliptical weights on/off.
 * @param b Elliptical weight state.
 **/
void PyramidPointRendererPlugin::setEllipticalWeight ( bool b ) {
  point_based_render->setEllipticalWeight(b);
}

/**
 * Turns quality per vertex on/off.
 * @param c Quality per vertex state.
 **/
void PyramidPointRendererPlugin::setQualityPerVertex ( bool c ) {
  point_based_render->setQualityPerVertex(c);
}

/// Cycles through objects list for displaying individual parts of the model.
/// When selected = 0 displays all files
void PyramidPointRendererPlugin::increaseSelected ( void ) {
  selected++;
  if (selected > (int)objects.size())
	selected = 0;
  cout << "selected : " << selected << endl;

}

/// Cycles through objects list for displaying individual parts of the model.
/// When selected = 0 displays all files
void PyramidPointRendererPlugin::decreaseSelected ( void ) {
  selected--;
  if (selected < 0)
	selected = objects.size();
  cout << "selected : " << selected << endl;
}

Q_EXPORT_PLUGIN(PyramidPointRendererPlugin)
