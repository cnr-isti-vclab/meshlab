/*
** pyramid_point_render.cc Pyramid Point Based Rendering.
**
**
**   history:	created  02-Jul-07
*/

#include "pyramid_point_renderer.h"


/**
 * Default constructor.
 **/
PyramidPointRenderer::PyramidPointRenderer(int w, int h) : PyramidPointRendererBase(w, h, 2) {
}

/**
 * Installs the shaders using the GLSL Kernel class.
 **/
void PyramidPointRenderer::createShaders ( void ) {

	// Store texture names to be passed as uniforms
	shader_texture_names = new string[fbo_buffers_count];
	shader_texture_names[0] = "textureA";
	shader_texture_names[1] = "textureB";

	bool link;

	mShaderProjection.SetSources(loadShaderSource("shader_point_projection.vert").toAscii().data(), loadShaderSource("shader_point_projection.frag").toAscii().data());
	link = mShaderProjection.prog.Link();

	std::string compileinfo = mShaderProjection.fshd.InfoLog();  
	std::cout << "Projection Frag shader info : " << compileinfo << "\n";
	assert (link == 1);

	mShaderAnalysis.SetSources(loadShaderSource("shader_analysis.vert").toAscii().data(), loadShaderSource("shader_analysis.frag").toAscii().data());
	link = mShaderAnalysis.prog.Link();

	compileinfo = mShaderAnalysis.fshd.InfoLog();  
	std::cout << "Analysis Frag shader info : " << compileinfo << "\n";
	assert (link == 1);

	mShaderSynthesis.SetSources(loadShaderSource("shader_synthesis.vert").toAscii().data(), loadShaderSource("shader_synthesis.frag").toAscii().data());
	link = mShaderSynthesis.prog.Link();

	compileinfo = mShaderSynthesis.fshd.InfoLog();  
	std::cout << "Synthesis Frag shader info : " << compileinfo << "\n";
	assert (link == 1);

	mShaderPhong.SetSources(loadShaderSource("shader_phong.vert").toAscii().data(), loadShaderSource("shader_phong.frag").toAscii().data());
	link = mShaderPhong.prog.Link();

	compileinfo = mShaderPhong.fshd.InfoLog();  
	std::cout << "Phong Frag shader info : " << compileinfo << "\n";
	assert (link == 1);

	check_for_ogl_error("shaders loading");
}
