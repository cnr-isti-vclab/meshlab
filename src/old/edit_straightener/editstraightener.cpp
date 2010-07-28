/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2008                                                \/)\/    *
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
Revision 1.5  2008/02/27 14:50:24  benedetti
fixed on_freehand_mesh_dragging, minor cleanups, now all functionality works

Revision 1.4  2008/02/26 21:35:17  benedetti
corrected after quaternion/similarity/trackball changes, on_freehand_mesh_dragging still doesn't work

Revision 1.3  2008/02/22 20:24:42  benedetti
refactored, cleaned up a bit, few feats added

Revision 1.2  2008/02/17 20:57:33  benedetti
updated following new specs (still got to clean up)

Revision 1.1  2008/02/16 14:29:35  benedetti
first version


****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include <meshlab/glarea.h>

#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/space/fitting3.h>

#include <wrap/qt/trackball.h>

#include "editstraightener.h"

using namespace vcg;

EditStraightener::EditStraightener()
:dialog(NULL),dialog_dock(NULL),
gla(NULL),mm(NULL),refsize(0),currentmode(ES_Normal),
origin(NULL),old_origin(NULL),dragged_origin(NULL),
dragged_mesh(NULL),drawned_axes(NULL),undosystem(NULL),draw_bbox(true)
{}

const QString EditStraightener::Info() 
{
	return tr("Change the coordinate frame of a mesh.");
}

bool EditStraightener::StartEdit(MeshModel &m, GLArea *g )
{
  //cleanup:
  gla=NULL;
  mm=NULL;
  EndEdit(m,g);
  gla = g;
  mm = &m;
  assert( (gla!=NULL) && (mm != NULL)); 
  
  gla->setCursor(QCursor(QPixmap(":/images/cur_straightener.png"),15,15));

  refsize=mm->cm.bbox.Diag()/2.0;

  assert(origin==NULL); 
  origin=new MovableCoordinateFrame(refsize);
  origin->basecolor=Color4b(170,170,0,255);
  origin->xcolor=Color4b(170,0,0,255);
  origin->ycolor=Color4b(0,170,0,255);
  origin->zcolor=Color4b(0,0,170,255);
  origin->linewidth=1.5;

  currentmode=ES_Normal;

  assert(dialog==NULL);
  dialog = new EditStraightenerDialog(gla->window());
  dialog_dock = new QDockWidget(gla->window());
  dialog_dock->setAllowedAreas(Qt::NoDockWidgetArea);
  dialog_dock->setWidget(dialog);
  QPoint p = gla->window()->mapToGlobal(QPoint(0,0));
  int x = -5 + p.x() + gla->window()->width() - dialog->width();
  int y = p.y();
  int w = dialog->width();
  int h = dialog->height();  
  dialog_dock->setGeometry(x,y,w,h);
  dialog_dock->setFloating(true);
  connect(dialog, SIGNAL( begin_action() ),
          this, SLOT( on_begin_action() ) );
  connect(dialog, SIGNAL( apply() ),
          this, SLOT( on_apply() ) );
  connect(dialog, SIGNAL( freeze() ),
          this, SLOT( on_freeze() ) );
  connect(dialog, SIGNAL( undo() ),
          this, SLOT( on_undo() ) );
  connect(dialog, SIGNAL( rot(float,Point3f) ),
          this, SLOT( on_rot(float,Point3f) ) );
  connect(dialog, SIGNAL( align_with_view() ),
          this, SLOT( on_align_with_view() ) );
  connect(dialog, SIGNAL( move_axis_to_bbox(int,float) ),
          this, SLOT( on_move_axis_to_bbox(int,float) ) );
  connect(dialog, SIGNAL( center_on_trackball() ),
          this, SLOT( on_center_on_trackball() ) );
  connect(dialog, SIGNAL( draw_on_mesh(bool,char,char) ),
          this, SLOT( on_draw_on_mesh(bool,char,char) ) );
  connect(dialog, SIGNAL( freehand_axis_dragging(bool) ),
          this, SLOT( on_freehand_axis_dragging(bool) ) );
  connect(dialog, SIGNAL( set_snap(float) ),
          this, SLOT( on_set_snap(float) ) );
  connect(dialog, SIGNAL( freehand_mesh_dragging(bool) ),
          this, SLOT( on_freehand_mesh_dragging(bool) ) );
  connect(dialog, SIGNAL( get_plane_from_selection(char,char) ),
          this, SLOT( on_get_plane_from_selection(char,char) ) );
  connect(dialog, SIGNAL( update_show(bool,bool,bool,bool,bool,bool,bool,bool,bool) ),
          this, SLOT( on_update_show(bool,bool,bool,bool,bool,bool,bool,bool,bool) ) );
          
  dialog->shoutShow();
  dialog->SetFreeze(freezable());

  dialog_dock->setVisible(true);
  dialog_dock->layout()->update();

  assert(undosystem==NULL);
  undosystem=new UndoSystem(this);

  gla->update();
  assert(origin!=NULL);
	return true;
}

void EditStraightener::EndEdit(MeshModel &, GLArea *)
{
  if (dialog!=NULL) { delete dialog; dialog=NULL; }
  if (dialog_dock!=NULL) { delete dialog_dock; dialog_dock=NULL; }
  if (origin!=NULL) { delete origin; origin=NULL; }
  if (old_origin!=NULL) { delete old_origin; old_origin=NULL; }
  if (dragged_origin!=NULL) { delete dragged_origin; dragged_origin=NULL; }
  if (dragged_mesh!=NULL) { delete dragged_mesh; dragged_mesh=NULL; }
  if (drawned_axes!=NULL) { delete drawned_axes; drawned_axes=NULL; }
  if (undosystem!=NULL) { delete undosystem; undosystem=NULL; }
  
  if(gla!=NULL){
     gla->releaseKeyboard();
  	 gla->update();
  }
  gla=NULL;
  mm=NULL;
}

void EditStraightener::Decorate(MeshModel &, GLArea *)
{
  dialog->updateSfn(mm->cm.sfn);

  if (currentmode == ES_FreehandAxisDragging){
    old_origin->Render(gla);
    dragged_origin->Render(gla);
  } else {
    origin->Render(gla);
  }
  
  if(currentmode == ES_FreehandMeshDragging)
    dragged_mesh->Render();

  if( (currentmode == ES_DrawOnMesh) && (drawned_axes!=NULL) ){
    drawned_axes->Render(gla);
    if (drawned_axes->IsReady()){
      Point3f a1,a2,b1,b2;  
      drawned_axes->GetAxes(a1,a2,b1,b2);
      origin->AlignWith(a2-a1,b2-b1,drawned_axes->firstchar,drawned_axes->secondchar); 
      on_apply();
      delete drawned_axes;
      drawned_axes=NULL;
      currentmode=ES_Normal;
      dialog->endSpecialMode();
      gla->trackball.Reset();
      gla->update();
    }
  }
  
  if(draw_bbox){
  	Box3f bbox = mm->cm.trBB();
  	float mx=bbox.min[0],my=bbox.min[1],mz=bbox.min[2];
  	float Mx=bbox.max[0],My=bbox.max[1],Mz=bbox.max[2];
    float x=math::Clamp(.0f,mx,Mx);
    float y=math::Clamp(.0f,my,My);
    float z=math::Clamp(.0f,mz,Mz);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glLineWidth(2);
    glPointSize(5);
    glBegin(GL_LINES);
      if(mx<0){ // negative x
        glColor3f(.3f,.0f,.0f);
        glVertex3f(mx,my,mz); glVertex3f(x,my,mz);
        glVertex3f(mx,my,Mz); glVertex3f(x,my,Mz);
        glVertex3f(mx,My,mz); glVertex3f(x,My,mz);
        glVertex3f(mx,My,Mz); glVertex3f(x,My,Mz);
      }
      if(Mx>0){ // positive x
        glColor3f(.6f,.3f,.3f);
        glVertex3f(x,my,mz); glVertex3f(Mx,my,mz);
        glVertex3f(x,my,Mz); glVertex3f(Mx,my,Mz);
        glVertex3f(x,My,mz); glVertex3f(Mx,My,mz);
        glVertex3f(x,My,Mz); glVertex3f(Mx,My,Mz);
      }
      if(my<0){ // negative y
        glColor3f(.0f,.3f,.0f);
        glVertex3f(mx,my,mz); glVertex3f(mx,y,mz);
        glVertex3f(mx,my,Mz); glVertex3f(mx,y,Mz);
        glVertex3f(Mx,my,mz); glVertex3f(Mx,y,mz);
        glVertex3f(Mx,my,Mz); glVertex3f(Mx,y,Mz);
      }
      if(My>0) { // positive y
        glColor3f(.3f,.6f,.3f);
        glVertex3f(mx,y,mz); glVertex3f(mx,My,mz);
        glVertex3f(mx,y,Mz); glVertex3f(mx,My,Mz);
        glVertex3f(Mx,y,mz); glVertex3f(Mx,My,mz);
        glVertex3f(Mx,y,Mz); glVertex3f(Mx,My,Mz);
      }
      if(mz<0) { // negative z
        glColor3f(.0f,.0f,.3f);
        glVertex3f(mx,my,mz); glVertex3f(mx,my,z);
        glVertex3f(mx,My,mz); glVertex3f(mx,My,z);
        glVertex3f(Mx,my,mz); glVertex3f(Mx,my,z);
        glVertex3f(Mx,My,mz); glVertex3f(Mx,My,z);
      }
      if(Mz>0) { // positive z
        glColor3f(.3f,.3f,.6f);
        glVertex3f(mx,my,z); glVertex3f(mx,my,Mz);
        glVertex3f(mx,My,z); glVertex3f(mx,My,Mz);
        glVertex3f(Mx,my,z); glVertex3f(Mx,my,Mz);
        glVertex3f(Mx,My,z); glVertex3f(Mx,My,Mz);
      }
    glEnd();
    glBegin(GL_POINTS);
      if((mx<0)&&(Mx>0)){ // x crosses 0
        glColor3f(1.0f,.6f,.6f);
        glVertex3f(x,my,mz);
        glVertex3f(x,my,Mz);
        glVertex3f(x,My,mz);
        glVertex3f(x,My,Mz);
      }
      if((my<0)&&(My>0)){ // y crosses 0
        glColor3f(.6f,1.0f,.6f);
        glVertex3f(mx,y,mz);
        glVertex3f(mx,y,Mz);
        glVertex3f(Mx,y,mz);
        glVertex3f(Mx,y,Mz);
      }
      if((mz<0)&&(Mz>0)){ // z crosses 0
        glColor3f(.6f,.6f,1.0f);
        glVertex3f(mx,my,z);
        glVertex3f(mx,My,z);
        glVertex3f(Mx,my,z);
        glVertex3f(Mx,My,z);
      }
    glEnd();
    glPopAttrib();  
  }
  
  assert(!glGetError());
}

void EditStraightener::mousePressEvent(QMouseEvent *e, MeshModel &, GLArea * )
{
  switch (currentmode) {
    case ES_FreehandAxisDragging:
      dragged_origin->MouseDown(e->x(), gla->height() - e->y(), QT2VCG(e->button(), e->modifiers()));
      break;
    case ES_FreehandMeshDragging:
      dragged_mesh->MouseDown(e->x(), gla->height() - e->y(), QT2VCG(e->button(), e->modifiers()));
      break;
    default:
      break;
  }
  gla->update();
}

void EditStraightener::mouseMoveEvent(QMouseEvent *e, MeshModel &, GLArea * )
{
  switch (currentmode) {
    case ES_FreehandAxisDragging:
      dragged_origin->MouseMove(e->x (),gla->height () - e->y ());
      break;
    case ES_DrawOnMesh:
      drawned_axes->mouseMove(e->pos());
      break;
    case ES_FreehandMeshDragging:
      dragged_mesh->MouseMove(e->x (),gla->height () - e->y ());
      break;
    default:
      break;
  }
  gla->update();
}

void EditStraightener::mouseReleaseEvent(QMouseEvent *e, MeshModel &, GLArea *)
{
  switch (currentmode) {
    case ES_FreehandAxisDragging:
      dragged_origin->MouseUp(e->x (),gla->height () - e->y (), QT2VCG (e->button (), 0));
      break;
    case ES_DrawOnMesh:
      drawned_axes->mouseRelease(e->pos());
      break;
    case ES_FreehandMeshDragging:
      dragged_mesh->MouseUp(e->x (),gla->height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
      break;
    default:
      break;
  }
  gla->update();  
}

void EditStraightener::keyReleaseEvent (QKeyEvent * e, MeshModel &, GLArea *)
{
  int button = 0;
  if (e->key () == Qt::Key_Control){
    button = QT2VCG (Qt::NoButton, Qt::ControlModifier);
  } else if (e->key () == Qt::Key_Shift) {
    button = QT2VCG (Qt::NoButton, Qt::ShiftModifier);
  } else if (e->key () == Qt::Key_Alt) {
    button = QT2VCG (Qt::NoButton, Qt::AltModifier);
  } else {
    e->ignore();
    gla->update();
    return;
  }
  switch (currentmode) {
    case ES_FreehandAxisDragging:
      dragged_origin->ButtonUp(button);
      break;
    case ES_FreehandMeshDragging:
      dragged_mesh->ButtonUp(button);
    default:
      e->ignore();
  }
  gla->update();
}


void EditStraightener::keyPressEvent (QKeyEvent * e, MeshModel &, GLArea *)
{
  int button;
  if (e->key () == Qt::Key_Control){
    button = QT2VCG (Qt::NoButton, Qt::ControlModifier);
  } else if (e->key () == Qt::Key_Shift) {
    button = QT2VCG (Qt::NoButton, Qt::ShiftModifier);
  } else if (e->key () == Qt::Key_Alt) {
    button = QT2VCG (Qt::NoButton, Qt::AltModifier);
  } else {
    e->ignore();
    gla->update();
    return;
  }
  switch (currentmode) {
    case ES_FreehandAxisDragging:
      dragged_origin->ButtonDown(button);
      break;
    case ES_FreehandMeshDragging:
      dragged_mesh->ButtonDown(button);
    default:
      e->ignore();
  }
  gla->update();
}

bool EditStraightener::freezable()
{
  const float EPSILON=1e-4;
  for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
      if( math::Abs(mm->cm.Tr[i][j] - (i==j?1:0)) > EPSILON)
        return true;
  return false;
}

// slots:
void EditStraightener::on_begin_action()
{
  assert(origin->GetPosition()==Point3f(0,0,0));
  assert(origin->GetRotation()==Quaternionf(0,Point3f(1,0,0)));
  undosystem->BeginAction();
}

void EditStraightener::on_apply()
{
  Matrix44f tr; 
  origin->GetTransform(tr);
  origin->Reset(true,true);
  undosystem->SaveTR();
  mm->cm.Tr = Inverse(tr) * mm->cm.Tr ;
  dialog->SetFreeze(freezable());
  gla->update();
}

void EditStraightener::on_freeze()
{
  assert(origin->GetPosition()==Point3f(0,0,0));
  assert(origin->GetRotation()==Quaternionf(0,Point3f(1,0,0)));
  on_begin_action();
  gla->setWindowModified(true);
  undosystem->SaveFreeze();
  tri::UpdatePosition<CMeshO>::Matrix(mm->cm, mm->cm.Tr);
  tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(mm->cm);
  tri::UpdateBounding<CMeshO>::Box(mm->cm);
  undosystem->SaveTR();
  mm->cm.Tr.SetIdentity();
  dialog->SetFreeze(false);
  gla->trackball.Reset();
  gla->update();
}

void EditStraightener::on_undo()
{
  assert(undosystem->CanUndo());
  undosystem->Undo();
  dialog->SetFreeze(freezable());
  gla->update();
}


void EditStraightener::on_rot(float angle_deg,Point3f axis)
{
  on_begin_action();
  origin->Rot(angle_deg,axis);
  on_apply();
  gla->update();
}

void EditStraightener::on_align_with_view()
{
  on_begin_action();
  origin->SetRotation(gla->trackball.track.rot);  
  on_apply();
  gla->trackball.Reset();
  gla->update();
}

void EditStraightener::on_move_axis_to_bbox(int axis, float value)
{
  Box3f bbox = mm->cm.trBB();
  Point3f p = origin->GetPosition();
  p[axis]=bbox.min[axis] + (bbox.max[axis]-bbox.min[axis]) * value;
  origin->SetPosition(p); // della parte begin_action\apply si occupa EditStraightenerDialog
  gla->update();
}

void EditStraightener::on_center_on_trackball()
{
  Box3f bbox;
  foreach(MeshModel * m, gla->meshDoc.meshList) 
    bbox.Add(m->cm.trBB());
  on_begin_action();
  origin->SetPosition(bbox.Center()-(gla->trackball.track.tra) * (0.5*bbox.Diag()));
  on_apply();
  gla->update();
}

void EditStraightener::on_draw_on_mesh(bool begin,char c1,char c2)
{
  if(begin){
    assert(currentmode==ES_Normal);
    on_begin_action();
    drawned_axes = new DrawAxes(c1,c2);
    currentmode=ES_DrawOnMesh;
  } else {
    assert(currentmode==ES_DrawOnMesh);
    delete drawned_axes;
    drawned_axes=NULL;
    on_undo();
    currentmode=ES_Normal;
  }
  gla->update();
}

void EditStraightener::on_freehand_axis_dragging(bool begin)
{
  if(begin){
    assert(currentmode==ES_Normal);
    assert(old_origin==NULL); 
    assert(dragged_origin==NULL);
    on_begin_action();    
    gla->setMouseTracking(true);
    gla->grabKeyboard ();    
    old_origin=new CoordinateFrame(refsize);
    old_origin->basecolor=Color4b(128,128,0,255);
    old_origin->xcolor=Color4b(128,0,0,255);
    old_origin->ycolor=Color4b(0,128,0,255);
    old_origin->zcolor=Color4b(0,0,128,255);
    old_origin->linewidth=1;
    dragged_origin=new ActiveCoordinateFrame(refsize);
    dragged_origin->basecolor=Color4b(255,255,0,255);
    dragged_origin->xcolor=Color4b(255,0,0,255);
    dragged_origin->ycolor=Color4b(0,255,0,255);
    dragged_origin->zcolor=Color4b(0,0,255,255);  
    dragged_origin->linewidth=3;
    dragged_origin->SetSnap(dialog->getSnap());
    dialog->shoutShow();
    currentmode=ES_FreehandAxisDragging;
  } else {
    assert(currentmode==ES_FreehandAxisDragging);
    origin->SetPosition(dragged_origin->GetPosition());
    origin->SetRotation(dragged_origin->GetRotation());
    delete (old_origin); old_origin=NULL;
    delete (dragged_origin); dragged_origin=NULL;
    currentmode=ES_Normal;
    gla->releaseKeyboard();
    gla->setMouseTracking(false);
    on_apply(); 
  }
  gla->update();
}

void EditStraightener::on_set_snap(float rot_snap_deg)
{
  if(dragged_origin!=NULL)
    dragged_origin->SetSnap(rot_snap_deg);
  gla->update();
}

void EditStraightener::on_freehand_mesh_dragging(bool begin)
{
  if(begin){
    assert(currentmode==ES_Normal);
    on_begin_action();
  	dragged_mesh = new DrawPhantom(mm,refsize);
    currentmode=ES_FreehandMeshDragging;
  } else {
    assert(currentmode==ES_FreehandMeshDragging);
    assert(dragged_mesh!=NULL);
    Matrix44f a,b,c;
    a.SetTranslate( dragged_mesh->manipulator->center);
    b = dragged_mesh->manipulator->track.Matrix();
    c.SetTranslate(-dragged_mesh->manipulator->center);    
    undosystem->SaveTR();
    mm->cm.Tr = mm->cm.Tr * a * b * c;
    on_apply(); 
    delete dragged_mesh;
    dragged_mesh=NULL;
    currentmode=ES_Normal;
  }
  gla->update();
}

void EditStraightener::on_get_plane_from_selection(char normal,char preserve)
{
  assert(mm->cm.sfn > 0);
 
  Box3f bbox; //il bbox delle facce selezionate
  std::vector< Point3f > selected_pts; //devo copiare i punti per il piano di fitting
  
  tri::UpdateSelection<CMeshO>::ClearVertex(mm->cm);
  tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(mm->cm);  
  CMeshO::VertexIterator vi;
  for(vi=mm->cm.vert.begin();vi!=mm->cm.vert.end();++vi)
    if(!(*vi).IsD() && (*vi).IsS() ){
  	  Point3f p = mm->cm.Tr * (*vi).P();
      bbox.Add(p);
      selected_pts.push_back(p);
    }

  Plane3f plane;
  PlaneFittingPoints(selected_pts,plane); //calcolo il piano di fitting

  on_begin_action();
  origin->SetPosition(plane.Projection(bbox.Center()));
  origin->AlignWith(plane.Direction(),Point3f(0,0,0),normal,preserve);
  on_apply(); 
  
  gla->update();
}

void EditStraightener::on_update_show(bool db,bool ba, bool bl, bool bv, bool ca, bool cl, bool cv, bool m, bool r)
{
  draw_bbox=db;
  origin->drawaxis=ba;
  origin->drawlabels=bl;
  origin->drawvalues=bv;
  if(dragged_origin!=NULL){
  	old_origin->drawaxis=ba;
  	old_origin->drawlabels=bl;
  	old_origin->drawvalues=bv;
    dragged_origin->drawaxis=ca;
    dragged_origin->drawlabels=cl;
    dragged_origin->drawvalues=cv;
    dragged_origin->drawmoves=m;
    dragged_origin->drawrotations=r;
  }
  gla->update();
}

//////////////////////////////////////////////////////////////

DrawAxes::DrawAxes(char ch1,char ch2)
:firstchar(ch1),secondchar(ch2),
currentphase(DA_BEGIN),twoaxes(true),
first(Color4b(0,0,0,0)),second(Color4b(0,0,0,0)),
label1(QChar(ch1)),label2(QChar(ch2))
{
  Color4b xc(255,128,128,255),yc(128,255,128,255),zc(128,128,255,255);
  switch(ch1){
    case 'X': first=Rubberband(xc); break;
    case 'Y': first=Rubberband(yc); break;
    case 'Z': first=Rubberband(zc); break;
    default: assert(0);
  }
  switch(ch2){
    case 'X': second=Rubberband(xc); break;
    case 'Y': second=Rubberband(yc); break;
    case 'Z': second=Rubberband(zc); break;
    case ' ': twoaxes=false; break;
    default: assert(0);
  }
}

void DrawAxes::Render(QGLWidget *glw)
{
  first.Render(glw);
  second.Render(glw);
  switch(currentphase){
    case DA_BEGIN:
      if(first.IsReady())
        currentphase=DA_DONE_FIRST;
      else first.RenderLabel(label1,glw);
      break;
    case DA_DONE_FIRST:
      if(second.IsReady())
        currentphase=DA_DONE_SECOND;
      else second.RenderLabel(label2,glw);
      break;
    case DA_DONE_SECOND:
      break;
    default:
      assert (0);
  }
  assert(!glGetError());
}

void DrawAxes::mouseMove(QPoint p)
{
  if(IsReady())
    return;
  if(currentphase>=DA_DONE_FIRST)
    second.Drag(p);
  else
    first.Drag(p);
}

void DrawAxes::mouseRelease(QPoint p)
{
  if(IsReady())
    return;
  if(currentphase >= DA_DONE_FIRST)
    second.Pin(p);
  else
    first.Pin(p);
}

bool DrawAxes::IsReady()
{
  return currentphase >= ( twoaxes ? DA_DONE_SECOND : DA_DONE_FIRST);
}

void DrawAxes::GetAxes(Point3f &f1,Point3f &f2,Point3f &s1,Point3f &s2)
{
  assert(IsReady());
  first.GetPoints(f1,f2);
  if(twoaxes){
    second.GetPoints(s1,s2);
  }else{
    s1=s2=Point3f(0,0,0);
  }
}

///////////////////////////////////

DrawPhantom::DrawPhantom(MeshModel* mm,float refsize)
{
  glmesh.m = &(mm->cm);
  glmesh.Update ();
  tr=mm->cm.Tr;
  manipulator = new Trackball();
  manipulator->radius = refsize;
  manipulator->center = Inverse(tr) * mm->cm.trBB().Center() ;

  // rifaccio i modi senza le scalature
	manipulator->ClearModes();
  manipulator->modes[0] = NULL;    
  manipulator->modes[Trackball::BUTTON_LEFT] = new SphereMode ();
  manipulator->modes[Trackball::BUTTON_LEFT |Trackball:: KEY_CTRL] = new PanMode ();
  manipulator->modes[Trackball::BUTTON_MIDDLE] = new PanMode ();
  manipulator->modes[Trackball::BUTTON_LEFT | Trackball::KEY_ALT] = new ZMode ();
  manipulator->SetCurrentAction();  
}

DrawPhantom::~DrawPhantom()
{
   if(manipulator!=NULL) {
     delete manipulator;
     manipulator=NULL;
  }
}

void DrawPhantom::Render()
{
  glPushMatrix();
  glPushAttrib (GL_ALL_ATTRIB_BITS);
  
  glMultMatrix(tr);
  
  manipulator->GetView();
  manipulator->Apply(true);  

  float amb[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
  float col[4] = { 0.5f, 0.5f, 0.8f, 1.0f };
  float spe[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  glEnable (GL_NORMALIZE);
  glEnable (GL_LINE_SMOOTH);
  glEnable (GL_BLEND);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor3f(0.4,0.4,0.8);
  
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, amb);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, col);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR,spe);

  glmesh.Draw < GLW::DMWire , GLW::CMNone, GLW::TMNone > ();
  
  glPopAttrib ();
  glPopMatrix();
  assert(!glGetError());  
}

void DrawPhantom::MouseDown(int x, int y, /*Button*/ int button)
{
  manipulator->MouseDown(x,y,button);
}

void DrawPhantom::MouseMove(int x, int y)
{
  manipulator->MouseMove(x,y);
}

void DrawPhantom::MouseUp(int x, int y, /*Button */ int button) 
{
  manipulator->MouseUp(x, y, button);
}

void DrawPhantom::ButtonUp(int button)
{
  manipulator->ButtonUp((Trackball::Button) button);
}

void DrawPhantom::ButtonDown(int button)
{
  manipulator->ButtonDown((Trackball::Button) button);
}

////////////////////////////////////

UndoSystem::UndoSystem(EditStraightener const *e)
:marks(0),es(e),undotype_vec(),tr_vec()
{ 
}

void UndoSystem::BeginAction()
{
  undotype_vec.push_back(US_MARK);
  marks++;
  if(marks>MAX_MARKS)
    limitmarks();
  if(marks==1)
    es->dialog->SetUndo(true);
}

void UndoSystem::Undo()
{
  assert(marks>0);
  while(revert());
  assert(undotype_vec.size()>0);
  assert(undotype_vec.back()==US_MARK);
  undotype_vec.pop_back();
  marks--;
  if(marks==0)
    es->dialog->SetUndo(false);  
}

bool UndoSystem::CanUndo()
{
  return marks>0;
}

void UndoSystem::SaveTR()
{
  assert(undotype_vec.size()>0);
  undotype_vec.push_back(US_TR);
  tr_vec.push_back(es->mm->cm.Tr);
}

void UndoSystem::SaveFreeze()
{
  assert(undotype_vec.size()>0);
  undotype_vec.push_back(US_FREEZE);
}


bool UndoSystem::revert()
{
  assert(undotype_vec.size()>0);
  UndoType undotype=undotype_vec.back();
  switch(undotype){
  	case US_MARK:
  	  return false;
    case US_TR:
      es->mm->cm.Tr=tr_vec.back();
      tr_vec.pop_back();
      break;
    case US_FREEZE:
      es->gla->setWindowModified(true);
      tri::UpdatePosition<CMeshO>::Matrix(es->mm->cm, Inverse(es->mm->cm.Tr));
      tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(es->mm->cm);
      tri::UpdateBounding<CMeshO>::Box(es->mm->cm);
      break;
  }
  undotype_vec.pop_back();
  return true;  
}

void UndoSystem::limitmarks()
{
  if(undotype_vec.size()<=MAX_MARKS)
    return;
  assert(undotype_vec.size()>0);
  assert(undotype_vec.front()==US_MARK);
  // levo il mark di sotto 
  undotype_vec.pop_front();
  marks--;
  // levo tutto fino al prossimo mark (escluso)
  while(true){
    switch(undotype_vec.front()){
      case US_MARK:
        return;
      case US_TR:
        tr_vec.pop_front();
        break;
      case US_FREEZE:
        break;
    }
    undotype_vec.pop_front();
  }
}
