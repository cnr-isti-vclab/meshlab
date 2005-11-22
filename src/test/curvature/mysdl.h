/****************************************************************************
 * VCGLib                                                            o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004                                                \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
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
/****************************************************************************
  History

$Log$
Revision 1.2  2005/11/22 17:47:54  vannini
fixato caricamento della mesh

Revision 1.1  2005/09/21 10:29:33  cignoni
Initial Relase

Revision 1.1  2005/03/15 07:00:54  ganovell
*** empty log message ***


****************************************************************************/

#include <iostream>
#include <fstream>


#include <SDL/SDL.h>

#include <gl/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>


#ifdef _SHOW_A_MESH
// the trimesh drawer
#include "mesh_type.h"
#include <wrap/gl/trimesh.h>
#endif //_SHOW_A_MESH

#include <wrap/gui/trackball.h>

bool fullscreen = false, quit = false,keepdrawing = true;
int width =1024;
int height = 768;
int x,y;

vcg::GlTrimesh<MyMesh> glWrap;
int drawMode;
int keypress;
int pic_x,pic_y,doPick;
float sc;
MyMesh::FaceType * Pick(int x, int y, int width=4, int height=4);

SDL_Surface *screen = NULL;
vcg::Trackball track;

void gl_print(float x, float y, char *str);
bool init(const std::string &str) {
  
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    return false;
  }

  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  int bpp = info->vfmt->BitsPerPixel;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  int flags = SDL_OPENGL;
  if(fullscreen) 
    flags |= SDL_FULLSCREEN;

  screen = SDL_SetVideoMode(width, height, bpp, flags);
  if(!screen) {
    return false;
  }
  
  SDL_WM_SetIcon(SDL_LoadBMP("inspector.bmp"), NULL);
  SDL_WM_SetCaption(str.c_str(), str.c_str());


  glDisable(GL_DITHER);
  glShadeModel(GL_SMOOTH);
  glHint( GL_FOG_HINT, GL_NICEST );
  glEnable(GL_DEPTH_TEST);
  glDepthFunc( GL_LEQUAL );
  glDisable(GL_LIGHTING); 

  glEnableClientState(GL_VERTEX_ARRAY);

	#ifdef	_SHOW_A_MESH
				glWrap.Update();
	#endif
  return true;
}


void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, width/(float)height, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,5,   0,0,0,   0,1,0);    
    
    glViewport(0,0,width,height);

		track.GetView();
    track.Apply();
    track.Draw();

		
#ifdef _SHOW_A_MESH			
				glScalef(1-sc,1-sc,1-sc);
				glScalef(1/glWrap.m->bbox.Diag(),1/glWrap.m->bbox.Diag(),1/glWrap.m->bbox.Diag());
				glTranslate(-glWrap.m->bbox.Center());	

				// to do some picking
				MyMesh::FaceType* fp=NULL;
					if(doPick)
					{
						fp = Pick(pic_x,pic_y,1,1);
						doPick=false;
					}
				
				// the trimesh drawing calls
				switch(drawMode)
				{
				case 0: glWrap.Draw<vcg::GLW::DMSmooth,vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
				case 1: glWrap.Draw<vcg::GLW::DMPoints,vcg::GLW::CMNone,vcg::GLW::TMNone> (); break;
				case 2: glWrap.Draw<vcg::GLW::DMWire,vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
				case 3: glWrap.Draw<vcg::GLW::DMFlatWire,vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
				case 4: glWrap.Draw<vcg::GLW::DMHidden,vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
				case 5: glWrap.Draw<vcg::GLW::DMFlat,vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
				case 6: break;
				}
#endif

//    glScalef(scale, scale, scale);       
 //   Point3f center = sphere.Center();
 //   glTranslatef(-center[0], -center[1], -center[2]);
    
    SDL_GL_SwapBuffers();
}


int sdl_idle() {
 
	SDL_Event event;
	while( !quit ) {           
    unsigned int anything = SDL_PollEvent(&event);      
    if(!anything && !keepdrawing) {
      SDL_WaitEvent(&event);
      anything = true;
    }
    if(anything) {        
      switch( event.type ) {
      case SDL_QUIT:  quit = 1; break;      
      case SDL_KEYDOWN:                                        
			switch(event.key.keysym.sym) {
			case SDLK_RCTRL:
			case SDLK_LCTRL: track.ButtonDown(vcg::Trackball::KEY_CTRL); break;
			case SDLK_q: exit(0); break;	
			}
	break;
      case SDL_KEYUP: 
			switch(event.key.keysym.sym) {
			case SDLK_RCTRL:
			case SDLK_LCTRL:
				track.ButtonUp(vcg::Trackball::KEY_CTRL); break;
			}
	break;
      case SDL_MOUSEBUTTONDOWN:   
	x = event.button.x;
	y = height - event.button.y;

#ifdef SDL_BUTTON_WHEELUP
	if(event.button.button == SDL_BUTTON_WHEELUP) 
	  track.MouseWheel(1);
	else if(event.button.button == SDL_BUTTON_WHEELDOWN) 
	  track.MouseWheel(-1);
	else 
#endif

	  if(event.button.button == SDL_BUTTON_LEFT)
	    track.MouseDown(x, y, vcg::Trackball::BUTTON_LEFT);
	  else if(event.button.button == SDL_BUTTON_RIGHT)
	    track.MouseDown(x, y, vcg::Trackball::BUTTON_RIGHT);
	break;
      case SDL_MOUSEBUTTONUP:          
	x = event.button.x;
	y = height - event.button.y; 
	if(event.button.button == SDL_BUTTON_LEFT)
	  track.MouseUp(x, y, vcg::Trackball::BUTTON_LEFT);
	else if(event.button.button == SDL_BUTTON_RIGHT)
	  track.MouseUp(x, y, vcg::Trackball::BUTTON_RIGHT);     
	break;
      case SDL_MOUSEMOTION: 
	while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_MOUSEMOTIONMASK));
	x = event.motion.x;
	y = height - event.motion.y;
	track.MouseMove(x, y);
	break;  
      case SDL_VIDEOEXPOSE:
      default: break;
      }
    }
						                                                                    
		display();
	}

  SDL_Quit();
  return -1;
}

void gl_print(float x, float y, char *str) {
  glRasterPos2f(x, y);
  int len = (int)strlen(str);
  for(int i = 0; i < len; i++) 
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

#ifdef _SHOW_A_MESH
// you can use this funcion replacing the face with your own pickable primitive
 MyMesh::FaceType * Pick(int x, int y,int w,int h)
{
	long hits;	
  int sz=glWrap.m->face.size()*5;
	unsigned int *selectBuf =new unsigned int[sz];
	//  static unsigned int selectBuf[16384];
  glSelectBuffer(sz, selectBuf);
  glRenderMode(GL_SELECT);
  glInitNames();

  /* Because LoadName() won't work with no names on the stack */
  glPushName(-1);
	double mp[16];
	
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport);
	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX ,mp);
	glPushMatrix();
  glLoadIdentity();
  //gluPickMatrix(x, viewport[3]-y, 4, 4, viewport);
  gluPickMatrix(x, viewport[3]-y, width, height, viewport);
	glMultMatrixd(mp);

	glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  int tricnt=0; 

	MyMesh::FaceIterator ti;
	for(ti=glWrap.m->face.begin();ti!=glWrap.m->face.end();++ti)
	{
		if(!(*ti).IsD())
		{
			glLoadName(tricnt);

			glBegin(GL_TRIANGLES);
				glVertex((*ti).P(0));
				glVertex((*ti).P(1));
				glVertex((*ti).P(2));
			glEnd();
		}
		tricnt++;
	}

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
	glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  hits = glRenderMode(GL_RENDER);
	//xstring buf;
	//if (hits <= 0)     return 0;
	std::vector< std::pair<double,int> > H;
	for(int ii=0;ii<hits;ii++){
		//TRACE("%ui %ui %ui %ui\n",selectBuf[ii*4],selectBuf[ii*4+1],selectBuf[ii*4+2],selectBuf[ii*4+3]);
		H.push_back( std::pair<double,int>(selectBuf[ii*4+1]/4294967295.0,selectBuf[ii*4+3]));
	}
	if(H.empty()) return NULL;
	std::sort(H.begin(),H.end());
	
	return (MyMesh::FaceType *)&glWrap.m->face[H[0].second];

}
#endif //_SHOW_A_MESH
