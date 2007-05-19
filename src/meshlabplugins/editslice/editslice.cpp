#include <QtGui>
#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "editslice.h"

#include <wrap/gl/pick.h>
#include <vcg/complex/trimesh/create/platonic.h>

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>
#include <vcg/space/index/grid_closest.h>
#include <vcg/complex/intersection.h>
#include <wrap/gl/space.h>
#include <wrap/gui/trackball.h>
#include <qfiledialog.h>
#include<limits>
#include <vcg/complex/edgemesh/update/bounding.h>
#include <wrap/io_trimesh/import.h>
#include <wrap/io_edgemesh/export_svg.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
using namespace vcg;




ExtraMeshSlidePlugin::ExtraMeshSlidePlugin() {
 first=false;
 isDragging=false;
 QAction* editslice = new QAction(QIcon(":/images/iconslice.png"),"Slice mesh geometry", this);
 editslice->setShortcut(Qt::Key_Escape);
 actionList << editslice;
 QAction *editAction;
 trackball_slice.center=Point3f(0, 0, 0);
 trackball_slice.radius= 50;
  
  foreach(editAction, actionList)
    editAction->setCheckable(true);
 
}

ExtraMeshSlidePlugin::~ExtraMeshSlidePlugin() {

	
}
QList<QAction *> ExtraMeshSlidePlugin::actions() const {
	return actionList;
}
 Trackball::Button QT2VCG(Qt::MouseButton qtbt,  Qt::KeyboardModifiers modifiers)
{

	int vcgbt=Trackball::BUTTON_NONE;
	if(qtbt & Qt::LeftButton		) vcgbt |= Trackball::BUTTON_LEFT;
	if(qtbt & Qt::RightButton		) vcgbt |= Trackball::BUTTON_RIGHT;
	if(qtbt & Qt::MidButton			) vcgbt |= Trackball::BUTTON_MIDDLE;
	if(modifiers & Qt::ShiftModifier   ) vcgbt |= Trackball::KEY_SHIFT;
	if(modifiers & Qt::ControlModifier ) vcgbt |= Trackball::KEY_CTRL;
	if(modifiers & Qt::AltModifier     ) vcgbt |= Trackball::KEY_ALT;
	
	return Trackball::Button(vcgbt);
}

const QString ExtraMeshSlidePlugin::Info(QAction *action) 
{
  if( action->text() != tr("Slice mesh geometry") ) assert (0);

	return tr("Interactive selection of plane intersec with mesh");
}
const PluginInfo &ExtraMeshSlidePlugin::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
	 ai.Version = tr("0.1");
	 ai.Author = ("Nicola Andrenucci");
   return ai;
 } 


void ExtraMeshSlidePlugin::restoreDefault(){
  trackball_slice.Reset();
}
 void ExtraMeshSlidePlugin::mousePressEvent    (QAction *, QMouseEvent * e, MeshModel &m, GLArea * gla)
 {   isDragging = true;
     disableTransision=true; //diable transition for main trackball
	 e->accept();

	  trackball_slice.ButtonUp(QT2VCG(Qt::NoButton,  Qt::ControlModifier ) ); 
      trackball_slice.ButtonUp(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
	  trackball_slice.ButtonUp(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
	 
	  /*if (((e->modifiers() & Qt::ShiftModifier) && (e->modifiers() & Qt::ControlModifier) && 
		  (e->button()==Qt::LeftButton))){
	  
	  }*/

	  if (((e->modifiers() & Qt::ControlModifier) &&
		    (e->button()==Qt::LeftButton)&&
			dialogsliceobj->getDefaultTrackball()
			)){ 
	    disableTransision=false;
		
	  }

	  if (((e->modifiers() & Qt::ShiftModifier) &&
		    (e->button()==Qt::LeftButton)&&
			dialogsliceobj->getDefaultTrackball()
			)){ 
          
	      disableTransision=false;
	  }
     if(dialogsliceobj->getDefaultTrackball()&&disableTransision)
          gla->trackball.MouseDown(e->x(),(gla->height()-e->y()),QT2VCG(e->button(), e->modifiers()) );
	  else if(!dialogsliceobj->getDefaultTrackball())
		  trackball_slice.MouseDown(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()) );
  
	 gla->update();
 }
 void ExtraMeshSlidePlugin::mouseMoveEvent     (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
 {
	isDragging = true;
	if(e->buttons()| Qt::LeftButton) 
	{
		if(dialogsliceobj->getDefaultTrackball())
			{
			  gla->trackball.MouseMove(e->x(),gla->height()-e->y());
			}
		    else
				trackball_slice.MouseMove(e->x(),gla->height()-e->y());
	}
	gla->update();
 }
 void ExtraMeshSlidePlugin::RestoreDefault(){
           trackball_slice.Reset();
		   gla->trackball.Reset();
		   gla->update();
 }
 void ExtraMeshSlidePlugin::SlotExportButton(){
	 
	 fileName = QFileDialog::getSaveFileName(gla->window(), tr("Save polyline File"),"/",tr("Mesh (*.svg)"));
	 Matrix44f mat_trac_rotation ; 
	 trackball_slice.track.rot.ToMatrix( mat_trac_rotation ); //Matrice di rotazione della trackball dei piani
	
	 
	 Point3f* dir=new Point3f(0,1,0);  
	 (*dir)= mat_trac_rotation * (*dir);
     Point3f translation_plains=trackball_slice.track.tra;  //vettore di translazione dei piani
	 
	 double avg_length;  //lunghezza media edge
	 TriMeshGrid mesh_grid;
	 
	 /* Tentativo di rotazione della mesh
	 CMeshO* mes= new CMeshO(m.cm);
     *mes=m.cm;
	  vcg::tri::UpdatePosition<CMeshO>::Matrix(*mes,mat_trac_rotation );
	 vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(*mes);
	 mesh_grid.Set(mes->face.begin(), mes->face.end());
	 */
	 mesh_grid.Set(m.cm.face.begin() ,m.cm.face.end());
	 std::vector<TriMeshGrid::Cell *> intersected_cells;
	 n_EdgeMesh edge_mesh;
     n_Mesh trimesh;
	 Plane3f p=*plains.begin();
	 p.SetDirection(*dir);
	 p.Normalize();
	 p.SetOffset(p.Offset()+ translation_plains[0]);
	 vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(p , edge_mesh, avg_length, &mesh_grid, intersected_cells);
     vcg::edge::UpdateBounding<n_EdgeMesh>::Box(edge_mesh);
	 
	 QByteArray fn = fileName.toLatin1();
	 vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(&edge_mesh, fn.data() );
	
 }

 void ExtraMeshSlidePlugin::mouseReleaseEvent  (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
 {
	  isDragging = true;
	  if(dialogsliceobj->getDefaultTrackball())
		 gla->trackball.MouseUp(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()));
	  else trackball_slice.MouseUp(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()));
}
 void ExtraMeshSlidePlugin::Decorate(QAction * ac, MeshModel &m, GLArea * gla)
 {   
	 
	 this->gla=gla;
	 this->m=m;
	 if(!gla->isEnabled()){
	    dialogsliceobj->close();
	 }
	 DrawPlane(this->gla,this->m);
   
 }
 void ExtraMeshSlidePlugin::EndEdit(QAction * , MeshModel &m, GLArea *gla ){
   dialogsliceobj->close();
 }
 void ExtraMeshSlidePlugin::StartEdit(QAction * , MeshModel &m, GLArea *gla ){
	 
	 if(!first){
		 dialogsliceobj=new dialogslice(gla->window());
		 
		
	 first=true;}
	 dialogsliceobj->show();
	 this->m=m;
	
	 
	 QObject::connect(dialogsliceobj, SIGNAL(exportMesh()), this,SLOT(SlotExportButton()));
     QObject::connect(dialogsliceobj, SIGNAL(Update_glArea()), this, SLOT(upGlA()));
 }
 void ExtraMeshSlidePlugin::upGlA(){
 
	 gla->update();
	 
 }
 
 void ExtraMeshSlidePlugin::DrawPlane(GLArea * gla, MeshModel &m){
     
	b=m.cm.bbox; //Boundig Box
	Point3f mi=b.min;
	Point3f ma=b.max;
	Point3f centre=b.Center();
	float edgeMax=0;
    float LX= ma[0]-mi[0];
	float LY= ma[1]-mi[1];
	float LZ= ma[2]-mi[2];
	edgeMax= max(LX, LY);
	edgeMax=max(edgeMax, LZ); //edgeMax è il lato maggiore della BBox
	dialogsliceobj->setDistanceRange(edgeMax);
	glPushMatrix();
	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_LIGHTING_BIT);
        trackball_slice.GetView();
		trackball_slice.Apply(true);
		trackball_slice.center=centre;
		trackball_slice.radius=edgeMax;
        glColor4f(1.0,0.0,0.0,0.8);
	    int plane=1;  //number of planis, defult=1

 if(dialogsliceobj!=0) plane=dialogsliceobj->getPlaneNumber();
  glEnable(GL_BLEND); 
  glEnable(GL_COLOR_MATERIAL);
  float layer=(float)LX /(float)(plane+1);
  dialogsliceobj->setDefaultDistance(layer);
  for(int i=1; i<=(plane); i++){
	  int in_ass;
	  if(dialogsliceobj->getdistanceDefault())in_ass=0;
		else in_ass=2;
		
		glEnable(GL_CULL_FACE);
		glColor4f(0,1,0,0.5);
		glBegin(GL_QUADS);
			glNormal3f(1,0,0);
			Plane3f* p=new Plane3f();
			float assi_x[4];
			assi_x[0]=mi[0]+(layer*i);
			assi_x[1]=mi[0]+(layer*((plane+1)-i));
			assi_x[2]=centre[0]-((dialogsliceobj->getDistance()*(plane+1))/2)+(dialogsliceobj->getDistance()*i);
			assi_x[3]=centre[0]-((dialogsliceobj->getDistance()*(plane+1))/2)+(dialogsliceobj->getDistance()*((plane+1)-i));	
			p->Init( Point3f (assi_x[0], centre[1]-edgeMax, centre[2]-edgeMax),
                Point3f( assi_x[0], centre[1]+edgeMax, centre[2]-edgeMax),
                Point3f( assi_x[0], centre[1]+edgeMax, centre[2]+edgeMax));

			glVertex3f(assi_x[in_ass], centre[1]-edgeMax, centre[2]-edgeMax);
			glVertex3f(assi_x[in_ass], centre[1]+edgeMax, centre[2]-edgeMax);
			glVertex3f(assi_x[in_ass], centre[1]+edgeMax, centre[2]+edgeMax);
			glVertex3f(assi_x[in_ass], centre[1]-edgeMax, centre[2]+edgeMax);
		glEnd();
		this->plains.insert(plains.begin()+(i-1) , *p);
		glColor4f(1,0,0,0.5);
		glBegin(GL_QUADS);
        glNormal3f(-1,0,0);
        glVertex3f(assi_x[in_ass+1], centre[1]-edgeMax, centre[2]-edgeMax);
        glVertex3f(assi_x[in_ass+1], centre[1]-edgeMax, centre[2]+edgeMax);
		glVertex3f(assi_x[in_ass+1], centre[1]+edgeMax, centre[2]+edgeMax);
		glVertex3f(assi_x[in_ass+1], centre[1]+edgeMax, centre[2]-edgeMax);
		glEnd();
	    }
      glPopAttrib();
      glPopMatrix();
 if(isDragging){
		 isDragging=false;
		 gla->update();}
 }


 
 Q_EXPORT_PLUGIN(ExtraMeshSlidePlugin)
