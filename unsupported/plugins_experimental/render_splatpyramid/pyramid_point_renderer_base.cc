/*
** pyramid_point_render.cc Pyramid Point Based Rendering.
**
**
**   history:	created  02-Jul-07
*/

#include "pyramid_point_renderer_base.h"

#include <stdexcept>

using std::runtime_error;
#define FOO(a) case a: throw std::runtime_error( where + ": " #a ); break
void
checkFramebufferStatus( const std::string& where )
{
 	GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
	switch( status ) {
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;
	FOO( GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT );
	FOO( GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT );
//	FOO( GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT );
	FOO( GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT );
	FOO( GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT );
	FOO( GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT );
	FOO( GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT );
	FOO( GL_FRAMEBUFFER_UNSUPPORTED_EXT );
	//	FOO( GL_FRAMEBUFFER_STATUS_ERROR_EXT );
	default:
		throw std::runtime_error( where + ": Framebuffer error.\n" );
	}
}



void PyramidPointRendererBase::init ( void ) {

  cout << canvas_width << " " << canvas_height << endl;

  levels_count = min((int)(log(canvas_width)/log(2.0)), (int)(log(canvas_height)/log(2.0)));

  cout << "LEVELS :  " << (int)(log(canvas_width)/log(2.0)) << " " << (int)(log(canvas_height)/log(2.0)) << " " << levels_count << endl;

  resetPointers();
  createFBO();

  vertices[0][0] = 0.0;
  vertices[0][1] = 0.0;
  vertices[1][0] = 0.0; 
  vertices[1][1] = canvas_height;
  vertices[2][0] = canvas_width;
  vertices[2][1] = canvas_height;
  vertices[3][0] = canvas_width; 
  vertices[3][1] = 0.0;

  texcoors0[0][0] = 0.0;
  texcoors0[0][1] = 0.0;
  texcoors0[1][0] = 0.0;
  texcoors0[1][1] = 1.0;
  texcoors0[2][0] = 1.0;
  texcoors0[2][1] = 1.0;
  texcoors0[3][0] = 1.0;
  texcoors0[3][1] = 0.0;
}

/**
 * Default constructor.
 **/
PyramidPointRendererBase::PyramidPointRendererBase() : PointBasedRenderer(),
													   fbo_buffers_count(2) {
  init();
}

PyramidPointRendererBase::PyramidPointRendererBase(int w, int h) : PointBasedRenderer(w, h),
																   fbo_buffers_count(2) {
  init();
}

PyramidPointRendererBase::PyramidPointRendererBase(int w, int h, int fbos) : PointBasedRenderer(w, h),
																			 fbo_buffers_count(fbos) {
  init();
}

PyramidPointRendererBase::~PyramidPointRendererBase()  {

	glDeleteTextures(fbo_buffers_count, fbo_textures);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDrawBuffer(GL_BACK);

	glDeleteTextures(1, &fbo_depth);
	
	fbo_lod.clear();
	delete [] fbo_buffers;
	delete [] fbo_textures;
	delete [] shader_texture_names;
}


/**
 * Renders the QUAD with textures.
 * Each fragment in shader will read one pixel from texture.
 **/
const void PyramidPointRendererBase::rasterizePixels(void)
     
{
  // Rasterize texture as quads.
  // Uses multiple tex coords in the case of reading from multiple levels
  // at the same time. (ex. synthesis phase)
  glBegin(GL_QUADS);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  
  for(int i = 0; i < 4; i++) {
	glMultiTexCoord2fARB(GL_TEXTURE0, texcoors0[i][0], texcoors0[i][1]);
	glVertex2f(vertices[i][0], vertices[i][1]);
  }
  glEnd();
}

/**
 * Activates textures to be accessed from shaders.
 * @param text_id Texture number
 * @param target_id GL_TEXTURE number, this must correspond to shader uniform var.
 **/
const void PyramidPointRendererBase::activateTexture(const int text_id, const int target_id) {

  glActiveTexture(GL_TEXTURE0 + target_id);

  if (text_id == -1)
	glBindTexture(FBO_TYPE, (GLuint)0);
  else {   
	glBindTexture(FBO_TYPE, fbo_textures[text_id]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  }
}

/** 
 * Project point samples to screen space.
 * @param obj Pointer to object for rendering.
 **/
void PyramidPointRendererBase::projectSurfels ( const Object* const obj )
{
  int level = 0;

  // render targets (GL_COLOR_ATTACHMENTs)
  GLuint buffers[fbo_buffers_count];
  for (int i = 0; i < fbo_buffers_count; ++i)
	buffers[i] = fbo_buffers[i];

  fbo_lod[level]->bind();
  glDrawBuffers(fbo_buffers_count, buffers);

  mShaderProjection.prog.Bind();
  mShaderProjection.prog.Uniform("canvas_size", (GLfloat)canvas_width, (GLfloat)canvas_height);  
  mShaderProjection.prog.Uniform("eye", (GLfloat)eye[0], (GLfloat)eye[1], (GLfloat)eye[2]);
  mShaderProjection.prog.Uniform("back_face_culling", (GLint)back_face_culling);
  mShaderProjection.prog.Uniform("scale", (GLfloat)scale_factor); 

  // Render vertices from surfel list.
  glPointSize(1.0);
  obj->render();

  mShaderProjection.prog.Unbind();
  fbo_lod[level]->release();
}


/** 
 * Pull phase.
 * Create pyramid structure starting one level above base level (already created with projection).
 * Each pixel is the average of the four pixels in level below.
 **/
void PyramidPointRendererBase::rasterizeAnalysisPyramid( void ) {
  
	// render targets (GL_COLOR_ATTACHMENTs)
	GLuint buffers[fbo_buffers_count];
	for (int i = 0; i < fbo_buffers_count; ++i)
	buffers[i] = fbo_buffers[i];

	// source textures (samplers) that are acessed in shaders (GL_TEXTURE0, GL_TEXTURE1 ...)
	for (int i = 0; i < fbo_buffers_count; ++i)
	activateTexture(i, i);

	mShaderAnalysis.prog.Bind();
	mShaderAnalysis.prog.Uniform("prefilter_size", (GLfloat)(prefilter_size));
	mShaderAnalysis.prog.Uniform("reconstruction_filter_size", (GLfloat)(reconstruction_filter_size));
	mShaderAnalysis.prog.Uniform("depth_test", depth_test);
	
	for (int i = 0; i < fbo_buffers_count; ++i)
	mShaderAnalysis.prog.Uniform(shader_texture_names[i].c_str(), i); //samplers
	mShaderAnalysis.prog.Unbind();

	GLfloat lw, lh, ratio_w, ratio_h;
	// Reconstructs all lower resolution levels bottom-up fashion
	for (int level = 1; level < levels_count; level++)
	{	  
		lw = floorf(canvas_width / pow(2.0, level));
		lh = floorf(canvas_height / pow(2.0, level));
	  	glViewport(0, 0, lw, lh);
	  	glMatrixMode(GL_PROJECTION);
	  	glLoadIdentity();
	  	gluOrtho2D(0.0,  lw, 0.0, lh);
	  	glMatrixMode(GL_MODELVIEW);
	  	glLoadIdentity();

	  	vertices[1][1] = lh;
	  	vertices[2][0] = lw;
	  	vertices[2][1] = lh;
	  	vertices[3][0] = lw;

		fbo_lod[level]->bind();
		glDrawBuffers(fbo_buffers_count, buffers);

		ratio_w = lw;
		ratio_h = lh;
		lw = floorf(canvas_width / pow(2.0, level-1));
		lh = floorf(canvas_height / pow(2.0, level-1));		
		
		mShaderAnalysis.prog.Bind();
		mShaderAnalysis.prog.Uniform("level", (GLint)level);	  
		mShaderAnalysis.prog.Uniform("offset", (GLfloat)0.25/lw, (GLfloat)0.25/lh);
		mShaderAnalysis.prog.Uniform("level_ratio", (GLfloat)(2.0*ratio_w/lw), (GLfloat)(2.0*ratio_h/lh));

		rasterizePixels();	  
		mShaderAnalysis.prog.Unbind();
		fbo_lod[level]->release();
	}
  
}

/**
 * Push phase.
 * Traverses pyramid from top to bottom,
 * synthesize missing pixels by searching the four closest pixels of level above.
 **/
void PyramidPointRendererBase::rasterizeSynthesisPyramid( void )     
{

  /// render targets (GL_COLOR_ATTACHMENTs)
  GLuint buffers[fbo_buffers_count];
  for (int i = 0; i < fbo_buffers_count; ++i)
	buffers[i] = fbo_buffers[i];

  /// source textures (samplers) that are acessed in shaders (GL_TEXTURE0, GL_TEXTURE1 ...)
  for (int i = 0; i < fbo_buffers_count; ++i)
	activateTexture(i, i);

  mShaderSynthesis.prog.Bind();
  mShaderSynthesis.prog.Uniform("minimum_size", (GLfloat)(minimum_radius_size));
  mShaderSynthesis.prog.Uniform("reconstruction_filter_size", (GLfloat)(reconstruction_filter_size));
  mShaderSynthesis.prog.Uniform("prefilter_size", (GLfloat)(prefilter_size));
  mShaderSynthesis.prog.Uniform("depth_test", depth_test);
  
  for (int i = 0; i < fbo_buffers_count; ++i)
	mShaderSynthesis.prog.Uniform(shader_texture_names[i].c_str(), i);
  mShaderSynthesis.prog.Unbind();

	GLfloat lw, lh, ratio_w, ratio_h;
	for (int level = levels_count - 2; level >= 0; level--)
    {
		lw = floorf(canvas_width / pow(2.0, level));
		lh = floorf(canvas_height / pow(2.0, level));
		glViewport(0, 0, lw, lh);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0,  lw, 0.0, lh);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		/// maximum coordinates for drawing quad
		vertices[1][1] = lh;
		vertices[2][0] = lw;
		vertices[2][1] = lh;
		vertices[3][0] = lw;

		fbo_lod[level]->bind();
		glDrawBuffers(fbo_buffers_count, buffers);

		mShaderSynthesis.prog.Bind();
		mShaderSynthesis.prog.Uniform("level", (GLint)level);
		ratio_w = lw;
		ratio_h = lh;
		lw = floorf(canvas_width / pow(2.0, level+1));
		lh = floorf(canvas_height / pow(2.0, level+1));
		mShaderSynthesis.prog.Uniform("half_pixel_size", (GLfloat)0.5/lw, (GLfloat)0.5/lh);	
		mShaderSynthesis.prog.Uniform("level_ratio", (GLfloat)(0.5*ratio_w/lw), (GLfloat)(0.5*ratio_h/lh));
		mShaderSynthesis.prog.Uniform("canvas_ratio", (GLfloat)lw/lh);

		rasterizePixels();
		mShaderSynthesis.prog.Unbind();
		fbo_lod[level]->release();
    }
}

/**
 * Deferred shading of synthesised base level
 **/
void PyramidPointRendererBase::rasterizePhongShading(void)
{
  int level = 0;

  mShaderPhong.prog.Bind();
  mShaderPhong.prog.Uniform("color_ambient", Mats[material_id][0], Mats[material_id][1], Mats[material_id][2], Mats[material_id][3]);
  mShaderPhong.prog.Uniform("color_diffuse", Mats[material_id][4], Mats[material_id][5], Mats[material_id][6], Mats[material_id][7]);
  mShaderPhong.prog.Uniform("color_specular", Mats[material_id][8], Mats[material_id][9], Mats[material_id][10], Mats[material_id][11]);
  mShaderPhong.prog.Uniform("shininess", Mats[material_id][12]);
  mShaderPhong.prog.Uniform("level", (GLint)level);
  /// samplers, binds normal texture, then binds extra attributes if any starting from third texture (color ...)
  mShaderPhong.prog.Uniform(shader_texture_names[0].c_str(), 0);
  for (int i = 2; i < fbo_buffers_count; ++i)
    mShaderPhong.prog.Uniform(shader_texture_names[i].c_str(), i-1);

  glViewport(0, 0, canvas_width, canvas_height);

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glDrawBuffer(GL_BACK);

  activateTexture(0, 0);
  /// source textures that are acessed in shaders
  for (int i = 2; i < fbo_buffers_count; ++i)
	activateTexture(i, i-1);

  rasterizePixels();

  mShaderPhong.prog.Unbind();

  /// clear
  for (int i = 0; i < fbo_buffers_count; ++i)
	activateTexture(-1, i);
}


/**
 * Clear all framebuffers and screen buffer.
 **/
void PyramidPointRendererBase::clearBuffers( void ) {
  mShaderProjection.prog.Unbind();
  mShaderAnalysis.prog.Unbind();
  mShaderSynthesis.prog.Unbind();
  mShaderPhong.prog.Unbind();

  glEnable(FBO_TYPE);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);

  GLint currentDrawBuffer;
  glGetIntegerv(GL_DRAW_BUFFER, &currentDrawBuffer);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
  
  check_for_ogl_error("before clearing ");

  /// clear all buffers of all fbos
  for (int level = 0; level < levels_count; level++) {
	fbo_lod[level]->bind();
	for (int j = 0; j < fbo_buffers_count; j++) {
	  glDrawBuffer(fbo_buffers[j]);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	checkFramebufferStatus( __func__ );
	check_for_ogl_error("clearing");

	fbo_lod[level]->release();
  }

  /// Clear the back buffer  
  glDrawBuffer(GL_BACK);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  check_for_ogl_error("clear buffers");
  
}

/**
 * Reconstructs the surface for visualization.
 **/
void PyramidPointRendererBase::projectSamples(Object* const obj) {
  // Project points to framebuffer with depth test on.
  projectSurfels( obj );
  check_for_ogl_error("project samples");
}

/**
 * Interpolate projected samples using pyramid interpolation
 * algorithm.
 **/
void PyramidPointRendererBase::interpolate() {
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	/// Set the Ortho once, while viewport is set for each pyramid level render pass
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, canvas_width, 0.0, canvas_height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/// Pull phase - Create pyramid structure
	rasterizeAnalysisPyramid();
	//  check_for_ogl_error("analysis");

	/// Push phase - Interpolate scattered data
	rasterizeSynthesisPyramid();
	// check_for_ogl_error("synthesis");
}

/**
 * Renders reconstructed model on screen with
 * per pixel shading.
 **/
void PyramidPointRendererBase::draw( void ) {

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glDisable(GL_COLOR_MATERIAL);

  /// These parameters are matching the Splatting Plugin for comparison
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, vcg::Point4f(0.3, 0.3, 0.3, 1.).V());
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, vcg::Point4f(0.6, 0.6, 0.6, 1.).V());
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, vcg::Point4f(0.5, 0.5, 0.5, 1.).V());

  ///  Deffered shading of the final image containing normal map
  rasterizePhongShading();

  glDisable(FBO_TYPE);

  check_for_ogl_error("draw");
}


/**
 * Initialize OpenGL state variables.
 **/
void PyramidPointRendererBase::createFBO() {

	assert(fbo_buffers_count <= 16);

	fbo_buffers = new GLuint[fbo_buffers_count];
	fbo_textures = new GLuint[fbo_buffers_count];

	check_for_ogl_error("new arrays fbo");

	/// First create one mipmap texture for each render target
	glGenTextures(fbo_buffers_count, fbo_textures);
	for (int i = 0; i < fbo_buffers_count; i++) {
		fbo_buffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;

		glBindTexture(FBO_TYPE, fbo_textures[i]);
		glTexImage2D(FBO_TYPE, 0, FBO_FORMAT, canvas_width, canvas_height, 0, GL_RGBA, GL_FLOAT, NULL);

		glGenerateMipmapEXT(FBO_TYPE);

		glTexParameteri(FBO_TYPE, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(FBO_TYPE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(FBO_TYPE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(FBO_TYPE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(FBO_TYPE, GL_TEXTURE_BASE_LEVEL, 0 );
		glTexParameteri(FBO_TYPE, GL_TEXTURE_MAX_LEVEL, levels_count );
	}

	check_for_ogl_error("buffers creation");

	/// create a depth buffer:
	glGenTextures(1, &fbo_depth);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo_depth);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, canvas_width, canvas_height);
	check_for_ogl_error("depth buffer creation");

	fbo_lod.resize(levels_count);
	
	check_for_ogl_error("framebuffer creation");

	/// now attach all textures to fbos, each fbo stores one mipmap level of all render targets
	for (int level = 0; level < levels_count; level++) {

		int dim = 1024/pow(2.0, double(level));
		fbo_lod[level] = new QGLFramebufferObject(dim, dim, FBO_TYPE);		
		
		if (!fbo_lod[level]->isValid())
			std::cout << level << " PyramidPointRenderer: invalid FBO\n";	  

		fbo_lod[level]->bind();
		// for each level: attach all render targets to the fbo
		for (int i = 0; i < fbo_buffers_count; i++) {
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, fbo_buffers[i], FBO_TYPE, fbo_textures[i], level);
		}
		check_for_ogl_error("fbo attachment");
		checkFramebufferStatus( __func__ );

		fbo_lod[level]->release();
		check_for_ogl_error("fbo attachment");
	}

	/// And lets also attach the depth buffer to the first fbo, that is, level 0 of the pyramid
	fbo_lod[0]->bind();
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
							   GL_RENDERBUFFER_EXT, fbo_depth);
	check_for_ogl_error("depth attachment");

	fbo_lod[0]->release();

	check_for_ogl_error("fbo_mipmap");
}

QString PyramidPointRendererBase::loadShaderSource(const QString& filename) const {
  
  QString res;
  
  QFile f(shaders_path.absolutePath() + "/shaders/splatpyramid/" + filename);
  
  if (!f.open(QFile::ReadOnly))
	{
	  std::cerr << "failed to load shader file " << filename.toAscii().data() << "\n";	  
	}
  //  else qDebug("Succesfully loaded shader");

  QTextStream stream(&f);
  res = stream.readAll();
  f.close();
  return res;
} 
