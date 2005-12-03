#include <iostream>

#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component.h>

#include <vcg/simplex/faceplus/base.h>
#include <vcg/simplex/faceplus/component.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/curvature.h>
#include <vcg/math/histogram.h>

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

class qVertex:	public VertexSimp2<qVertex, CEdge, CFace, vert::Coord3f, vert::Normal3f, vert::Qualityf, vert::Color4b >{};
class CFace:		public FaceSimp2<qVertex,CEdge,CFace, face::VertexRef, face::FFAdj, face::Color4b, face::Normal3f >{};
class MyMesh:		public tri::TriMesh< vector<qVertex>, vector<CFace> >{};

MyMesh mesh;
vcg::GlTrimesh<MyMesh> glWrap;
vcg::Trackball track;
int drawMode = 0;
int width = 320;
int height = 240;

float histo_frac = 0.15f;
int histo_range=1000;

GLhandleARB p;
GLhandleARB v;
GLhandleARB f;

GLint edge;  
GLint phong; 
GLint diffuseColor;
GLint phongColor;

float lpos[4] = {1,0.5,1,0};

static void Gaussian(MyMesh &m);

/*
	code from "trimesh_sdl.cpp" 
*/

void initShader()
{
	GLenum err = glewInit();
	if (GLEW_OK == err)
	{
		v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		char *fs = "uniform vec3 DiffuseColor;uniform vec3 PhongColor;uniform float Edge;uniform float Phong;varying vec3 Normal;void main (void){vec3 color = DiffuseColor;float f = dot(vec3(0,0,1),Normal);if (abs(f) < Edge)color = vec3(0);if (f > Phong)color = PhongColor;gl_FragColor = vec4(color, 1);}";
		char *vs = "varying vec3 Normal;void main(void){Normal = normalize(gl_NormalMatrix * gl_Normal);gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;}";
		const char * vv = vs;
		const char * ff = fs;
		glShaderSourceARB(v, 1, &vv,NULL);
		glShaderSourceARB(f, 1, &ff,NULL);
		
		glCompileShaderARB(v);
		glCompileShaderARB(f);
		p = glCreateProgramObjectARB();
		glAttachObjectARB(p,v);
		glAttachObjectARB(p,f);
		glLinkProgramARB(p);

		edge =				 glGetUniformLocationARB(p, "Edge");
		phong =				 glGetUniformLocationARB(p, "Phong");
		diffuseColor = glGetUniformLocationARB(p, "DiffuseColor");
		phongColor =	 glGetUniformLocationARB(p, "PhongColor");

		cout << "shader active" << endl;
	}	
}


void initGL()
{
  glClearColor(0, 0, 0, 0); 
  glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);	
	initShader();
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
		glClearColor(1.0,1.0,1.0,1.0);
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


		glUseProgramObjectARB(p);
		glUniform1fARB(edge, 0.64f);
		glUniform1fARB(phong, 0.9540001f);
		glUniform3fARB(diffuseColor, 0.0f, 0.25f, 1.0f);
		glUniform3fARB(phongColor, 0.75f, 0.75f, 1.0f);
//		

		// the trimesh drawing calls
		switch(drawMode)
		{
			case 0: glWrap.Draw<vcg::GLW::DMSmooth,		vcg::GLW::CMPerVert,vcg::GLW::TMNone> ();break;
		  case 1: glWrap.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMPerVert,vcg::GLW::TMNone> ();break;
		  case 2: glWrap.Draw<vcg::GLW::DMWire,     vcg::GLW::CMPerVert,vcg::GLW::TMNone> ();break;
		  case 3: glWrap.Draw<vcg::GLW::DMFlatWire, vcg::GLW::CMPerVert,vcg::GLW::TMNone> ();break;
		  case 4: glWrap.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMPerVert,vcg::GLW::TMNone> ();break;
		  case 5: glWrap.Draw<vcg::GLW::DMFlat,     vcg::GLW::CMPerVert,vcg::GLW::TMNone> ();break;
		  default: break;
		}

		glUseProgramObjectARB(0);

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
				
				case SDLK_s: {histo_range+=100; Gaussian(mesh); vcg::tri::UpdateColor<MyMesh>::VertexQuality(mesh); glWrap.Update(); break; }
				case SDLK_x: {histo_range-=100; Gaussian(mesh); vcg::tri::UpdateColor<MyMesh>::VertexQuality(mesh); glWrap.Update(); break; }
				
				case SDLK_d: {histo_frac+=0.001f; Gaussian(mesh); vcg::tri::UpdateColor<MyMesh>::VertexQuality(mesh); glWrap.Update(); break; }
				case SDLK_c: {histo_frac-=0.001f; Gaussian(mesh); vcg::tri::UpdateColor<MyMesh>::VertexQuality(mesh); glWrap.Update(); break; }
				
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
	
	float min, max = 0.0;
	vcg::Histogram<float> histo;

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
			area0 = ( e01*(1.0/tan(angle2)) + e20*(1.0/tan(angle1)) ) /8;
			// voronoi area v[1]
			area1 = ( e01*(1.0/tan(angle2)) + e12*(1.0/tan(angle0)) ) /8;
			// voronoi area v[2]
			area2 = ( e20*(1.0/tan(angle1)) + e20*(1.0/tan(angle0)) ) /8;
			
			(*fi).V(0)->Q()  += area0;
			(*fi).V(1)->Q()  += area1;
			(*fi).V(2)->Q()  += area2;
		}
		else // triangolo ottuso
		{ 
			(*fi).V(0)->Q() += vcg::Area<CFace>((*fi)) / 3.0;
			(*fi).V(1)->Q() += vcg::Area<CFace>((*fi)) / 3.0;
			(*fi).V(2)->Q() += vcg::Area<CFace>((*fi)) / 3.0;      
		}
	}

	i = 0;
	for(vi=m.vert.begin();vi!=m.vert.end();++vi,++i) if(!(*vi).IsD())
	{
		area[i] = (*vi).Q();
		(*vi).Q() = (float)(2.0 * M_PI);
	}
	
	//cout << "Vertex count: " << i << endl;

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
		if(area[i]==0) 
			(*vi).Q() = 0;
		else
			(*vi).Q() /= area[i];
	
		if ((*vi).Q() < min) min = (*vi).Q();
		if ((*vi).Q() > max) max = (*vi).Q();

	}

	//cout << "min:" << min << " max:" << max << endl;

	histo.SetRange(min, max, histo_range);
	
	for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
	{
		histo.Add((*vi).Q());
	} 

	min = histo.Percentile(histo_frac);
	max = histo.Percentile(1.0f-histo_frac);

	cout << "Histo: frac=" << histo_frac << " pmin=" << min << " pmax=" << max << "  range=" << histo_range << endl;
	
	for(vi=m.vert.begin(); vi!=m.vert.end(); ++vi) if(!(*vi).IsD())
	{
		(*vi).Q() = math::Clamp((*vi).Q(), min, max);
	}

	//--- DeInit
	
	delete[] area;

}


int main(int argc, char *argv[]) 
{	

	int res;
	
	cout << "Opening sample mesh...";
	if (argc > 1) 
		res = vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh,argv[1]);
	else 
		res = vcg::tri::io::ImporterPLY<MyMesh>::Open(mesh,"../../sample/screwdriver.ply");

	if (res!=0)
		cout <<  vcg::tri::io::ImporterPLY<MyMesh>::ErrorMsg(res) << endl;
	else 
		cout << "ok" << endl;
	

	vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
	vcg::tri::UpdateNormals<MyMesh>::PerVertex(mesh);
	Gaussian(mesh);	
	vcg::tri::UpdateColor<MyMesh>::VertexQuality(mesh);
	
	glWrap.m = &mesh;
	glWrap.Update();
	
	initSDL("SDL_minimal_viewer");
	initGL();

	sdl_idle();
	
	return 0;

}
