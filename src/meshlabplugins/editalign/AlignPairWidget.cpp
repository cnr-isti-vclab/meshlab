#include <GL/glew.h>
#include <QtGui>
#include <QGLWidget>
#include <wrap/gl/pick.h>


//#include "alignDialog.h"
//#include <meshlab/glarea.h>
#include "editalign.h"
#include "AlignPairWidget.h"

#include <wrap/qt/trackball.h>
#include <wrap/gl/picking.h>
AlignPairWidget::AlignPairWidget (QWidget * parent) :QGLWidget (parent)
{
	meshLeft=0;
	meshRight=0;
	tt[0]=&trackLeft;
	tt[1]=&trackRight;	
	
	pickedPoints[0].clear();
	pickedPoints[1].clear();

	//resize(800,400);
	setMinimumSize(800,400);
	hasToPick=false;
	pointToPick=Point2i(-1,-1);
}


void AlignPairWidget::initMesh(MeshModel *left, MeshTree *right)
{
	meshLeft=left;
	meshRight=right;
	updateGL();
}

void AlignPairWidget::initializeGL ()
{
  glClearColor(0, 0, 0, 0); 
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void AlignPairWidget::paintGL ()
{
	//MeshModel * mm[2]={meshLeft,meshRight};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(meshLeft==0 || meshRight==0) return;
	
	for(int i=0;i<2;++i)
		{
			if(i==0)  glViewport (0, 0, (GLsizei) width()/2, (GLsizei) height()); 
       else			glViewport (width()/2, 0, (GLsizei) width()/2, (GLsizei) height()); 
			 
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluPerspective(30, (AlignPairWidget::width()/2)/(float)AlignPairWidget::height(), 0.1, 100);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				gluLookAt(0,0,4,   0,0,0,   0,1,0);
				tt[i]->center=vcg::Point3f(0, 0, 0);
				tt[i]->radius= 1;
				tt[i]->GetView();
				tt[i]->Apply(false);
				glPushMatrix();
				Box3f bb;
				if(i==0) bb=meshLeft->cm.bbox;
				else	   bb=meshRight->bbox();
				float d=2.0f/bb.Diag();
				vcg::glScale(d);
				glTranslate(-bb.Center());
				if(i==0)
						meshLeft->Render(GLW::DMFlat,GLW::CMPerMesh,GLW::TMNone);
				else
					foreach(MeshNode *mn, meshRight->nodeList) 
						if(mn->glued) mn->m->Render(GLW::DMFlat,GLW::CMPerMesh,GLW::TMNone);

						
				drawPickedPoints(i);	
				
				int pickSide= ( pointToPick[0] < (width()/2) )? 0 : 1;
				if(hasToPick && pickSide==i)
					{	
						Point3f pp;
						hasToPick=false;
						if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp)) 
						{
							qDebug("Picked point %i %i -> %f %f %f",pointToPick[0],pointToPick[1],pp[0],pp[1],pp[2]);
							pickedPoints[pickSide].push_back(pp);
							hasToPick=false;
						}
					}
				tt[i]->DrawPostApply();

		} 
}

void AlignPairWidget::drawPickedPoints(int side)
{
	glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_CURRENT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE);
	if(side==0) glColor(vcg::Color4b(Color4b::Red));
	if(side==1) glColor(vcg::Color4b(Color4b::Blue));
	glPointSize(3.0);
	
	for(int i=0; i<pickedPoints[side].size();++i)
		{
		Point3f &pt =pickedPoints[side][i];
			glBegin(GL_POINTS);
				glVertex(pt);
			glEnd();
			renderText(pt[0],pt[1],pt[2],QString("%1").arg(i));
		}
	glPopAttrib();
	
}
void AlignPairWidget::keyReleaseEvent (QKeyEvent * e)
{
  e->ignore ();
	
  for(int i=0;i<2;++i)
		{
			if (e->key () == Qt::Key_Control) tt[i]->ButtonUp (QT2VCG (Qt::NoButton, Qt::ControlModifier));
			if (e->key () == Qt::Key_Shift)   tt[i]->ButtonUp (QT2VCG (Qt::NoButton, Qt::ShiftModifier));
			if (e->key () == Qt::Key_Alt)     tt[i]->ButtonUp (QT2VCG (Qt::NoButton, Qt::AltModifier));
		}
	updateGL ();
}

void AlignPairWidget::keyPressEvent (QKeyEvent * e)
{
  e->ignore ();
  for(int i=0;i<2;++i)
		{
			if (e->key () == Qt::Key_Control)  tt[i]->ButtonDown (QT2VCG (Qt::NoButton, Qt::ControlModifier));
			if (e->key () == Qt::Key_Shift)    tt[i]->ButtonDown (QT2VCG (Qt::NoButton, Qt::ShiftModifier));
			if (e->key () == Qt::Key_Alt)      tt[i]->ButtonDown (QT2VCG (Qt::NoButton, Qt::AltModifier));
		}
  updateGL ();
}
void AlignPairWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	hasToPick=true;
	pointToPick=Point2i(e->x(), height() -e->y());
}
void AlignPairWidget::mousePressEvent (QMouseEvent * e)
{
  e->accept ();
  setFocus ();
	int index = e->x () < ( width() /2) ? 0 : 1 ;
  tt[index]->MouseDown (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
  updateGL ();
}

void AlignPairWidget::mouseMoveEvent (QMouseEvent * e)
{
	int index = e->x () < ( width() /2) ? 0 : 1 ;
	
	if (e->buttons ()) {
    tt[index]->MouseMove (e->x (), height () - e->y ());
    updateGL ();
  }
}

void AlignPairWidget::mouseReleaseEvent (QMouseEvent * e)
{
	int index = e->x () < ( width() /2) ? 0 : 1 ;
  tt[index]->MouseUp (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
  updateGL ();
}

void AlignPairWidget::wheelEvent (QWheelEvent * e)
{
	int index = e->x () < ( width() /2) ? 0 : 1 ;
  const int WHEEL_STEP = 120;
  tt[index]->MouseWheel (e->delta () / float (WHEEL_STEP), QTWheel2VCG (e->modifiers ()));
  updateGL ();
}
