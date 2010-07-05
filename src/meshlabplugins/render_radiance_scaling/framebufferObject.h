/****************************************************************************
* Render Radiance Scaling                                                   *
* Meshlab's plugin                                                          *
*                                                                           *
* Copyright(C) 2010                                                         *
* Vergne Romain, Dumas Olivier                                              *
* INRIA - Institut Nationnal de Recherche en Informatique et Automatique    *
*                                                                           *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
#ifndef FRAMEBUFFER_OBJECT
#define FRAMEBUFFER_OBJECT

#include <GL/glew.h>
#include <vector> 

class FramebufferObject {
 public:
  FramebufferObject();
  ~FramebufferObject();
  
  inline void bind();
  inline static void unbind();

  void attachTexture(GLenum tex_target, 
                     GLuint tex_id,
                     GLenum attachment = GL_COLOR_ATTACHMENT0_EXT,
                     int mip_level     = 0,
                     int z_slice       = 0);

  void attachRenderBuffer(GLuint rb_id,
                          GLenum attachment = GL_COLOR_ATTACHMENT0_EXT);
  
  void unattach(GLenum attachment);
  void unattachAll();

  bool isValid();

  GLenum getAttachedType(GLenum attachment);
  GLuint getAttachedId(GLenum attachment);
  GLint  getAttachedMipLevel(GLenum attachment);
  GLint  getAttachedCubeFace(GLenum attachment);
  GLint  getAttachedZSlice(GLenum attachment);
  
  static int getMaxColorAttachments();
  static GLenum *buffers(unsigned int i=0);

 private:
  void  unbindCurrentBindThis();
  void  unbindThisBindCurrent();

  GLuint fbo_id;
  GLint  current_fbo_id; 
};

inline void FramebufferObject::bind() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo_id);
}

inline void FramebufferObject::unbind() {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
 

#endif //  FRAMEBUFFER_OBJECT
