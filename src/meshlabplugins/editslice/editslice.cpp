#include <QtGui>
#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "editslice.h"
#include <qstring.h>
#include <wrap/gl/pick.h>

#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/vertexplus/component_ocf.h>
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
if ( dialogsliceobj!=0) { 
			delete  dialogsliceobj; 
			
			 dialogsliceobj=0;
			 }
	
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



void ExtraMeshSlidePlugin::mouseReleaseEvent  (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
{

	 trackball_slice.MouseUp(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()));


}
 void ExtraMeshSlidePlugin::mousePressEvent    (QAction *, QMouseEvent * e, MeshModel &m, GLArea * gla)
 {  

	  if (  (e->button()==Qt::LeftButton)         &&
		  !(e->modifiers() & Qt::ShiftModifier) )
		  trackball_slice.MouseDown(e->x(),gla->height()-e->y(),QT2VCG(e->button(), e->modifiers()) );
	 gla->update();
 }
 void ExtraMeshSlidePlugin::mouseMoveEvent     (QAction *,QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
 {
  if( (e->buttons()| Qt::LeftButton) &&
	  !(e->modifiers() & Qt::ShiftModifier))
	  trackball_slice.MouseMove(e->x(),gla->height()-e->y());
	gla->update();
 }
 void ExtraMeshSlidePlugin::RestoreDefault(){
           trackball_slice.Reset();
		   gla->trackball.Reset();
		   gla->update();
 }
 void ExtraMeshSlidePlugin::SlotExportButton()
 {
	 
	QFileDialog saveF;
	fileName = saveF.getSaveFileName(gla->window(), tr("Saving..."),"/",tr("Mesh (*.svg)"));
	if (fileName==0) return;

	
	dialogsliceobj->hide();


	Matrix44f mat_trac_rotation ; 
	trackball_slice.track.rot.ToMatrix( mat_trac_rotation ); //rotation Matrix of the plans' trackball 
	Point3f* dir=new Point3f(1,0,0);   //the plans' normal vector init 
	(*dir)= mat_trac_rotation * (*dir); //rotation of the directions vector 
	Point3f translation_plans=trackball_slice.track.tra;  //vettore di translazione dei piani
	bool EvportVector=false;           //variabile used after
	vector<n_EdgeMesh*> ev;           
	ev.clear();                        
	
	Point3d d((*dir).X(),(*dir).Y(), (*dir).Z());
	pr.setPlane(0, d); 
	svgpro= new SVGPro(gla->window(), point_Vector.size(), dialogsliceobj->getExportOption());
	svgpro->Init(pr.getWidth(), pr.getHeight(), pr.getViewBox()[0], pr.getViewBox()[1], pr.getScale());
	if ( svgpro->exec() == QDialog::Accepted ) 
	
	{ 
		UpdateVal(svgpro, &pr);
		if (!dialogsliceobj->getExportOption())
			pr.numCol=1;
		else
			pr.numCol=point_Vector.size();
		pr.numRow=1;
		
		mesh_grid = new TriMeshGrid();
		mesh_grid->Set(m.cm.face.begin() ,m.cm.face.end());
		float scale =  (pr.getViewBox().V(0)/pr.numCol) /(edgeMax*(1.4142)) ;
		pr.setScale(scale);
		pr.setTextDetails( svgpro->showText );
		for(int i=0; i<point_Vector.size(); i++){	
			Point3f rotationCenter=m.cm.bbox.Center(); //the point where the plans rotate
			Point3f po=point_Vector[i]-m.cm.bbox.Center();
			Plane3f p;
			p.SetDirection(*dir);
				/*
				/ Equazione del piano ax+by+cz=distance
				/  a,b,c coordinata centro di rotazione del piano
			    /  x,y,z vettore di rotazione del pinao
			   */   
			
			Point3f off= mat_trac_rotation * (translation_plans+po); //translation vector
			p.SetOffset( (rotationCenter.X()*dir->X() )+ (rotationCenter.Y()*dir->Y()) +(rotationCenter.Z()*dir->Z())+ (off*(*dir)));
			
			double avg_length;  
			edge_mesh = new n_EdgeMesh();
			vcg::Intersection<n_Mesh, n_EdgeMesh, n_Mesh::ScalarType, TriMeshGrid>(p , *edge_mesh, avg_length, mesh_grid, intersected_cells);
			vcg::edge::UpdateBounding<n_EdgeMesh>::Box(*edge_mesh);
		
			
			if (!dialogsliceobj->getExportOption()){
			   QString index;
			   index.setNum(i);
			   fileN=fileName.left( fileName.length ()- 4 )+"_"+index+".svg";
			   pr.setPosition(Point2d(0,0));
			   pr.numCol=1;
			   pr.numRow=1;
			   
			   vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(edge_mesh, fileN.toLatin1().data(), pr);
		
			}
			else{
		     
			 ev.push_back(edge_mesh);
             EvportVector=true;
            }
		}
	
	if(EvportVector){
		
		vcg::edge::io::ExporterSVG<n_EdgeMesh>::Save(&ev, fileName.toLatin1().data(),pr);
        //Free memory allocated
		
		vector<n_EdgeMesh*>::iterator it;
		for(it=ev.begin(); it!=ev.end(); it++){
			delete *it;}
		
		} 
   }
 dialogsliceobj->show();
 }

void ExtraMeshSlidePlugin::UpdateVal(SVGPro* sv,  SVGProperties * pr){
	 bool ok;
     

	 pr->setDimension(sv->getImageWidth(),sv->getImageHeight());
	 pr->setViewBox(Point2d(sv->getViewBoxWidth(), sv->getViewBoxHeight()));
	
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
   //if ( dialogsliceobj!=0) { 
	//		delete  dialogsliceobj; 
			
	//		 dialogsliceobj=0;
	//		
	// }
 }
 void ExtraMeshSlidePlugin::StartEdit(QAction * , MeshModel &m, GLArea *gla ){
	 gla->update();
	 if(!first){
		 dialogsliceobj=new dialogslice(gla->window());
		 dialogsliceobj->show();
		 first=true;
		
		 this->m=m;
		 QObject::connect(dialogsliceobj, SIGNAL(exportMesh()), this,SLOT(SlotExportButton()));
         QObject::connect(dialogsliceobj, SIGNAL(Update_glArea()), this, SLOT(upGlA()));
	    QObject::connect(dialogsliceobj, SIGNAL(RestoreDefault()), this, SLOT(RestoreDefault()));
	 }
	 }
 void ExtraMeshSlidePlugin::upGlA(){
 
	 gla->update();
	 
 } 
 void ExtraMeshSlidePlugin::DrawPlane(GLArea * gla, MeshModel &m){
     
	b=m.cm.bbox; //Boundig Box
	Point3f mi=b.min;
	Point3f ma=b.max;
	Point3f centre=b.Center() ;
	edgeMax=0;
    float LX= ma[0]-mi[0];
	float LY= ma[1]-mi[1];
	float LZ= ma[2]-mi[2];
	edgeMax= max(LX, LY);
	edgeMax=max(edgeMax, LZ); //edgeMax: the longest side of BBox
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
  point_Vector.clear();
  for(int i=1; i<=(plane); i++){
	  int in_ass;
	  if(dialogsliceobj->getdistanceDefault())in_ass=0;
		else in_ass=2;
		
		glEnable(GL_CULL_FACE);
		glColor4f(0,1,0,0.5);
		glBegin(GL_QUADS);
			glNormal3f(1,0,0);
			
			float assi_x[4];
			assi_x[0]=mi[0]+(layer*i);
			assi_x[1]=mi[0]+(layer*((plane+1)-i));
			assi_x[2]=centre[0]-((dialogsliceobj->getDistance()*(plane+1))/2)+(dialogsliceobj->getDistance()*i);
			assi_x[3]=centre[0]-((dialogsliceobj->getDistance()*(plane+1))/2)+(dialogsliceobj->getDistance()*((plane+1)-i));	
			
			this->point_Vector.push_back(Point3f(assi_x[in_ass], centre[1], centre[2]));
			
			glVertex3f(assi_x[in_ass], centre[1]-edgeMax, centre[2]-edgeMax);
			glVertex3f(assi_x[in_ass], centre[1]+edgeMax, centre[2]-edgeMax);
			glVertex3f(assi_x[in_ass], centre[1]+edgeMax, centre[2]+edgeMax);
			glVertex3f(assi_x[in_ass], centre[1]-edgeMax, centre[2]+edgeMax);
		glEnd();
		
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
