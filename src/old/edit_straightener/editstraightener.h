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
Revision 1.5  2008/03/02 16:48:46  benedetti
Fixed after coordinateframe/activecoordinateframe file splitting

Revision 1.4  2008/02/26 21:35:34  benedetti
corrected after quaternion/similarity/trackball changes

Revision 1.3  2008/02/22 20:24:42  benedetti
refactored, cleaned up a bit, few feats added

Revision 1.2  2008/02/17 20:57:33  benedetti
updated following new specs (still got to clean up)

Revision 1.1  2008/02/16 14:29:35  benedetti
first version


****************************************************************************/

#ifndef EDITSTRAIGHTENER_H
#define EDITSTRAIGHTENER_H

#include <GL/glew.h>
#include <QtGui>
#include <common/interfaces.h>

#include <wrap/gui/activecoordinateframe.h>
#include <wrap/gui/rubberband.h>


#include "editstraightenerdialog.h"

class DrawAxes;
class DrawPhantom;
class UndoSystem;

/*!
  @brief The straightener editing plugin.

 */
class EditStraightener : public QObject, public MeshEditInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshEditInterface)

public:
  EditStraightener(); 
  virtual ~EditStraightener() {}

  static const QString Info();
  virtual bool StartEdit(MeshModel &, GLArea * );
  virtual void EndEdit(MeshModel &, GLArea * );
  virtual void Decorate(MeshModel &, GLArea * );
  virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void mouseReleaseEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void keyReleaseEvent(QKeyEvent *,   MeshModel &, GLArea * );
  virtual void keyPressEvent(QKeyEvent *,   MeshModel &, GLArea * );

private:
  //types:
  typedef enum { ES_Normal = 0,
                 ES_DrawOnMesh = 1,
                 ES_FreehandAxisDragging = 2,
                 ES_FreehandMeshDragging = 3
               } EditStraightenerMode;
  // data:
  EditStraightenerDialog *dialog;
  QDockWidget *dialog_dock;
  GLArea* gla;
  MeshModel* mm;
  float refsize;
  EditStraightenerMode currentmode;
  MovableCoordinateFrame *origin;
  CoordinateFrame *old_origin;
  ActiveCoordinateFrame *dragged_origin;
  DrawPhantom *dragged_mesh;
  DrawAxes *drawned_axes;  
  UndoSystem *undosystem;
  bool draw_bbox;
  // friends:
  friend class UndoSystem;
  //functions:
  bool freezable();
  
public slots:
  void on_begin_action();
  void on_apply();
  void on_freeze();
  void on_undo();
  void on_rot(float,Point3f);
  void on_align_with_view();
  void on_move_axis_to_bbox(int,float);
  void on_center_on_trackball();
  void on_draw_on_mesh(bool,char,char);
  void on_freehand_axis_dragging(bool);
  void on_set_snap(float);
  void on_freehand_mesh_dragging(bool);
  void on_get_plane_from_selection(char,char);
  void on_update_show(bool,bool,bool,bool,bool,bool,bool,bool,bool);
};

class DrawAxes
{
public:
  char firstchar,secondchar;
  DrawAxes(char ch1,char ch2=' ');
  virtual ~DrawAxes() {}
  void Render(QGLWidget *);
  void mouseMove(QPoint);
  void mouseRelease(QPoint);
  bool IsReady();
  void GetAxes(Point3f &,Point3f &,Point3f &,Point3f &);
private:
  //types:
  typedef enum { DA_BEGIN = 0,
	             DA_DONE_FIRST = 1,
	             DA_DONE_SECOND = 2
	           } DrawAxesPhase;  
  // data:
  DrawAxesPhase currentphase;
  bool twoaxes;
  Rubberband first,second;
  QString label1,label2;
};

class DrawPhantom
{
public:
  DrawPhantom(MeshModel*,float);
  virtual ~DrawPhantom();
  void Render();
  void MouseDown(int, int, int);
  void MouseMove(int, int); 
  void MouseUp(int, int, int); 
  void ButtonUp(int);
  void ButtonDown(int);
  Trackball *manipulator;
  Matrix44f tr;
private:
  GlTrimesh < CMeshO > glmesh;
};

class UndoSystem
{
public:
  UndoSystem(EditStraightener const*);
  virtual ~UndoSystem() {}
  void BeginAction();
  void Undo();
  bool CanUndo();
//  void SaveCandidate();
  void SaveTR();
  void SaveFreeze();
private:
  // types:
  typedef enum { US_MARK = 0,
	             US_TR = 1,
	             US_FREEZE = 2
	           } UndoType;  
  // data:
  const static int MAX_MARKS=100;
  int marks;
  EditStraightener const *es;
  QVector<UndoType> undotype_vec;
  QVector<Matrix44f> tr_vec;
  // functions:
  bool revert();
  void limitmarks();
  
};

#endif /*EDITSTRAIGHTENER_H*/
