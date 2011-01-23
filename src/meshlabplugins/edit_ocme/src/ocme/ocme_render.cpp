#include <GL/glew.h>
#include <wrap/gl/space.h>
#include <wrap/gl/trimesh.h>
#include <wrap/gl/splatting_apss/splatrenderer.h>
#include "impostor_definition.h"
#include "ocme_definition.h"
#include "ocme_extract.h"
#include "ocme_disk_loader.h"
#include <vcg/space/point4.h>


extern unsigned int generic_bool ;
int COff(const int & h);


void DrawCellSel ( CellKey & ck, int mode = 0 )
{
	vcg::Point3f p = ck.P3f();
	int  lev = ck.h;
	float stepf = ( lev>0 ) ? ( 1<<lev ) :  1.f/ ( float ) ( 1<< ( -lev ) ) ;

	vcg::Box3f ubox;
	ubox.min=vcg::Point3f ( -0.5,-0.5,-0.5 );
	ubox.max=-ubox.min;


	vcg::Color4b c;
	c = c.Scatter ( 255,lev+127 );
 	c.ColorRamp(-10.f,  10.f, lev );
	c[3] = 127;
	glColor( c );

	glPushMatrix();

	switch ( mode )
	{
		case 0:	glTranslatef ( ( p[0]+ 0.5 ) *stepf, ( p[1]+ 0.5 ) *stepf, ( p[2]+ 0.5 ) *stepf );
			glScalef ( stepf*0.1 ,stepf*0.1  ,stepf*0.1 );
			vcg::glBoxFlat ( ubox );
			break;
		case 1:;
		case 3: glTranslatef ( ( p[0]+ 0.5 ) *stepf, ( p[1]+ 0.5 ) *stepf, ( p[2]+ 0.5 ) *stepf );
			glScalef ( stepf ,stepf ,stepf );
			vcg::glBoxFlat ( ubox );
			break;
		case 2:	glTranslatef ( ( p[0]+ 0.5 ) *stepf, ( p[1]+ 0.5 ) *stepf, ( p[2]+ 0.5 ) *stepf );
			glScalef ( stepf*0.1 ,stepf*0.1  ,stepf*0.1 );
			GLUquadricObj*  q = gluNewQuadric();
			gluSphere (  q,1.0,10,10 );
			gluDeleteQuadric(q);
			break;
	}

	glPopMatrix();

}

void OCME::InitRender()
	{	
		renderParams.one_level = false; 
		renderParams.only_impostors = true; 
		renderParams.visitOn = true;
		renderParams.render_subcells = false;
		renderParams.level = 0;
		renderParams.memory_limit_in_core = 500 * (1<<20);
		renderParams.memory_limit_video = 150 * (1<<20);

		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
		
	}


bool OCME::IsToRefineScreenErr(Cell * & c, float & prio ){
	vcg::Point4f pos4;
	vcg::Point3f pos3;
	float scale;
	const CellKey & ck = c->key;

	pos3 = ck.BBox3f().Center();
	memcpy(&pos4[0],&pos3[0],3*sizeof(float));pos4[3]=1.0;

	scale = pow(mm.Determinant(),1/(float)3.0);
	pos4 = mm*pos4; // position of cube center in view space

	float  dis0 = sqrt( pos4[0]*pos4[0] + pos4[1]*pos4[1]+ pos4[2]*pos4[2]);// distance viewer - center
	float dis = dis0 - ( ck.BBox3f().Diag()*0.5*scale);				  // distance viewer - closest point

	vcg::Point4f prj = pm * vcg::Point4f(0,ck.BBox3f().Diag()* scale/2,-dis,1.0);
	
	float projerr =0.5*vp[2]*prj[1]/prj[3]; // error on screen
	
	prio = projerr;
	if(projerr<0) return true;
	return (projerr > 50.0);
}


bool OCME::IsInFrustum(Cell * c, vcg::Point3f * f, float &dist, bool bsphere_or_bbox ){
		const vcg::Point3f pos3 = (bsphere_or_bbox)?c->key.BBox3f().Center():c->bbox.bbox3.Center();

		const float radius =  ((bsphere_or_bbox)? c->key.BBox3f().Diag():c->bbox.bbox3.Diag())*0.5f;

		vcg::Point3f n_l = ((f[6]-f[0])^(f[4]-f[0])).Normalize();
		dist = (pos3-f[0])*n_l-radius;
		if(dist > 0)
			return false;

		vcg::Point3f n_r = ((f[5]-f[1])^(f[7]-f[1])).Normalize();
		dist = std::max<float>(dist ,(pos3-f[1])*n_r-radius);

		if(dist > 0)
			return false;

		vcg::Point3f n_t = ((f[7]-f[3])^(f[6]-f[3])).Normalize();
		dist = std::max<float>(dist ,(pos3-f[3])*n_t-radius);

		if(dist > 0)
			return false;

		vcg::Point3f n_b = ((f[4]-f[0])^(f[5]-f[0])).Normalize();
		dist = std::max<float>(dist ,(pos3-f[1])*n_b-radius);

		if(dist > 0)
			return false;

		vcg::Point3f n_n = ((f[3]-f[0])^(f[2]-f[0])).Normalize();
		dist = std::max<float>(dist ,(pos3-f[3])*n_n-radius);

		if(dist > 0)
			return false;

		vcg::Point3f n_f = ((f[6]-f[5])^(f[7]-f[5])).Normalize();
		dist = std::max<float>(dist ,(pos3-f[7])*n_f-radius);

		if(dist > 0)
			return false;

		 
		return true;
}

void OCME::Visit(std::vector<Cell*> &  to_render){
 std::list<Cell*>::iterator ori;
 for(ori = octree_roots.begin(); ori != octree_roots.end(); ++ori)
	 Visit((*ori)->key, to_render);

}
void OCME::Visit(CellKey root, std::vector<Cell*> & to_render){

	float _dist;
	std::vector<Cell*> que;
	que.push_back(GetCell(root,false));
	std::vector<CellKey>   children;
	do{
		Cell * c = que.back();
		que.pop_back();
		
		if(  IsToRefineScreenErr(c,c->rd->priority) && c->impostor->non_empty_children &&  IsInFrustum(c,this->frustum,_dist) ){
			children.clear();
			Children(c->key,children);
			for(unsigned int ci = 0; ci < 8 ; ++ci)
				if(c->impostor->non_empty_children & (1 << ci))// non empty	(save a GetCell ..useless..)	
				{
					Cell * to_add = GetCell(children[ci],false);
					if(!to_add) continue;
					que.push_back(to_add);
					if ( !to_add->IsEmpty())
						if(!to_add->rd->to_render())
							{
								c->rd->to_render = FBool(& to_render_fbool);
								c->rd->to_render = true;
								to_render.push_back(to_add);
							}
				}
		}  
		else
		{
			if(!c->rd->to_render())
			{
				c->rd->to_render = FBool(& to_render_fbool);
				c->rd->to_render = true;
				to_render.push_back(c);
			}
		}  
	} while (!que.empty());
}


void OCME::ConvertFrustumView2World(vcg::Point4f * fv,vcg::Point3f * fw){
	vcg::Point4f p4;
	vcg::Matrix44f mm_inv,pm_inv;
	mm_inv = vcg::Inverse(mm);
	pm_inv = vcg::Inverse(pm);

	//  bring the canonical view frustum in world space
	for(int i = 0 ; i <  8; ++i){
		 p4 =  (mm_inv * pm_inv * fv[i]).HomoNormalize()   ;
		 memcpy( &fw[i][0],  &p4[0], sizeof(float) * 3);
	}
}

void OCME::Select(std::vector<Cell*>  & selected){
		RAssert(MemDbg::CheckHeap(1));
		/* get the viewing parameters */
		glGetFloatv(GL_MODELVIEW_MATRIX,&mm[0][0]);
		vcg::Transpose(mm);

		glGetFloatv(GL_PROJECTION_MATRIX,&pm[0][0]);
		vcg::Transpose(pm);

		glGetIntegerv(GL_VIEWPORT,&vp[0]);
		ConvertFrustumView2World(this->sel_corners,this->sel_frustum);

		std::vector<std::pair<float,Cell*> > selected_dist;
		float dist;
		for(unsigned int i = 0; i < cells_to_render.size(); ++i) 
				if(IsInFrustum(cells_to_render[i],sel_frustum,dist,false))
					selected_dist.push_back(make_pair<float,Cell*>(dist,cells_to_render[i]));
		std::sort(selected_dist.begin(),selected_dist.end());
		for(unsigned int i = 0; i < selected_dist.size();++i)
			selected.push_back(selected_dist[i].second);
}
void OCME::DeSelect(std::vector<Cell*>  & selected){
}

void OCME::Render(int impostorRenderMode){
	bool useSplatting = (impostorRenderMode==0);
	bool useBox = (impostorRenderMode==2);

	render_mutex.lock();
	RAssert(MemDbg::CheckHeap(1));

	vcg::Color4b c;


	/* increase mark */
	++to_render_fbool;

	/* get the viewing paramters */
	glGetFloatv(GL_MODELVIEW_MATRIX,&mm[0][0]);
	vcg::Transpose(mm);

	glGetFloatv(GL_PROJECTION_MATRIX,&pm[0][0]);
	vcg::Transpose(pm);

	glGetIntegerv(GL_VIEWPORT,&vp[0]);

	/* update the frustum in world space */
	ConvertFrustumView2World(corners,this->frustum);

	if(renderParams.visitOn){
		cells_to_render.clear();
		Visit(cells_to_render);
	}
	RAssert(MemDbg::CheckHeap(1));

	if(!renderParams.only_impostors){
		for(unsigned int i = 0; i < cells_to_render.size(); ++i)
			if(cells_to_render[i]->face->Size())
				NeedCell(cells_to_render[i]);

		stat.cells_to_render = cells_to_render.size();

	}

	RAssert(MemDbg::CheckHeap(1));
 	::RemoveDuplicates(cells_to_render);
	std::vector<Impostor*> impostor_to_render;
	std::vector< std::vector<vcg::Point3f> *				>  positions;
	std::vector< std::vector<vcg::Point3f> *				>  normals;
	std::vector< std::vector<vcg::Point3<unsigned char> > *	>  colors;
	std::vector<float>    radiuses;

	for(unsigned int i = 0; i < cells_to_render.size(); ++i)
          if(!cells_to_render[i]->generic_bool()) 
		  { // generic_bool() == the cell is taken in editing
          if(cells_to_render[i]->rd->renderCacheToken && cells_to_render[i]->rd->renderCacheToken->lock() )
		 {
				// c = c.Scatter ( 32,COff(cells_to_render[i]->key.h) );
				//glColor ( c );

				default_renderer.m = &cells_to_render[i]->rd->Mesh();

				if(vcg::tri::HasPerVertexColor(cells_to_render[i]->rd->Mesh()))
	//					default_renderer.Draw<vcg::GLW::DMPoints , vcg::GLW::CMPerVert, vcg::GLW::TMNone>();
					default_renderer.Draw<vcg::GLW::DMFlat , vcg::GLW::CMPerVert, vcg::GLW::TMNone>();
	//				default_renderer.DrawPointsBase<vcg::GLW::NMPerVert,vcg::GLW::CMPerVert>();
				else 
//					default_renderer.Draw<vcg::GLW::DMPoints,vcg::GLW::CMLast,vcg::GLW::TMNone>();
					default_renderer.Draw<vcg::GLW::DMFlat,vcg::GLW::CMNone,vcg::GLW::TMNone>();

				cells_to_render[i]->rd->renderCacheToken->unlock();
			}
		 else
		  {
			  if(!useSplatting){
				  if(useBox)
					  DrawCellSel(cells_to_render[i]->key,3);
				  else
					cells_to_render[i]->impostor->Render( 0);
			  }
			  else{ 
				  // impostor_to_render.push_back(cells_to_render[i]->impostor);
				  positions.push_back(&cells_to_render[i]->impostor->positionsV);
				  normals.push_back(&cells_to_render[i]->impostor->normalsV);
				  colors.push_back(&cells_to_render[i]->impostor->colorsV);
				  radiuses.push_back( CS( cells_to_render[i]->key.h)/Impostor::Gridsize());
			  }
			}
	}
		 
	 //for(std::vector<Impostor *  > :: iterator  i =impostor_to_render.begin(); i != impostor_to_render.end();++i)
		// (*i)->Render(this->renderParams.render_subcells);
	 if(useSplatting)
	 {
		 glPushAttrib(GL_ALL_ATTRIB_BITS);
                 splat_renderer.Render(positions,normals,colors,radiuses,vcg::GLW::CMPerMesh,
                         vcg::GLW::TMNone);
		 glPopAttrib();
	 }
		
	 renderCache.controller.updatePriorities();

	 render_mutex.unlock();
 }


void OCME::NeedCell(Cell * c){
	this->renderCache.Req(c,c->rd->priority);
}

void OCME::StopAndFlushRendering(){
		render_mutex.lock();
		this->cells_to_render.clear();
		this->renderCache.controller.pause();
}
void OCME::StartRendering(){
		render_mutex.unlock();
}
