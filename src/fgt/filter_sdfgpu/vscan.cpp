#include <GL/glew.h>
//#include "log.h"

#include "vscan.h"

#include <qdebug.h>


Vscan::Vscan(int _sx, int _sy):sx(_sx),sy(_sy){
  initDone=false;
}

Vscan::~Vscan()
{
    glDeleteFramebuffers(1, (GLuint *)&frameID);
}

static int shaderProgram = -1;

void Vscan::fillDepthBuffer(float* buf) const{
  
  // readback Textures
    
  glBindFramebuffer(GL_FRAMEBUFFER, frameID );
  
  glReadPixels(0,0,sx, sy, GL_DEPTH_COMPONENT, GL_FLOAT, buf);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0 );

}

void useScreenAsDest()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0 );
}

void useDefaultShader()
{
  glUseProgram(0);
}

void setDepthPeelingTolerance(float t){
  glUniform1f( glGetUniformLocation(shaderProgram,"tolerance"), t );
}

void setDepthPeelingSize(const Vscan & scan){
  float f[2];
  f[0] = 1.0f/scan.sizeX();
  f[1] = 1.0f/scan.sizeY();
  glUniform2f( glGetUniformLocation(shaderProgram,"oneOverBufSize"), f[0], f[1] );
}


void Vscan::useAsSource(){
//  glEnable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE0); 
  glBindTexture(GL_TEXTURE_2D, depthID );
  //glActiveTexture(GL_TEXTURE1);
  //glBindTexture(GL_TEXTURE_2D, qualityID );
  
  glActiveTexture(GL_TEXTURE0); 

  //sourceMatr = transform;
}

void Vscan::useAsDest()
{
  if (!initDone) {
    qDebug("useAsDest: init not done!");
    return;
  }
   qDebug("useAsDest: frameID = %d!",frameID);
  glBindFramebuffer(GL_FRAMEBUFFER, frameID );
  glViewport (0, 0, (GLsizei) sx, (GLsizei) sy);

}

void Vscan::splash(bool depth)
{
  if (!initDone) return;
  
  glUseProgram(0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0 );
  //glViewport (0, 0, sx, sy);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glColor3f(1,1,1);
  glBindTexture(GL_TEXTURE_2D, (depth)?depthID:qualityID );
  
  //glDisable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(1,1); glVertex3f(+1,+1,0);
  glTexCoord2f(0,1); glVertex3f(-1,+1,0);
  glTexCoord2f(0,0); glVertex3f(-1,-1,0);
  glTexCoord2f(1,0); glVertex3f(+1,-1,0);
  glEnd();
  
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
}

bool Vscan::init(){
  
  if (initDone) return true;

     qDebug("Initializing FrameBufferObjects \n");
  unsigned int status=12345;
  if (glCheckFramebufferStatus && glBindFramebuffer)
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  switch(status) { 
   case 12345: 
       qDebug("FrameBufferObject Extension not found! [hint: Update drivers] No shadows :(...\n");
      return false; 
   case GL_FRAMEBUFFER_COMPLETE: break;
   case GL_FRAMEBUFFER_UNSUPPORTED:
      qDebug("FrameBufferObject not supported by your card! No shadows :(...\n");
      return false; 
  } 

   // creiamo:
  glGenFramebuffers(1, (GLuint *)&frameID ); // frame buffer
  
  glGenTextures(1, (GLuint *)&qualityID );
  glGenTextures(1, (GLuint *)&depthID );

  {
     glBindFramebuffer(GL_FRAMEBUFFER, frameID );

     const int NTRIES=2;
     unsigned int tryme[NTRIES]={GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT16};
     static GLuint dummydepth=666;
   
     for (int i=0; i<NTRIES; i++) {
      
       // initialize texture
       //glActiveTextureARB(GL_TEXTURE1_ARB); 
       glBindTexture(GL_TEXTURE_2D, qualityID );

       glTexImage2D(GL_TEXTURE_2D, 0, 
         GL_RGBA8, 
         sx, sy, 0,
         GL_RGBA,
         GL_UNSIGNED_BYTE, 
         0
       );
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


       glBindTexture(GL_TEXTURE_2D, depthID );

       glTexImage2D(GL_TEXTURE_2D, 0, 
         tryme[i], 
         sx, sy, 0,
         GL_DEPTH_COMPONENT,
         GL_FLOAT, 
         0
       );
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);


       // attach texture to framebuffer depth and color buffer
       glFramebufferTexture2D(GL_FRAMEBUFFER,
         GL_DEPTH_ATTACHMENT,
         GL_TEXTURE_2D, depthID, 0
       );
  
       glFramebufferTexture2D(GL_FRAMEBUFFER,
         GL_COLOR_ATTACHMENT0,
         GL_TEXTURE_2D, qualityID, 0
       );
      
       if (checkValid()) {
         glBindFramebuffer(GL_FRAMEBUFFER, 0 );
         qDebug("Init try no. %d successful!",i);
         initDone = true;
         break;
       } else qDebug("Init Try no %d Failed",i);
     } // for i

  }

  return initDone;

}

bool Vscan::checkValid()
{ 
  GLenum res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  switch(res) { 
    case GL_FRAMEBUFFER_COMPLETE: return true;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      qDebug("Unsupported FB!");
      return false;  
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      qDebug("Incompl: attachment !");
      return false;  
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      qDebug("Incompl: missing attach  FB!");
      return false;  
    //case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT:  Note this enum was removed by the Revision 117 of the FBO spec.
      qDebug("Incompl: dupicate attach  FB!");
    //  return false;  
    //case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
    //  LogError("Incompl: dimensions!");
    //  return false;
    ///case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
     // LogError("Incompl: formats!");
     // return false;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      qDebug("Incompl: draw buffer!");
      return false;  
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      qDebug("Incompl: read buffer!");
      return false;  
    default:
      qDebug("Unknow FB error!");
      return false;  
  }
} 
