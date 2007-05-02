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
using namespace vcg;

ExtraMeshSlidePlugin::ExtraMeshSlidePlugin() {
 first=false;
 isDragging=false;
 actionList << new QAction(QIcon(":/images/iconslice.png"),"Slice mesh geometry", this);
  QAction *editAction;
  trackball_slice.center=Point3f(0, 0, 0);
  trackball_slice.radius= 50;
  foreach(editAction, actionList)
    editAction->setCheckable(true);
 
}
ExtraMeshSlidePlugin::~ExtraMeshSlidePlugin() {

	delete slicedialog;
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
	 //e->accept();

      trackball_slice.ButtonUp(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
      trackball_slice.ButtonUp(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
	  trackball_slice.ButtonUp(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
	  if (((e->modifiers() & Qt::ShiftModifier) && (e->modifiers() & Qt::ControlModifier) && 
		  (e->button()==Qt::LeftButton))){
	  
	  }

	  if (((e->modifiers() & Qt::ControlModifier) &&
		    (e->button()==Qt::LeftButton)&&
			slicedialog->getDefaultTrackball()
			)){ 
	    disableTransision=false;
		//e->button()==0;
	  }

	  if (((e->modifiers() & Qt::ShiftModifier) &&
		    (e->button()==Qt::LeftButton)&&
			slicedialog->getDefaultTrackball()
			)){ 

	      disableTransision=false;
	  }
     if(slicedialog->getDefaultTrackball()&&disableTransision)
          gla->trackball.MouseDown(e->x(),(gla->height()-e->y()),QT2VCG(e->button(), e->modifiers()) );
	  else if(!slicedialog->getDefaultTrackball())
		  trackball_slice.MouseDown(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()) );
  
	 gla->update();
 }
 void ExtraMeshSlidePlugin::mouseMoveEvent     (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
 {
	isDragging = true;
	if(e->buttons()| Qt::LeftButton) 
	{
		if(slicedialog->getDefaultTrackball())
			{
			  gla->trackball.MouseMove(e->x(),gla->height()-e->y());
			}
		    else
				trackball_slice.MouseMove(e->x(),gla->height()-e->y());
	}
	gla->update();
 }
 void ExtraMeshSlidePlugin::SlotExportButton(){
	 QString fileName = QFileDialog::getSaveFileName(gla->window(), tr("Save polyline File"),"/",tr("Mesh (*.obj)"));
	 vcg::Plane3f* pl= new vcg::Plane3f();
	 pl->SetDirection(vcg::Point3f(1,0,0));	 
	 pl->SetOffset(100);
	 //vcg::tri::Grid* gr=new vcg::tri::Grid();
	 
	 
	 

 }

 void ExtraMeshSlidePlugin::mouseReleaseEvent  (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
 {
	  isDragging = true;
	  if(slicedialog->getDefaultTrackball())
		 gla->trackball.MouseUp(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()));
	     
			
	 else trackball_slice.MouseUp(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()));
	
	
  
 }
 void ExtraMeshSlidePlugin::Decorate(QAction * ac, MeshModel &m, GLArea * gla)
 {
	 this->gla=gla;
	 this->m=m;
	 
		 DrawPlane(this->gla,this->m);
   
 }
 void ExtraMeshSlidePlugin::EndEdit(QAction * , MeshModel &m, GLArea *gla ){
   slicedialog->close();
 }
 void ExtraMeshSlidePlugin::StartEdit(QAction * , MeshModel &m, GLArea *gla ){
	 if(!first){
	 slicedialog=new Slicedialog(gla->window());
	 
	 first=true;}
	 
	 slicedialog->show();
	 
	 QObject::connect(slicedialog, SIGNAL(exportMesh()), this,SLOT(SlotExportButton()));
     QObject::connect(slicedialog, SIGNAL(Update_glArea()), this, SLOT(upGlA()));
 }
 void ExtraMeshSlidePlugin::upGlA(){
 
	 gla->update();
 }
 void ExtraMeshSlidePlugin::DrawPlane(GLArea * gla, MeshModel &m){
	 
    
	 
    Box3f b=m.cm.bbox;
	
	Point3f mi=b.min;
	Point3f ma=b.max;
	Point3f centre=b.Center();
    float LX= ma[0]-mi[0];
	float LY= ma[1]-mi[1];
	float LZ= ma[2]-mi[2];
	float Delta= max(LX, LY);
	Delta=max(Delta, LZ);
    
 
	glPushMatrix();
	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_LIGHTING_BIT);
    

	
		trackball_slice.GetView();
		trackball_slice.Apply(true);
		trackball_slice.center=centre;
		trackball_slice.radius=Delta;


    glColor4f(1.0,0,0,0.8);
	if(slicedialog->getRestoreDefalut()){
		trackball_slice.Reset();
		gla->trackball.Reset();
        slicedialog->setRestoreDefalut(false);
	}
   
  
   int plane=1;
 if(slicedialog!=0) plane=slicedialog->getPlaneNumber();
  
  float layer=(float)LX /(float)(plane+1);
  for(int i=1; i<=(plane); i++){
	  if(slicedialog->getdistanceDefault()){
		glEnable(GL_BLEND); 
		glColor4f(0,1,0,0.5);
		glBegin(GL_QUADS);
	    glNormal3f(1,0,0);
        glVertex3f(mi[0]+(layer*i), centre[1]-Delta, centre[2]-Delta);
        glVertex3f(mi[0]+(layer*i), centre[1]+Delta, centre[2]-Delta);
		glVertex3f(mi[0]+(layer*i), centre[1]+Delta, centre[2]+Delta);
		glVertex3f(mi[0]+(layer*i), centre[1]-Delta, centre[2]+Delta);
		glEnd();
		glColor4f(1,0,0,0.5);
		glBegin(GL_QUADS);
        glNormal3f(-1,0,0);
        glVertex3f(mi[0]+(layer*i)-0.001, centre[1]-Delta, centre[2]-Delta);
        glVertex3f(mi[0]+(layer*i)-0.001, centre[1]+Delta, centre[2]-Delta);
		glVertex3f(mi[0]+(layer*i)-0.001, centre[1]+Delta, centre[2]+Delta);
		glVertex3f(mi[0]+(layer*i)-0.001, centre[1]-Delta, centre[2]+Delta);
		glEnd();
	  }
	  else{
	    
	    glEnable(GL_BLEND); 
		glColor4f(0,1,0,0.5);
		glBegin(GL_QUADS);
	    glNormal3f(1,0,0);
         glVertex3f(centre[0]-((slicedialog->getDistance()*(plane+1))/2)+(slicedialog->getDistance()*i), centre[1]-Delta, centre[2]-Delta);
         glVertex3f(centre[0]-((slicedialog->getDistance()*(plane+1))/2)+(slicedialog->getDistance()*i), centre[1]+Delta, centre[2]-Delta);
		glVertex3f(centre[0]-((slicedialog->getDistance()*(plane+1))/2)+(slicedialog->getDistance()*i), centre[1]+Delta, centre[2]+Delta);
		glVertex3f(centre[0]-((slicedialog->getDistance()*(plane+1))/2)+(slicedialog->getDistance()*i), centre[1]-Delta, centre[2]+Delta);
		glEnd();
		glColor4f(1,0,0,0.5);
		glBegin(GL_QUADS);
        glNormal3f(-1,0,0);
        glVertex3f(centre[0]-((slicedialog->getDistance()*(plane+1))/2)+(slicedialog->getDistance()*i)-0.01, centre[1]-Delta, centre[2]-Delta);
        glVertex3f(centre[0]-((slicedialog->getDistance()*(plane+1))/2)+(slicedialog->getDistance()*i)-0.01, centre[1]+Delta, centre[2]-Delta);
		glVertex3f(centre[0]-((slicedialog->getDistance()*(plane+1))/2)+(slicedialog->getDistance()*i)-0.01, centre[1]+Delta, centre[2]+Delta);
		glVertex3f(centre[0]-((slicedialog->getDistance()*(plane+1))/2)+(slicedialog->getDistance()*i)-0.01, centre[1]-Delta, centre[2]+Delta);
		glEnd();
	  }
  }
  
      glPopAttrib();
      glPopMatrix();
 if(isDragging){
		 isDragging=false;
		 gla->update();}
 }


 
 Q_EXPORT_PLUGIN(ExtraMeshSlidePlugin)
