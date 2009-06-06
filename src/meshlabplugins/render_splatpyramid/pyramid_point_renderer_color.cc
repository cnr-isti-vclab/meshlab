/*
** pyramid_point_render.cc Pyramid Point Based Rendering.
**
**
**   history:	created  02-Jul-07
*/

#include "pyramid_point_renderer_color.h"

/**
 * Default constructor.
 **/
PyramidPointRendererColor::PyramidPointRendererColor(int w, int h) : PyramidPointRendererBase(w, h, 3) {
  quality_per_vertex = 0;
}

void PyramidPointRendererColor::createShaders ( void ) {

  // Store texture names to be passed as uniforms
  shader_texture_names = new string[fbo_buffers_count];
  shader_texture_names[0] = "textureA";
  shader_texture_names[1] = "textureB";
  shader_texture_names[2] = "textureC";

  bool link;

  mShaderProjection.SetSources(loadShaderSource("shader_point_projection_color.vert").toAscii().data(), loadShaderSource("shader_point_projection_color.frag").toAscii().data());
  link = mShaderProjection.prog.Link();

  std::string compileinfo = mShaderProjection.vshd.InfoLog();  
  std::cout << "Proj vert shader info : " << compileinfo << "\n";
  assert (link == 1);

  mShaderAnalysis.SetSources(loadShaderSource("shader_analysis_color.vert").toAscii().data(), loadShaderSource("shader_analysis_color.frag").toAscii().data());
  link = mShaderAnalysis.prog.Link();

  compileinfo = mShaderAnalysis.fshd.InfoLog();  
  std::cout << "Analysis frag shader info : " << compileinfo << "\n";
  assert (link == 1);

  mShaderSynthesis.SetSources(loadShaderSource("shader_synthesis_color.vert").toAscii().data(), loadShaderSource("shader_synthesis_color.frag").toAscii().data());
  link = mShaderSynthesis.prog.Link();

  compileinfo = mShaderSynthesis.fshd.InfoLog();  
  std::cout << "Synth Frag shader info : " << compileinfo << "\n";
  assert (link == 1);

  mShaderPhong.SetSources(loadShaderSource("shader_phong_color.vert").toAscii().data(), loadShaderSource("shader_phong_color.frag").toAscii().data());
  link = mShaderPhong.prog.Link();

  compileinfo = mShaderPhong.fshd.InfoLog();  
  std::cout << "Phong Frag shader info : " << compileinfo << "\n";
  assert (link == 1);

  check_for_ogl_error("shaders loading");

}
