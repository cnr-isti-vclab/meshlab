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
:actionList(),base(NULL),candidate(NULL),
dialog(NULL),dialog_dock(NULL),
gla(NULL),mm(NULL),refsize(0),currentmode(ES_Normal),
drawaxes(NULL),drawphantom(NULL),undosystem(NULL)
{
  actionList << new QAction(QIcon(":/images/icon_straightener.png"),"Straighten up a mesh", this);
  foreach(QAction *editAction, actionList)
    editAction->setCheckable(true);
}

QList<QAction *> EditStraightener::actions() const {
  return actionList;
}

const QString EditStraightener::Info(QAction *action) 
{
  if( action->text() != tr("Straighten up a mesh") ) assert (0);
  return tr("Change the coordinate frame of a mesh.");
}

const PluginInfo &EditStraightener::Info() 
{
  static PluginInfo ai; 
  ai.Date=tr(__DATE__);
  ai.Version = tr("1.0");
  ai.Author = ("Luca Benedetti");
  return ai;
}

void EditStraightener::StartEdit(QAction *a, MeshModel &m, GLArea *g )
{
  //cleanup:
  gla=NULL;
  mm=NULL;
  EndEdit(a,m,g);
  gla = g;
  mm = &m;
  assert( (gla!=NULL) && (mm != NULL)); 
  
  gla->setCursor(QCursor(QPixmap(":/images/cur_straightener.png"),15,15));

  refsize=mm->cm.bbox.Diag()/2.0;

  assert(base==NULL); 
  base=new CoordinateFrame(refsize);
  base->basecolor=Color4b(128,128,0,255);
  base->xcolor=Color4b(128,0,0,255);
  base->ycolor=Color4b(0,128,0,255);
  base->zcolor=Color4b(0,0,128,255);
  base->linewidth=1;

  assert(candidate==NULL);
  candidate=new ActiveCoordinateFrame(refsize);
  candidate->basecolor=Color4b(255,255,0,255);
  candidate->xcolor=Color4b(255,0,0,255);
  candidate->ycolor=Color4b(0,255,0,255);
  candidate->zcolor=Color4b(0,0,255,255);  
  candidate->linewidth=3;

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
  connect(dialog, SIGNAL( apply() ),
          this, SLOT( on_apply() ) );
  connect(dialog, SIGNAL( freeze() ),
          this, SLOT( on_freeze() ) );
  connect(dialog, SIGNAL( undo() ),
          this, SLOT( on_undo() ) );
  connect(dialog, SIGNAL( reset_axes() ),
          this, SLOT( on_reset_axes() ) );
  connect(dialog, SIGNAL( reset_origin() ),
          this, SLOT( on_reset_origin() ) );
  connect(dialog, SIGNAL( flip(Point3f) ),
          this, SLOT( on_flip(Point3f) ) );
  connect(dialog, SIGNAL( align_with_view() ),
          this, SLOT( on_align_with_view() ) );
  connect(dialog, SIGNAL( move_axis_to_bbox(int,float) ),
          this, SLOT( on_move_axis_to_bbox(int,float) ) );
  connect(dialog, SIGNAL( center_on_trackball() ),
          this, SLOT( on_center_on_trackball() ) );
  connect(dialog, SIGNAL( draw_on_mesh(bool,bool) ),
          this, SLOT( on_draw_on_mesh(bool,bool) ) );
  connect(dialog, SIGNAL( get_xy_plane_from_selection() ),
          this, SLOT( on_get_xy_plane_from_selection() ) );
  connect(dialog, SIGNAL( freehand_mesh_dragging(bool) ),
          this, SLOT( on_freehand_mesh_dragging(bool) ) );
  connect(dialog, SIGNAL( set_snap(float) ),
          this, SLOT( on_set_snap(float) ) );
  connect(dialog, SIGNAL( update_show(bool,bool,bool,bool,bool,bool,bool,bool) ),
          this, SLOT( on_update_show(bool,bool,bool,bool,bool,bool,bool,bool) ) );
   connect(dialog, SIGNAL( begin_action() ),
          this, SLOT( on_begin_action() ) );
          
  dialog->shoutShow();

  dialog_dock->setVisible(true);
  dialog_dock->layout()->update();

  assert(undosystem==NULL);
  undosystem=new UndoSystem(this);

  gla->update();
  assert((base!=NULL) && (candidate!=NULL));
}

void EditStraightener::EndEdit(QAction *, MeshModel &, GLArea *)
{
  if (dialog!=NULL) { 
    delete dialog; 
    delete dialog_dock; 
    dialog=NULL;
    dialog_dock=NULL;
  }
  
  if(base!=NULL) {
     delete base;
     base=NULL;
  }
  
  if(candidate!=NULL) {
     delete candidate;
     candidate=NULL;
  }
  
  if(drawaxes!=NULL) {
     delete drawaxes;
     drawaxes=NULL;
  }
  
  if(drawphantom!=NULL) {
     delete drawphantom;
     drawphantom=NULL;
  }

  if(undosystem!=NULL) {
     delete undosystem;
     undosystem=NULL;
  }
  
  if(gla!=NULL)
    gla->update();
  gla=NULL;
  mm=NULL;
}

void EditStraightener::Decorate(QAction *, MeshModel &, GLArea *)
{
  dialog->updateSfn(mm->cm.sfn);

  base->Render(gla);
  
  if(drawphantom!=NULL)
    drawphantom->Render();
  else
    candidate->Render(gla);

  if(drawaxes!=NULL){
    drawaxes->Render(gla);
    if (drawaxes->IsReady()){
      Point3f a1,a2,b1,b2;  
      drawaxes->GetAxes(a1,a2,b1,b2);
      undosystem->SaveCandidate();
      candidate->AlignWith(a2-a1,b2-b1); 
      delete drawaxes;
      drawaxes=NULL;
      currentmode=ES_Normal;
      dialog->endSpecialMode();
      gla->update();
    }
  }
  assert(!glGetError());
}

void EditStraightener::mousePressEvent(QAction *, QMouseEvent *e, MeshModel &, GLArea * )
{
  switch (currentmode) {
    case ES_Normal:
      undosystem->BeginAction();
      undosystem->SaveCandidate();
      candidate->MouseDown(e->pos(), e->x(), gla->height() - e->y(), QT2VCG(e->button(), e->modifiers()));
      break;
    case ES_FreehandMeshDragging:
      assert(drawphantom!=NULL);
      drawphantom->MouseDown(e->x(), gla->height() - e->y(), QT2VCG(e->button(), e->modifiers()));
      break;
    default:
      break;
  }
  gla->update();
}

void EditStraightener::mouseMoveEvent(QAction *, QMouseEvent *e, MeshModel &, GLArea * )
{
  switch (currentmode) {
    case ES_Normal:
      candidate->MouseMove(e->pos(), e->x (),gla->height () - e->y ());
      break;
    case ES_DrawOnMesh:
      assert(drawaxes!=NULL);
      drawaxes->mouseMove(e->pos());
      break;
    case ES_FreehandMeshDragging:
      assert(drawphantom!=NULL);
      drawphantom->MouseMove(e->x (),gla->height () - e->y ());
      break;
    default:
      break;
  }
  gla->update();
}

void EditStraightener::mouseReleaseEvent(QAction *,QMouseEvent *e, MeshModel &, GLArea *)
{
  switch (currentmode) {
    case ES_Normal:
      candidate->MouseUp(e->x (),gla->height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
      break;
    case ES_DrawOnMesh:
      assert(drawaxes!=NULL);
      drawaxes->mouseRelease(e->pos());
      break;
    case ES_FreehandMeshDragging:
      assert(drawphantom!=NULL);
      drawphantom->MouseUp(e->x (),gla->height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
      break;
    default:
      break;
  }
  gla->update();  
}

void EditStraightener::keyReleaseEvent (QAction *, QKeyEvent * e, MeshModel &, GLArea *)
{
  int button = 0;
  if (e->key () == Qt::Key_Control){
    button = QT2VCG (Qt::NoButton, Qt::ControlModifier);
  } else if (e->key () == Qt::Key_Shift) {
    button = QT2VCG (Qt::NoButton, Qt::ShiftModifier);
  } else if (e->key () == Qt::Key_Alt) {
    button = QT2VCG (Qt::NoButton, Qt::AltModifier);
  } else {
    gla->update();
    return;
  }
  switch (currentmode) {
    case ES_Normal:
      candidate->ButtonUp(button);
      break;
    case ES_FreehandMeshDragging:
      assert(drawphantom!=NULL);
      drawphantom->ButtonUp(button);
    default:
      break;
  }
  gla->update();
}


void EditStraightener::keyPressEvent (QAction *, QKeyEvent * e, MeshModel &, GLArea *)
{
  int button;
  if (e->key () == Qt::Key_Control){
    button = QT2VCG (Qt::NoButton, Qt::ControlModifier);
  } else if (e->key () == Qt::Key_Shift) {
    button = QT2VCG (Qt::NoButton, Qt::ShiftModifier);
  } else if (e->key () == Qt::Key_Alt) {
    button = QT2VCG (Qt::NoButton, Qt::AltModifier);
  } else {
    gla->update();
    return;
  }
  switch (currentmode) {
    case ES_Normal:
      candidate->ButtonDown(button);
      break;
    case ES_FreehandMeshDragging:
      assert(drawphantom!=NULL);
      drawphantom->ButtonDown(button);
    default:
      break;
  }
  gla->update();
}

// slots:
void EditStraightener::on_apply()
{
  Matrix44f tr; 
  candidate->GetTransform(tr);
  undosystem->SaveCandidate();
  candidate->Reset(true,true);
  undosystem->SaveTR();
  mm->cm.Tr = Inverse(tr) * mm->cm.Tr ;
  gla->update();
}

void EditStraightener::on_freeze()
{
  gla->setWindowModified(true);
  undosystem->SaveFreeze();
  tri::UpdatePosition<CMeshO>::Matrix(mm->cm, mm->cm.Tr);
  tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(mm->cm);
  tri::UpdateBounding<CMeshO>::Box(mm->cm);
  undosystem->SaveTR();
  mm->cm.Tr.SetIdentity();
  gla->trackball.Reset();
  gla->update();
}

void EditStraightener::on_undo()
{
  assert(undosystem->CanUndo());
  undosystem->Undo();
  gla->update();
}

void EditStraightener::on_reset_axes()
{
  undosystem->SaveCandidate();
  candidate->Reset(false,true);
  gla->update();
}

void EditStraightener::on_reset_origin()
{
  undosystem->SaveCandidate();
  candidate->Reset(true,false);
  gla->update();
}


void EditStraightener::on_flip(Point3f axis)
{
  undosystem->SaveCandidate();
  candidate->Flip(axis);
  gla->update();
}

void EditStraightener::on_align_with_view()
{
  undosystem->SaveCandidate();
  candidate->SetRotation(Inverse(gla->trackball.track.rot));  
  gla->update();
}

void EditStraightener::on_move_axis_to_bbox(int axis, float value)
{
  Box3f bbox = mm->cm.trBB();
  Point3f p = candidate->GetPosition();
  p[axis]=bbox.min[axis] + (bbox.max[axis]-bbox.min[axis]) * value;
  undosystem->SaveCandidate();
  candidate->SetPosition(p);
  gla->update();
}

void EditStraightener::on_center_on_trackball()
{
  Box3f bbox;
  foreach(MeshModel * m, gla->meshDoc.meshList) 
    bbox.Add(m->cm.trBB());
  undosystem->SaveCandidate();
  candidate->SetPosition(bbox.Center()-(gla->trackball.track.tra) * (0.5*bbox.Diag()));
  gla->update();
}

void EditStraightener::on_draw_on_mesh(bool twoaxes,bool begin)
{
  if(begin){
    assert(currentmode==ES_Normal);
  	drawaxes = new DrawAxes(twoaxes);
    currentmode=ES_DrawOnMesh;
  } else {
    assert(currentmode==ES_DrawOnMesh);
    delete drawaxes;
    drawaxes=NULL;
    currentmode=ES_Normal;
  }
  gla->update();
}

void EditStraightener::on_get_xy_plane_from_selection()
{
  assert(mm->cm.sfn > 0);
 
  Box3f bbox; //il bbox delle facce selezionate
  std::vector< Point3f > selected_pts; //devo copiare i punti per il piano di fitting
  
  tri::UpdateSelection<CMeshO>::ClearVertex(mm->cm);
  tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(mm->cm);  
  CMeshO::VertexIterator vi;
  for(vi=mm->cm.vert.begin();vi!=mm->cm.vert.end();++vi)
    if(!(*vi).IsD() && (*vi).IsS() ){
  	  Point3f p=(*vi).P();
      bbox.Add(p);
      selected_pts.push_back(p);
    }  

  Plane3f plane;
  PlaneFittingPoints(selected_pts,plane); //calcolo il piano di fitting

  undosystem->SaveCandidate();
  candidate->SetPosition(plane.Projection(bbox.Center()));
  candidate->AlignWith(plane.Direction(),Point3f(0,0,0));   
  gla->update();
}

void EditStraightener::on_freehand_mesh_dragging(bool begin)
{
  if(begin){
    assert(currentmode==ES_Normal);
    undosystem->SaveCandidate();
    candidate->Reset(true,true);
  	drawphantom = new DrawPhantom(mm,refsize);
    currentmode=ES_FreehandMeshDragging;
  } else {
    assert(currentmode==ES_FreehandMeshDragging);
    assert(drawphantom!=NULL);
    Matrix44f tr;
    tr=drawphantom->manipulator->track.Matrix(); 

    undosystem->SaveTR();
    mm->cm.Tr = mm->cm.Tr * tr ;    
    delete drawphantom;
    drawphantom=NULL;
    currentmode=ES_Normal;
  }
  gla->update();
}

void EditStraightener::on_set_snap(float rot_snap_deg)
{
  candidate->SetSnap(rot_snap_deg);
  gla->update();
}

void EditStraightener::on_update_show(bool ba, bool bl, bool bv, bool ca, bool cl, bool cv, bool m, bool r)
{
  base->drawaxis=ba;
  base->drawlabels=bl;
  base->drawvalues=bv;
  candidate->drawaxis=ca;
  candidate->drawlabels=cl;
  candidate->drawvalues=cv;
  candidate->drawmoves=m;
  candidate->drawrotations=r;
  gla->update();
}

void EditStraightener::on_begin_action()
{
  undosystem->BeginAction();
}

//////////////////////////////////////////////////////////////

DrawAxes::DrawAxes(bool b)
:currentphase(DA_BEGIN),twoaxes(b),
first(Color4b(128,128,255,255)),
second(Color4b(128,255,128,255))
{

}

void DrawAxes::Render(QGLWidget *glw)
{
  first.Render(glw);
  second.Render(glw);
  switch(currentphase){
    case DA_BEGIN:
      if(first.IsReady())
        currentphase=DA_DONE_FIRST;
      else first.RenderLabel("Z",glw);
      break;
    case DA_DONE_FIRST:
      if(second.IsReady())
        currentphase=DA_DONE_SECOND;
      else second.RenderLabel("Y",glw);
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

DrawPhantom::DrawPhantom(MeshModel* mm,float refsize)
{
  glmesh.m = &(mm->cm);
  glmesh.Update ();
  tr = mm->cm.Tr;
  manipulator = new Trackball();   
  manipulator->radius = refsize;
  manipulator->center = Inverse(tr) * mm->cm.trBB().Center() ;
  // rifaccio i modi senza le scalature
  std::map<int, TrackMode *>::iterator it;
  for(it = manipulator->modes.begin(); it != manipulator->modes.end(); it++)
  {
    if ((*it).second)
      delete (*it).second;
  }
  manipulator->modes.clear();
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
:marks(0),es(e),undotype_vec(),tr_vec(),pos_vec(),rot_vec()
{ 
}

void UndoSystem::BeginAction()
{
  undotype_vec.push_back(US_MARK);
  marks++;
  if(marks>MAX_MARKS)
    limitmarks();
  if(marks==1)
    es->dialog->enableUndo();
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
    es->dialog->disableUndo();  
}

bool UndoSystem::CanUndo()
{
  return marks>0;
}

void UndoSystem::SaveCandidate()
{
  assert(undotype_vec.size()>0);
  undotype_vec.push_back(US_CANDIDATE);
  pos_vec.push_back(es->candidate->GetPosition());
  rot_vec.push_back(es->candidate->GetRotation());
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
    case US_CANDIDATE:
      es->candidate->SetPosition(pos_vec.back());
      es->candidate->SetRotation(rot_vec.back());  
      pos_vec.pop_back();
      rot_vec.pop_back();
      break;
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
      case US_CANDIDATE:
        pos_vec.pop_front();
        rot_vec.pop_front();
        break;
      case US_TR:
        tr_vec.pop_front();
        break;
      case US_FREEZE:
        break;
    }
    undotype_vec.pop_front();
  }
}

Q_EXPORT_PLUGIN(EditStraightener)
