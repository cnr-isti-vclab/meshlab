#include <iostream>

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertex/with/vn.h>
#include <vcg/simplex/vertex/with/afvn.h>
#include <vcg/simplex/face/with/af.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/gl/trimesh.h>
#include <wrap/gui/trackball.h>
#include <vcg/math/base.h>
#include <SDL/SDL.h>
#include <gl/glew.h>

using namespace vcg;
using namespace std;

class CEdge;
class CFace;

//class CVertex : public VertexSimp2<CVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Curvaturef >{};
class qVertex : public VertexSimp2<qVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Qualityf >{};
class CFace: public FaceAF<qVertex,CEdge,CFace>{};
class MyMesh: public tri::TriMesh< vector<qVertex>, vector<CFace> >{};

MyMesh mesh;
vcg::GlTrimesh<MyMesh> glWrap;
vcg::Trackball track;
int drawMode;
int width = 800;
int height = 600;

/*
	code from "trimesh_sdl.cpp" 
*/
void initGL()
{
  glClearColor(0, 0, 0, 0); 
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}
void myReshapeFunc(GLsizei w, GLsizei h)
{
  SDL_SetVideoMode(w, h, 0, SDL_OPENGL|SDL_RESIZABLE|SDL_DOUBLEBUF);
	width=w;
  height=h;
  glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
  initGL();
}

bool initSDL(const std::string &str) {
  /* Initialize SDL for video output */
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }
  if ( SDL_SetVideoMode(width, height, 0, SDL_OPENGL|SDL_RESIZABLE) == NULL ) {
    fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  
  SDL_WM_SetCaption(str.c_str(), str.c_str());  
  myReshapeFunc(width, height);
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

    track.center=Point3f(0, 0, 0);
    track.radius= 1;

		track.GetView();
    track.Apply();
    
    float d=1.0f/mesh.bbox.Diag();
    glScale(d);
		glTranslate(-glWrap.m->bbox.Center());	

		// the trimesh drawing calls
		switch(drawMode)
		{
		  case 0: glWrap.Draw<vcg::GLW::DMSmooth,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
		  case 1: glWrap.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
		  case 2: glWrap.Draw<vcg::GLW::DMWire,     vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
		  case 3: glWrap.Draw<vcg::GLW::DMFlatWire, vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
		  case 4: glWrap.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
		  case 5: glWrap.Draw<vcg::GLW::DMFlat,     vcg::GLW::CMNone,vcg::GLW::TMNone> ();break;
		  default: break;
		}
    SDL_GL_SwapBuffers();
}

int sdl_idle() {
  bool quit=false;
 	SDL_Event event;
	while( !quit ) {  
    SDL_WaitEvent(&event);
    switch( event.type ) {
      case SDL_QUIT:  quit = true; break; 
      case SDL_VIDEORESIZE : 			myReshapeFunc(event.resize.w,event.resize.h); 			break;
      case SDL_KEYDOWN:                                        
			  switch(event.key.keysym.sym) {
			    case SDLK_RCTRL:
			    case SDLK_LCTRL: track.ButtonDown(vcg::Trackball::KEY_CTRL); break;
			    case SDLK_q: exit(0); break;	
			    case SDLK_SPACE: drawMode=((drawMode+1)%6); printf("Current Mode %i\n",drawMode); break;	
			  }  break;
      case SDL_KEYUP: 
			  switch(event.key.keysym.sym) {
			    case SDLK_RCTRL:
			    case SDLK_LCTRL: track.ButtonUp(vcg::Trackball::KEY_CTRL); break;
			  }	break;
      case SDL_MOUSEBUTTONDOWN:   
	      switch(event.button.button) {
          case SDL_BUTTON_WHEELUP:    track.MouseWheel( 1); break;
          case SDL_BUTTON_WHEELDOWN:  track.MouseWheel(-1); break;
          case SDL_BUTTON_LEFT:	      track.MouseDown(event.button.x, (height - event.button.y), vcg::Trackball::BUTTON_LEFT); break;
          case SDL_BUTTON_RIGHT:	    track.MouseDown(event.button.x, (height - event.button.y), vcg::Trackball::BUTTON_RIGHT);break;
        } break;
      case SDL_MOUSEBUTTONUP:          
	      switch(event.button.button) {
          case SDL_BUTTON_LEFT:	      track.MouseUp(event.button.x, (height - event.button.y), vcg::Trackball::BUTTON_LEFT); break;
          case SDL_BUTTON_RIGHT:	    track.MouseUp(event.button.x, (height - event.button.y), vcg::Trackball::BUTTON_RIGHT);break;
        } break;
      case SDL_MOUSEMOTION: 
	      while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_MOUSEMOTIONMASK));
	      track.MouseMove(event.button.x, (height - event.button.y));
	      break;  
      case SDL_VIDEOEXPOSE:
      default: break;
      }
		display();
	}

  SDL_Quit();
  return -1;
}

/*
	end of code from "trimesh_sdl.cpp"
*/

static void Gaussian(MyMesh &m){
	
	assert(m.HasPerVertexQuality());

	MyMesh::VertexIterator vi;		// iteratore vertice
	MyMesh::FaceIterator fi;		// iteratore facce
	double *area;					// areamix vector
	int i;							// index
	double area0, area1, area2;
	double angle0, angle1, angle2; 
	
	//--- Initialization
	area = new double[m.vn];

	//reset the values to 0
	for(vi=m.vert.begin();vi!=m.vert.end();++vi) if(!(*vi).IsD())
		(*vi).Q() = 0.0;

	//--- compute Areamix
	for(fi=m.face.begin();fi!=m.face.end();++fi) if(!(*fi).IsD())
	{
		
		// angles
			 angle0 = math::Abs(Angle(	(*fi).V(1)->P()-(*fi).V(0)->P(),(*fi).V(2)->P()-(*fi).V(0)->P() ));
			 angle1 = math::Abs(Angle(	(*fi).V(0)->P()-(*fi).V(1)->P(),(*fi).V(2)->P()-(*fi).V(1)->P() ));
 			 angle2 = M_PI-(angle0+angle1);
		
		if((angle0 < M_PI/2) || (angle1 < M_PI/2) || (angle2 < M_PI/2))  // triangolo non ottuso
		{ 
			float e01 = SquaredDistance( (*fi).V(1)->P() , (*fi).V(0)->P() );
			float e12 = SquaredDistance( (*fi).V(2)->P() , (*fi).V(1)->P() );
			float e20 = SquaredDistance( (*fi).V(0)->P() , (*fi).V(2)->P() );
			
			// voronoi area v[0]
			area0 = ( e01*(1/tan(angle2)) + e20*(1/tan(angle1)) ) /8;
			// voronoi area v[1]
			area1 = ( e01*(1/tan(angle2)) + e12*(1/tan(angle0)) ) /8;
			// voronoi area v[2]
			area2 = ( e20*(1/tan(angle1)) + e20*(1/tan(angle0)) ) /8;
			
			(*fi).V(0)->Q()  += area0;
			(*fi).V(1)->Q()  += area1;
			(*fi).V(2)->Q()  += area2;
		}
		else // triangolo ottuso
		{  
			(*fi).V(0)->Q() += (*fi).Area() / 3;
			(*fi).V(1)->Q() += (*fi).Area() / 3;
			(*fi).V(2)->Q() += (*fi).Area() / 3;            
		}
	}

	i = 0;
	for(vi=m.vert.begin();vi!=m.vert.end();++vi,++i) if(!(*vi).IsD())
	{
		area[i] = (*vi).Q();
		(*vi).Q() = (float)(2.0 * M_PI);
	}

	for(fi=m.face.begin();fi!=m.face.end();++fi)  if(!(*fi).IsD())
	{
		float angle0 = math::Abs(Angle(
			(*fi).V(1)->P()-(*fi).V(0)->P(),(*fi).V(2)->P()-(*fi).V(0)->P() ));
		float angle1 = math::Abs(Angle(
			(*fi).V(0)->P()-(*fi).V(1)->P(),(*fi).V(2)->P()-(*fi).V(1)->P() ));
		float angle2 = M_PI-(angle0+angle1);
		
		(*fi).V(0)->Q() -= angle0;
		(*fi).V(1)->Q() -= angle1;
		(*fi).V(2)->Q() -= angle2;
	}
	i=0;
	for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi,++i) if(!(*vi).IsD())
	{
		(*vi).Q() /= area[i];
		(*vi).Q()=math::Clamp((*vi).Q(),-0.050f,0.050f);
		
	}
	
	//--- DeInit
	
	delete[] area;

}


int main(int argc, char *argv[]) 
{	
	cout << "Opening sample mesh...";
	int res = vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh,"../../sample/bunny10k.ply");

	if (res!=0)
	{
		cout <<  vcg::tri::io::ImporterPLY<MyMesh>::ErrorMsg(res) << endl;
	} else {
		cout << "ok" << endl;
	}

	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerVertex(mesh);

	glWrap.m = &mesh;
	glWrap.Update();
	
	initSDL("SDL_minimal_viewer");
	initGL();

	sdl_idle();
	
	return 0;

}
