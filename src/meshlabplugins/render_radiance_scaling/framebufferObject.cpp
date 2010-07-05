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
#include "framebufferObject.h"
#include <iostream>
#include <assert.h>

using namespace std;

std::vector<GLenum> _buffers;

FramebufferObject::FramebufferObject()
  : fbo_id(0) {
  glGenFramebuffersEXT(1,&fbo_id);
}

FramebufferObject::~FramebufferObject() {
  glDeleteFramebuffersEXT(1,&fbo_id);
}

void FramebufferObject::attachTexture(GLenum tex_target,GLuint tex_id, 
                                      GLenum attachment,int mip_level,int z_slice) {
  unbindCurrentBindThis();
  
  glBindTexture(tex_target,tex_id);

  if(tex_target==GL_TEXTURE_1D)
    glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT,attachment,
                              GL_TEXTURE_1D,tex_id,mip_level);
  else if(tex_target == GL_TEXTURE_3D)
    glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT,attachment,
                              GL_TEXTURE_3D,tex_id,mip_level,z_slice);
  else
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,attachment,
                              tex_target,tex_id,mip_level);

  unbindThisBindCurrent();
}

void FramebufferObject::attachRenderBuffer(GLuint buff_id,GLenum attachment) {  
  unbindCurrentBindThis();

  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,attachment, 
                               GL_RENDERBUFFER_EXT,buff_id);
  
  unbindThisBindCurrent();
}

void FramebufferObject::unattach(GLenum attachment) {
  unbindCurrentBindThis();

  GLenum type = getAttachedType(attachment);
  switch(type){
  case GL_RENDERBUFFER_EXT:
    attachRenderBuffer(0, attachment);
    break;
  case GL_TEXTURE:
    attachTexture(GL_TEXTURE_2D, 0, attachment);
    break;
  default:
    break;
  }

  unbindThisBindCurrent();
}

void FramebufferObject::unattachAll() {
  int nb_attachments = getMaxColorAttachments();
  for(int i=0;i<nb_attachments;i++)
    unattach(GL_COLOR_ATTACHMENT0_EXT+i);
}

GLint FramebufferObject::getMaxColorAttachments() {
  GLint max_attach = 0;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &max_attach);
  return max_attach;
}

void FramebufferObject::unbindCurrentBindThis() {
  glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT,&current_fbo_id);
  if(fbo_id != (GLuint)current_fbo_id)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo_id);
}

void FramebufferObject::unbindThisBindCurrent() {
  if(fbo_id != (GLuint)current_fbo_id)
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,(GLuint)current_fbo_id);
}

bool FramebufferObject::isValid() {
  unbindCurrentBindThis();

  bool res = false;

  GLenum status;                                            
  status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  switch(status){                                          
  case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
    res = true;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
    cerr << "glift::CheckFramebufferStatus() ERROR:\n\t"
         << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n";
    res = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
    cerr << "glift::CheckFramebufferStatus() ERROR:\n\t"
         << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n";
    res = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    cerr << "glift::CheckFramebufferStatus() ERROR:\n\t"
         << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n";
    res = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    cerr << "glift::CheckFramebufferStatus() ERROR:\n\t"
         << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n";
    res = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
    cerr << "glift::CheckFramebufferStatus() ERROR:\n\t"
         << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n";
    res = false;
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
    cerr << "glift::CheckFramebufferStatus() ERROR:\n\t"
         << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n";
    res = false;
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
    cerr << "glift::CheckFramebufferStatus() ERROR:\n\t"
         << "GL_FRAMEBUFFER_UNSUPPORTED_EXT\n";
    res = false;
    break;
  default:
    cerr << "glift::CheckFramebufferStatus() ERROR:\n\t"
         << "Unknown ERROR\n";
    res = false;
  }

  unbindThisBindCurrent();
  return res;
}

GLenum FramebufferObject::getAttachedType(GLenum attachment) {
  // GL_RENDERBUFFER_EXT or GL_TEXTURE
  unbindCurrentBindThis();

  GLint type = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
                                           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, 
                                           &type);
  unbindThisBindCurrent();

  return GLenum(type);
}

GLuint FramebufferObject::getAttachedId(GLenum attachment) {
  unbindCurrentBindThis();

  GLint id = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
                                           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
                                           &id);
  unbindThisBindCurrent();

  return GLuint(id);
}

GLint FramebufferObject::getAttachedMipLevel(GLenum attachment) {
  unbindCurrentBindThis();

  GLint level = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
                                           GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT, 
                                           &level);
  unbindThisBindCurrent();

  return level;
}

GLint FramebufferObject::getAttachedCubeFace(GLenum attachment) {
  unbindCurrentBindThis();

  GLint level = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
                                           GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT,
                                           &level);
  unbindThisBindCurrent();

  return level;
}

GLint FramebufferObject::getAttachedZSlice(GLenum attachment) {
  unbindCurrentBindThis();

  GLint slice = 0;
  glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, attachment,
                                           GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT,
                                           &slice);
  unbindThisBindCurrent();

  return slice;
}

GLenum *FramebufferObject::buffers(unsigned int i) {
  if(_buffers.empty()) {
    for(int j=0;j<getMaxColorAttachments();++j) {
      _buffers.push_back(GL_COLOR_ATTACHMENT0_EXT+j);
    }
  }
    
  assert((int)i<getMaxColorAttachments());
    
  return &(_buffers[i]);
}
  
