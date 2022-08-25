/*
** pyramid_point_renderer_base.h Pyramid Point Based Rendering Base header.
**
**
**   history:	created  02-Jul-07
*/


#ifndef __PYRAMID_POINT_RENDERER_BASE_H__
#define __PYRAMID_POINT_RENDERER_BASE_H__

#include <cmath>
#include <cassert>

#include "point_based_renderer.h"
#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QGLFramebufferObject>

#define FBO_TYPE GL_TEXTURE_2D
#define FBO_FORMAT GL_RGBA32F

/**
 * Pyramid point renderer algorithm as described in: <br>
 * Efficient Point-Based Rendering Using Image Reconstruction. <br>
 * Ricardo Marroquim, Martin Kraus, Paulo Roma Cavalcanti <br>
 * IEEE/Eurographics Symposium on Point-Based Graphics (PBG), Sep-2007
 **/
class PyramidPointRendererBase : public PointBasedRenderer
{
 private:
	virtual void rasterizeAnalysisPyramid( void );
	virtual void rasterizeSynthesisPyramid( void );
	virtual void rasterizePhongShading(void);

 protected:

  	void createFBO();

	void projectSurfels( const Object * const );

	const void activateTexture(const int text_id, const int target_id);

	const void rasterizePixels(void);

	QString loadShaderSource(const QString& file) const;

	void resetPointers ( void ) {   
		fbo_buffers = NULL;
		fbo_textures = NULL;
		shader_texture_names = NULL;
	}

 public:
	PyramidPointRendererBase();
	PyramidPointRendererBase(int w, int h);
	PyramidPointRendererBase(int w, int h, int fbos);
	~PyramidPointRendererBase();

	void init ( void );

	virtual void createShaders ( void ) = 0;

	void draw();
	void clearBuffers (void);
	void projectSamples (Object* const obj );
	void interpolate ( void );
	
	void setShadersDir ( QDir d) { shaders_path = d; }

	protected:
	/// Number of frame buffer object attachments.
	int fbo_buffers_count;

	/// Shaders using VCG lib
	ProgramVF mShaderProjection;
	ProgramVF mShaderAnalysis;
	ProgramVF mShaderSynthesis;
	ProgramVF mShaderPhong;

	/// Textures names to pass as uniform to shaders
	string *shader_texture_names;

	/// The application-created framebuffer object.
	vector<QGLFramebufferObject*> fbo_lod;

	/// Framebuffer for depth test.
	GLuint fbo_depth;

	/// usually fboBuffers[i] == GL_COLOR_ATTACHMENT0_EXT + i, 
	/// but we don't rely on this assumption
	GLuint* fbo_buffers;

	/** Textures bound to the framebuffer object; 
	* the ping-pong rendering switches between pairs 0-2 and 1-3
	* because this can be easily generalized to triples 0-2-4 and 1-3-5 etc.
	* (pairs 0-1 and 2-3 would have to be rearranged to 0-1-2 and 3-4-5).
	* use getTextureOfBuffer to find the texture name of a buffer
	**/
	GLuint *fbo_textures;

	/// Number of pyramid levels.
	int levels_count;

	/// Current rasterize level
	int cur_level;

	GLfloat vertices[4][2];
	GLfloat texcoors0[4][2];

	/// Path to meshlab's shaders directory
	QDir shaders_path;
  

};

#endif
