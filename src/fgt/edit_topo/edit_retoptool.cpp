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
$Log: edit_retoptool.cpp,v $
****************************************************************************/
#include "edit_retoptool.h"
#include "edit_retoptoolmeshbuilder.h"








using namespace std;
using namespace vcg;
//
// --- Plugin specific methods ---
//
edit_retoptool::edit_retoptool() 
{
	edit_retoptooldialogobj=0;
	reDraw = false;
	click = false;

	nameVtxCount = 0;
	stack.clear();
	Estack.clear();

	lastPoint.V = Point3f(0,0,0);
	lastPoint.vName = "--";
	
	connectStart.V = Point3f(0,0,0);
	connectStart.vName = "--";
	connectEnd.V = Point3f(0,0,0);
	connectEnd.vName = "--";

	actionList << new QAction(QIcon(":/images/icon_measure.png"), "Re-Topology Tool", this);
	foreach(QAction *editAction, actionList)
		editAction->setCheckable(true);
}


edit_retoptool::~edit_retoptool() 
{
	stack.clear();
}


QList<QAction *> edit_retoptool::actions() const 
{
  return actionList;
}

const QString edit_retoptool::Info(QAction *action) 
{
  if( action->text() != tr("ReTop Tool") ) assert (0);
  return tr("Allow to re-top a model");
}

const PluginInfo &edit_retoptool::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
   ai.Version = tr("0.1");
   ai.Author = ("dani");
   return ai;
}
void edit_retoptool::Decorate(QAction *, MeshModel &m, GLArea * gla)
{
	updateMatrixes();
	// onClick
	if(click)
	{
		click=false;

		// Original Vertex selection mode (yellow pointer)
		if(edit_retoptooldialogobj->utensil==U_VTX_SEL)
		{
			CVertexO * temp_vert=0;
			if (getVertexAtMouse(m, temp_vert))
			{
				if(temp_vert->P() != lastPoint.V)
				{
					Vtx temp;
					temp.V = temp_vert->P();
					temp.vName = QString("V%1").arg(nameVtxCount++);

					bool contained = false;
					for(int i=0; i<stack.count(); i++)
					{
						Vtx at = stack.at(i);
						if(at.V==temp.V)
							contained = true;
					}

					if(!contained)
					{
						stack.push_back(temp);
						lastPoint = temp;				
						edit_retoptooldialogobj->insertVertexInTable(lastPoint.vName, QString("%1").arg(lastPoint.V.X()), QString("%1").arg(lastPoint.V.Y()), QString("%1").arg(lastPoint.V.Z()));
					}
					else nameVtxCount--;
				}
			}
		}


		// Free vertex selection (no pointer)
		if(edit_retoptooldialogobj->utensil==U_VTX_SEL_FREE)
		{
			Point3f temp_vert;
			if (Pick(mousePos.x(), mouseRealY, temp_vert))
			{
				if(temp_vert != lastPoint.V)
				{
					Vtx temp;
					temp.V = temp_vert;
					temp.vName = QString("V%1").arg(nameVtxCount++);

					bool contained = false;
					for(int i=0; i<stack.count(); i++)
					{
						Vtx at = stack.at(i);
						if(at.V==temp.V)
							contained = true;
					}

					if(!contained)
					{
						stack.push_back(temp);
						lastPoint = temp;				
						edit_retoptooldialogobj->insertVertexInTable(lastPoint.vName, QString("%1").arg(lastPoint.V.X()), QString("%1").arg(lastPoint.V.Y()), QString("%1").arg(lastPoint.V.Z()));
					}
					else nameVtxCount--;
				}
			}
		}


		// Remove selected vertex mode (Yellow pointer)
		if(edit_retoptooldialogobj->utensil==U_VTX_DEL)
		{
			Vtx vtx = getVisibleVertexNearestToMouse(stack);

			if(vtx.V != Point3f(0,0,0))
			{
				for(int i=0; i<stack.count(); i++)
					if(stack.at(i) == vtx)
					{
						stack.removeAt(i);
						edit_retoptooldialogobj->removeVertexInTable(QString("%1").arg(vtx.V.X()), QString("%1").arg(vtx.V.Y()), QString("%1").arg(vtx.V.Z()));
					}

				if(stack.count()==0)
					nameVtxCount = 0;
			}
		}


		// Edge mode (vertex connection)
		if(edit_retoptooldialogobj->utensil==U_VTX_CONNECT)
		{
			if(connectStart.V==Point3f(0,0,0))
			{
				Vtx vtx = getVisibleVertexNearestToMouse(stack);
				if(vtx.V != Point3f(0,0,0))
				{
					connectStart.V = vtx.V;
					connectStart.vName = vtx.vName;
				}	
			}
			else
			{
				Vtx vtx = getVisibleVertexNearestToMouse(stack);
				if((vtx.V != Point3f(0,0,0))&&(vtx.V != connectStart.V))
				{
					connectEnd.V = vtx.V;
					connectEnd.vName = vtx.vName;
					Edg added;
					added.v[0] = connectStart;
					added.v[1] = connectEnd;
			
					if(!Estack.contains(added))
					{
						Estack.push_back(added);
						edit_retoptooldialogobj->insertConnectionInTable(connectStart.vName, connectEnd.vName);

						QList<Edg> endStack;
						QList<Edg> staStack;
						Edg _3rd;

						//--> Check if the "just now" selected edge is connected to others, and creates a new face
						for(int i=0; i<Estack.count(); i++)
						{
							Edg ed = Estack.at(i);

							if(ed != added)
							{
								if((ed.v[0] == connectEnd)||(ed.v[1] == connectEnd))
									endStack.push_back(ed);
								if((ed.v[0] == connectStart)||(ed.v[1] == connectStart))
									staStack.push_back(ed);
							}
						}

						for(int i=0; i<endStack.count(); i++)
						{
							Vtx nextEnd;
							Edg edgEnd = endStack.at(i);

							if(edgEnd.v[0] == connectEnd)
								nextEnd = edgEnd.v[1];
							if(edgEnd.v[1] == connectEnd)
								nextEnd = edgEnd.v[0];

							for(int j=0; j<staStack.count(); j++)
							{
								Vtx nextSta;
								Edg edgSta = staStack.at(j);

								if(edgSta.v[0] == connectStart)
									nextSta = edgSta.v[1];
								if(edgSta.v[1] == connectStart)
									nextSta = edgSta.v[0];

								if(nextSta == nextEnd)
								{
									Fce toAdd;
									toAdd.e[0] = added;
									toAdd.e[1] = edgSta;
									toAdd.e[2] = edgEnd;

									if(!Fstack.contains(toAdd))
									{
										Fstack.push_back(toAdd);

										QList<QString> vNames;
										for(int n=0; n<3; n++)
											for(int m=0; m<2; m++)
												if(!vNames.contains(toAdd.e[n].v[m].vName))
													vNames.push_back(toAdd.e[n].v[m].vName);

										edit_retoptooldialogobj->insertFaceInTable(vNames.at(0), vNames.at(1), vNames.at(2));
									}
								}							
							}						
						}						

						connectStart.V = Point3f(0,0,0);
						connectStart.vName = "00";
					}
					connectEnd.V = Point3f(0,0,0);
					connectEnd.vName = "00";
				}
			}
		}




	}// end if click

	
	
	
	
	
	// Decorations... //
	


	// Even if there's something selected: show vtx	
	if(edit_retoptooldialogobj->utensil==U_FCE_SEL)
	{
		if(in.count()!=0)
		{
			for(int i=0; i<in.count(); i++)
				drawPoint(m, 4.0f, Color4b::White, Color4b::White, in.at(i));
		}
		if(out.count()!=0)
		{
			for(int i=0; i<out.count(); i++)
				drawPoint(m, 2.0f, Color4b::Yellow, Color4b::Yellow, out.at(i));
		}	

		if(Estack.count()!=0)
		{
			for(int i=0; i<Estack.count(); i++)
			{
				Edg e = Estack.at(i);
				Vtx p1 = e.v[0];
				Vtx p2 = e.v[1];

				drawLine(m, 2.0f, 3.0f, Color4b::Blue, Color4b::Green, p1.V, p2.V);
			}
		}

	}
	
	if((edit_retoptooldialogobj->utensil==U_VTX_CONNECT)||(edit_retoptooldialogobj->utensil==U_VTX_SEL)
		||(edit_retoptooldialogobj->utensil==U_VTX_SEL_FREE)||(edit_retoptooldialogobj->utensil==U_VTX_DEL)
		||(edit_retoptooldialogobj->utensil==U_VTX_CONNECT)||(edit_retoptooldialogobj->utensil==U_VTX_DE_CONNECT))
	{
		if(stack.count()!=0)
			drawPoint(m, 3.0f, Color4b::DarkRed, Color4b::Red, stack);

		if(stack.count()!=0)
			drawLabel(stack);

		if(Estack.count()!=0)
		{
			for(int i=0; i<Estack.count(); i++)
			{
				Edg e = Estack.at(i);
				Vtx p1 = e.v[0];
				Vtx p2 = e.v[1];

				drawLine(m, 2.0f, 3.0f, Color4b::Blue, Color4b::Green, p1.V, p2.V);
			}
		}
	}

	// select/deselect existing facese
	if(edit_retoptooldialogobj->utensil==U_FCE_SEL)
	{
		if(Fstack.count()!=0)
		{
			for(int f=0; f<Fstack.count(); f++)
			{
				Fce fa = Fstack.at(f);
				for(int ed=0; ed<3; ed++)
				{
					Edg e = fa.e[ed];
					Vtx p1 = e.v[0];
					Vtx p2 = e.v[1];
					drawLine(m, 2.0f, 3.0f, Color4b::DarkRed, Color4b::Red, p1.V, p2.V);					
				}
			}		
		}		
	}

	// Yellow pointer (vertex selection)
	if(edit_retoptooldialogobj->utensil==U_VTX_SEL)
	{
		Point3f p = Point3f(0,0,0);

		glPushMatrix();
		glMultMatrix(m.cm.Tr);
		
		CVertexO * temp_vert=0;
		if (getVertexAtMouse(m, temp_vert))
		{
			cursorPoint = temp_vert->P();
			drawPoint(m, 4.0f, Color4b::DarkGreen, Color4b::Yellow, cursorPoint);
		}
	}


	// Vertex de-selection (Yellow pointeR)
	if((edit_retoptooldialogobj->utensil==U_VTX_DEL)&&(stack.count()!=0))
	{
		Vtx vtx = getVisibleVertexNearestToMouse(stack);

		if(vtx.V != Point3f(0,0,0))
			drawPoint(m, 4.0f, Color4b::DarkGreen, Color4b::Green, vtx.V);
	}


	// edge mode (vtx connection)
	if(edit_retoptooldialogobj->utensil==U_VTX_CONNECT)
	{
		if(connectStart.V==Point3f(0,0,0) && connectEnd.V==Point3f(0,0,0))
		{
			Vtx vtx = getVisibleVertexNearestToMouse(stack);

			if(vtx.V != Point3f(0,0,0))
				drawPoint(m, 4.0f, Color4b::DarkGreen, Color4b::Green, vtx.V);			
		}


		if(connectStart.V!=Point3f(0,0,0) && connectEnd.V==Point3f(0,0,0))
		{
			drawPoint(m, 4.0f, Color4b::LightBlue, Color4b::Blue, connectStart.V);

			Vtx vtx = getVisibleVertexNearestToMouse(stack);

			if(vtx.V != Point3f(0,0,0))
			{
				drawPoint(m, 4.0f, Color4b::DarkGreen, Color4b::Green, vtx.V);
				drawLine(m, 2.0f, 3.0f, Color4b::Blue, Color4b::Green, connectStart.V, vtx.V);
			}
		}
	}

	// edge deselection mode
	if(edit_retoptooldialogobj->utensil==U_VTX_DE_CONNECT)
	{
		// Get the vtx closest to the mouse
		Vtx vtx = getVisibleVertexNearestToMouse(stack);		

		// Get all edges he howns
		QList<Edg> nearEdge;
		for(int i=0; i<Estack.count(); i++)
		{
			Edg e = Estack.at(i);
			if((e.v[0] == vtx)||(e.v[1] == vtx))
				nearEdge.push_back(e);
		}

		if(nearEdge.count()>1)
		{
			Edg e = nearEdge.at(0);

			double x1, x2, y1, y2, tz;

			gluProject(e.v[0].V.X(),e.v[0].V.Y(),e.v[0].V.Z(), mvmatrix,projmatrix,viewport, &x1,&y1,&tz);
			gluProject(e.v[1].V.X(),e.v[1].V.Y(),e.v[1].V.Z(), mvmatrix,projmatrix,viewport, &x2,&y2,&tz);

			float bestD = distancePointSegment(QPointF(mousePos.x(),mouseRealY), QPointF(x1,y1), QPointF(x2,y2));
			Edg bestE = e;

			for(int i=1; i<nearEdge.count(); i++)
			{
				e = nearEdge.at(i);
				gluProject(e.v[0].V.X(),e.v[0].V.Y(),e.v[0].V.Z(), mvmatrix,projmatrix,viewport, &x1,&y1,&tz);
				gluProject(e.v[1].V.X(),e.v[1].V.Y(),e.v[1].V.Z(), mvmatrix,projmatrix,viewport, &x2,&y2,&tz);

				float dist = distancePointSegment(QPointF(mousePos.x(),mouseRealY), QPointF(x1,y1), QPointF(x2,y2));

				if(dist < bestD)
				{
					bestD = dist;
					bestE = e;		
				}
			}
			drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Green, bestE.v[0].V, bestE.v[1].V);
		}
		else if (nearEdge.count()==1)
		{
			Edg e = nearEdge.at(0);
			drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Green, e.v[0].V, e.v[1].V);
		}


	}
}









































void edit_retoptool::StartEdit(QAction *, MeshModel &m, GLArea *parent)
{	
	parentGla = parent;
	parent->setCursor(QCursor(QPixmap(":/images/cursor_paint.png"),1,1));	

	if (edit_retoptooldialogobj==0)
		edit_retoptooldialogobj=new edit_retoptooldialog(parent->window());

	parent->setMouseTracking(true);
	edit_retoptooldialogobj->show();

	connect(edit_retoptooldialogobj, SIGNAL( mesh_create() ),
          this, SLOT( on_mesh_create() ) );

	connect(edit_retoptooldialogobj, SIGNAL( fuffa() ),
          this, SLOT( on_fuffa() ) );
}

void edit_retoptool::EndEdit(QAction *, MeshModel &, GLArea *)
{
	
}



void edit_retoptool::on_mesh_create()
{
	in.clear();
	out.clear();


	MeshModel *mm= new MeshModel();	
	parentGla->meshDoc.meshList.push_back(mm);	

	MeshModel *m     = parentGla->meshDoc.meshList.back();	// destination = last
	MeshModel *currentMesh  = parentGla->meshDoc.mm();		// source = current		

	RetopMeshBuilder * rm = new RetopMeshBuilder(currentMesh);


	// Mesh creation
	if(edit_retoptooldialogobj->isRadioButtonSimpleChecked())
		rm->createBasicMesh(*m, Fstack, stack);
	else
	{
		int iter = edit_retoptooldialogobj->getIterations();
		int dist1 = edit_retoptooldialogobj->dist(1);
		int dist2 = edit_retoptooldialogobj->dist(2); 
		rm->createRefinedMesh(*m, iter, Fstack, edit_retoptooldialogobj, dist1, dist2);
	}
	// Color sampling
	if(edit_retoptooldialogobj->isCheckBoxTrColorChecked())
	{
		// TODO Color sampling
	}

//	rm->draww(stack);


//	in = rm->Lin;
//	out = rm->Lout;

	delete rm;
	m->fileName = "Retopology.ply";
	tri::UpdateBounding<CMeshO>::Box(m->cm);	// updates bounding box
	m->cm.Tr = currentMesh->cm.Tr;				// copy transformation
}









// DEBUG!!!
void edit_retoptool::on_fuffa()
{/*
//			MeshModel *trgMesh     = parentGla->meshDoc.meshList.back();	// destination = last
//			MeshModel *srcMesh = parentGla->meshDoc.mm();		// source = current		

			MeshModel *m2      = parentGla->meshDoc.mm();		// source = current		
			MeshModel *m1	 = parentGla->meshDoc.meshList.back();	// destination = last

	CMeshO *m = &m2->cm;

	m2->updateDataMask(MeshModel::MM_FACEMARK);

	typedef GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
	MetroMeshGrid   unifGrid;

	// Parameters
	typedef trimesh::FaceTmark<CMeshO> MarkerFace;
	MarkerFace markerFunctor;
	

	unifGrid.Set(m->face.begin(), m->face.end());

	markerFunctor.SetMesh(m);


	CMeshO::FaceIterator fi;
	//for(fi=m1->cm.face.begin(); fi!=m1->cm.face.end(); fi++)
	for(int fuffa = 0; fuffa<1; fuffa++)
	{

	/*CMeshO::VertexIterator vi = m1->cm.vert.begin();
	CMeshO::VertexType &p1 =  (*vi); vi++;
	CMeshO::VertexType &p2 = (*vi); 

	Point3f punto ;

	for(int i=0; i<3; i++)
	{

		if(i==0)
		{
			p1 = *fi->V(0);
			p2 = *fi->V(1);
			punto = (stack.at(0).V + stack.at(1).V)/2;
		}
		if(i==1)
		{
			p1 = *fi->V(1);
			p2 = *fi->V(2);
			punto = (stack.at(1).V + stack.at(2).V)/2;
		}
		if(i==2)
		{
			p1 = *fi->V(0);
			p2 = *fi->V(2);
			punto = (stack.at(0).V + stack.at(2).V)/2;
		}



	float dist_upper_bound =m->bbox.Diag()/50;
	// the results
	Point3f closestPt, normf, bestq, ip;
	float dist = dist_upper_bound;

	//Point3f punto = stack.at(0).V //((p1.P() + p2.P()) /2);

	const CMeshO::CoordType &startPt= punto;


	in.push_back(punto);
    
	CMeshO::FaceType   *nearestF=0;
	vcg::face::PointDistanceBaseFunctor PDistFunct;

	
	nearestF =  unifGrid.GetClosest(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);


	out.push_back(closestPt);

	}
	}
*/

}
//
// --- Plugin events methods ---
//
void edit_retoptool::mousePressEvent(QAction *, QMouseEvent * event, MeshModel &m, GLArea * gla) 
{
	mousePos=event->pos();
	click=false;
	gla->update();
}

void edit_retoptool::mouseMoveEvent(QAction *,QMouseEvent * event, MeshModel &m, GLArea * gla)
{
	mousePos=event->pos();
	mouseRealY = gla->curSiz.height() - mousePos.y();
	reDraw=true;
	gla->update();
}

void edit_retoptool::mouseReleaseEvent(QAction *,QMouseEvent * event, MeshModel &, GLArea * gla)
{
	click=true;
	gla->update();
	mousePos=event->pos();
}














//
// --- Mesh coords methods ---
//
int edit_retoptool::getNearest(QPointF center, QPointF *points,int num) {
		int nearestInd=0;
		float dist=fabsf(center.x()-points[0].x())*fabsf(center.x()-points[0].x())+fabsf(center.y()-points[0].y())*fabsf(center.y()-points[0].y());
		for (int lauf=1; lauf<num; lauf++) {
			float temp=fabsf(center.x()-points[lauf].x())*fabsf(center.x()-points[lauf].x())+
				fabsf(center.y()-points[lauf].y())*fabsf(center.y()-points[lauf].y());
			if (temp<dist) {
				nearestInd=lauf;
				dist=temp;
			}
		}
		return nearestInd;
	}

bool edit_retoptool::getFaceAtMouse(MeshModel &m, CMeshO::FacePointer& val) {
	QPoint mid=QPoint(mousePos.x(), mouseRealY);
	return (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, val,2,2));
}

bool edit_retoptool::getVertexAtMouse(MeshModel &m,CMeshO::VertexPointer& value) {
	CFaceO * temp=0;

	QPoint mid=QPoint(mousePos.x(), mouseRealY);
	double tx,ty,tz;
	if (getFaceAtMouse(m,temp)) {
		QPointF point[3];
		for (int lauf=0; lauf<3; lauf++) {
			gluProject(temp->V(lauf)->P()[0],temp->V(lauf)->P()[1],temp->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
			point[lauf]=QPointF(tx,ty);
		}
		value=temp->V(getNearest(mid,point,3));
		return true;
	}
	return false;
}




bool isVertexVisible(Point3f v)
{
	return true;
}


Vtx edit_retoptool::getVisibleVertexNearestToMouse(QList<Vtx> list)
{
	int pCount = list.count();
	Vtx found; 
	found.V = Point3f(0,0,0);
	QPoint mid = QPoint(mousePos.x(), mouseRealY);
	double tx,ty,tz;

	QPointF * point;
	int pointCount = 0;
	point = new QPointF[pCount];

	for (int i=0; i<pCount; i++) 
	{
		Vtx p = list.at(i);
		gluProject(p.V.X(),p.V.Y(),p.V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);

		if(isVertexVisible(p.V))
		{
			point[i] = QPointF(tx, ty);
			pointCount++;
		}
		else
		{
			point[i] = QPointF(0,0);
		} 
	}

	if(pointCount>0)
	{
		QPointF * realPoint;
		realPoint = new QPointF[pointCount];
		int j=0;

		for(int i =0; i<pCount; i++)
		{
			if(point[i]!= QPointF(0,0))
			{
				realPoint[j] = point[i]; 
				j++;
			}
		}

		int nr = getNearest(mid, realPoint, j);
		if(nr < (pointCount))
		{
			found = list.at(nr);
		}
	}
	point = NULL;
	delete[] point;

	return found;
}


float edit_retoptool::distancePointSegment(QPointF p, QPointF segmentP1,QPointF segmentP2)
{
	float x0, y0, x1, x2, y1, y2, m, q;
	
	x1 = segmentP1.x();
	y1 = segmentP1.y();

	x2 = segmentP2.x();
	y2 = segmentP2.y();

	m = (y2-y1)/(x2-x1);
	q = y1 - m*x1;
			
	x0 = p.x();
	y0 = p.y();

	return ((y0 - m*x0 -q) / (sqrt(1 + m*m)));
}
//
// --- Plugin decorations methods ---
//
void edit_retoptool::drawLabel(QList<Vtx> list)
{
	
	QVector<Vtx> v = list.toVector();
	int pCount = list.count();
	double tx,ty,tz;
	for(int i=0; i<pCount; i++)
	{
		gluProject(v[i].V.X(),v[i].V.Y(),v[i].V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
		parentGla->renderText(tx+5, (parentGla->curSiz.height() - 5 - ty), v[i].vName, QFont());
	}

}

void edit_retoptool::drawLabel(Vtx v)
{
	double tx,ty,tz;

	gluProject(v.V.X(),v.V.Y(),v.V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
	parentGla->renderText(tx+5, (parentGla->curSiz.height() - 5 - ty), v.vName, QFont());
}


void edit_retoptool::drawPoint(MeshModel &m, float pSize, Color4b colorBack, Color4b colorFront, Point3f p)
{
	glPushMatrix();
	glMultMatrix(m.cm.Tr);
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST); 
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);

	glColor(colorFront);
		
	glPointSize(pSize);

	glBegin(GL_POINTS);				
		glVertex(p);
		glVertex(p);
	glEnd(); 
			
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);
	glColor(colorBack);
		
	glPointSize(pSize);

	glBegin(GL_POINT);
		glVertex(cursorPoint);
		glVertex(cursorPoint);
	glEnd();
		
	glPopAttrib();
	glPopMatrix();
}


void edit_retoptool::drawPoint(MeshModel &m, float pSize, Color4b colorBack, Color4b colorFront, QList<Vtx> list)
{
	glPushMatrix();
	glMultMatrix(m.cm.Tr);
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
	glLineWidth(2.0f);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST); 
	glDepthMask(GL_FALSE);
		
	glDisable(GL_LIGHTING);
	glColor(colorBack);
		
	glPointSize(pSize);
	
	QVector<Vtx> v = list.toVector();
	int pCount = list.count();

	glBegin(GL_POINTS);
		for(int i=0; i<pCount; i++)
			glVertex(v[i].V);
	glEnd(); 
			
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);
	glColor(colorFront);
		
	glPointSize(pSize);

	glBegin(GL_POINTS);
		for(int i=0; i<pCount; i++)
			glVertex(v[i].V);
	glEnd();
		
	glPopAttrib();
	glPopMatrix();
}


void edit_retoptool::drawLine(MeshModel &m, float pSize, float lSize, Color4b colorBack, Color4b colorFront, Point3f p1, Point3f p2)
{
	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(false);
	glLineWidth(lSize);
	glPointSize(pSize);
  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glColor(colorFront);
    glBegin(GL_LINES);
		glVertex(p1);
		glVertex(p2);
	glEnd();
    glBegin(GL_POINTS);
		glVertex(p1);
		glVertex(p2);
    glEnd();    
	glDisable(GL_DEPTH_TEST);
//    glLineWidth(0.7);
//    glPointSize(1.4);
    glColor(colorBack);
    glBegin(GL_LINES);
		glVertex(p1);
		glVertex(p2);
    glEnd();
    glBegin(GL_POINTS);
		glVertex(p1);
		glVertex(p2);
    glEnd();
  
	glPopAttrib();
}



void edit_retoptool::drawFace(CMeshO::FacePointer fp, MeshModel &m, GLArea * gla)
{
    glPointSize(3.0f);
	
	glBegin(GL_POINTS); //GL_LINE_LOOP);
		glVertex(fp->P(0)); 
		glVertex(fp->P(1));
		glVertex(fp->P(2));
	glEnd();
}











Q_EXPORT_PLUGIN(edit_retoptool)