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
$Log: edit_topo.cpp,v $
****************************************************************************/
#include "edit_topo.h"



#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/complex/trimesh/create/resampler.h>
#include <vcg/simplex/face/distance.h>
#include <vcg/complex/trimesh/update/color.h>






using namespace std;
using namespace vcg;
//
// --- Plugin specific methods ---
//

inline bool pointInTriangle(const QPointF &p, const QPointF &a, const QPointF &b, const QPointF &c) {
															float fab=(p.y()-a.y())*(b.x()-a.x()) - (p.x()-a.x())*(b.y()-a.y());
															float fbc=(p.y()-c.y())*(a.x()-c.x()) - (p.x()-c.x())*(a.y()-c.y());
															float fca=(p.y()-b.y())*(c.x()-b.x()) - (p.x()-b.x())*(c.y()-b.y());
															if (fab*fbc>0 && fbc*fca>0)
																return true;
															return false;
}



edit_topo::edit_topo() 
{
	edit_topodialogobj=0;
	reDraw = false;
	click = false;
	first_model_generated=false;

	nameVtxCount = 0;
	stack.clear();
	Estack.clear();

	drag_click=false;
	drag_stack.clear();	

	lastPoint.V = Point3f(0,0,0);
	lastPoint.vName = "--";
	
	connectStart.V = Point3f(0,0,0);
	connectStart.vName = "--";
	connectEnd.V = Point3f(0,0,0);
	connectEnd.vName = "--";

	actionList << new QAction(QIcon(":/images/icon_topo.png"), "Re-Topology Tool", this);
	foreach(QAction *editAction, actionList)
		editAction->setCheckable(true);
}


edit_topo::~edit_topo() 
{
	stack.clear();
	Estack.clear();
	Fstack.clear();
	delete edit_topodialogobj;
	edit_topodialogobj = 0;
}


QList<QAction *> edit_topo::actions() const 
{
  return actionList;
}

const QString edit_topo::Info(QAction *action) 
{
  if( action->text() != tr("ReTop Tool") ) assert (0);
  return tr("Allow to re-top a model");
}

const PluginInfo &edit_topo::Info() 
{
   static PluginInfo ai; 
   ai.Date=tr(__DATE__);
   ai.Version = tr("0.1");
   ai.Author = ("Paolo Cignoni, Daniele Bonetta");
   return ai;
}
void edit_topo::Decorate(QAction *, MeshModel &m, GLArea * gla)
{
	updateMatrixes();
	// onClick
	if(click)
	{
		click=false;

		//---
		// Existing vertex selection mode (yellow pointer)
		//---
		if(edit_topodialogobj->utensil==U_VTX_SEL)
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
						edit_topodialogobj->updateVtxTable(stack);
					}
					else nameVtxCount--;
				}
			}
		}


		//---
		// Free vertex selection (no pointer)
		//---
		if(edit_topodialogobj->utensil==U_VTX_SEL_FREE)
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
						edit_topodialogobj->updateVtxTable(stack);
					}
					else nameVtxCount--;
				}
			}
		}


		//---
		// Remove selected vertex mode (Yellow pointer)
		//---
		if(edit_topodialogobj->utensil==U_VTX_DEL)
		{
			Vtx vtx = getVisibleVertexNearestToMouse(stack);

			if(vtx.V != Point3f(0,0,0))
			{
				// Vertex del
				for(int i=0; i<stack.count(); i++)
					if(stack.at(i) == vtx)
					{
						stack.removeAt(i);
						edit_topodialogobj->updateVtxTable(stack);
					}

				int EtoDel = 0;
				int FtoDel = 0;

				// And delete all the edges and faces where it's present
				for(int e=0; e<Estack.count(); e++)
					if(Estack.at(e).containsVtx(vtx))
						EtoDel++;
				for(int e=0; e<Fstack.count(); e++)
					if(Fstack.at(e).containsVtx(vtx))
						FtoDel++;

				for(int i=0; i<EtoDel; i++)
				{	bool del = false;
					for(int e=0; e<Estack.count(); e++)
						if((Estack.at(e).containsVtx(vtx))&&(!del))
						{
							Estack.removeAt(e);
							del = true;
						}}

				for(int i=0; i<FtoDel; i++)
				{	bool del = false;			
					for(int f=0; f<Fstack.count(); f++)
						if((Fstack.at(f).containsVtx(vtx))&&(!del))
						{
							Fstack.removeAt(f);
							del = true;
						}}

				if(stack.count()==0)
					nameVtxCount = 0;
			}
		}


		//---
		// Edge mode (vertex connection)
		//---
		if(edit_topodialogobj->utensil==U_VTX_CONNECT)
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
						edit_topodialogobj->updateEdgTable(Estack);

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
									toAdd.selected = true;

									if(!Fstack.contains(toAdd))
									{
										Fstack.push_back(toAdd);

										QList<QString> vNames;
										for(int n=0; n<3; n++)
											for(int m=0; m<2; m++)
												if(!vNames.contains(toAdd.e[n].v[m].vName))
													vNames.push_back(toAdd.e[n].v[m].vName);

										edit_topodialogobj->updateFceTable(Fstack);
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


		//---
		// Face Select/Deselect		
		//---
		if(edit_topodialogobj->utensil==U_FCE_SEL)
		{
			Fce nearest;
			bool got = false;
			double tx,ty,tz;
			int at = 0;
			for(int f=0; f<Fstack.count(); f++)
			{
				Fce fc = Fstack.at(f);
	
				QList<Vtx> allv;
				for(int e=0; e<3; e++)
					for(int v=0; v<2; v++)
						if(!allv.contains(fc.e[e].v[v]))
							allv.push_back(fc.e[e].v[v]);

				
				gluProject(allv.at(0).V.X(),allv.at(0).V.Y(),allv.at(0).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p0 = QPointF(tx, ty);
				gluProject(allv.at(1).V.X(),allv.at(1).V.Y(),allv.at(1).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p1 = QPointF(tx, ty);
				gluProject(allv.at(2).V.X(),allv.at(2).V.Y(),allv.at(2).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p2 = QPointF(tx, ty);

				QPoint p = QPoint(mousePos.x(), mouseRealY);

				if(pointInTriangle(p, p0, p1, p2))
				{
					nearest = fc;
					got = true;
					at = f;
				}
			}

			if(got)
			{
				Fstack.removeAt(at);
				nearest.selected=!nearest.selected;
				Fstack.push_back(nearest);
			}		
		}


		//---
		// edge deselection mode
		//---
		if(edit_topodialogobj->utensil==U_VTX_DE_CONNECT)
		{
			Edg minE;
			// Remove the edge from his stack, and also from the faces stack
			if(getVisibleEdgeNearestToMouse(Estack, minE))
			{
				for(int at=0; at<Estack.count(); at++)
					if(Estack.at(at)==minE)
						Estack.removeAt(at);

				int toDel = 0;
				for(int f=0; f<Fstack.count(); f++)
					if(Fstack.at(f).containsEdg(minE))
						toDel++;
			
				for(int i=0; i<toDel; i++)
				{	bool del = false;
					for(int f=0; f<Fstack.count(); f++)
						if((Fstack.at(f).containsEdg(minE))&&(!del))
						{
							Fstack.removeAt(f);
							del = true;
						}}

				edit_topodialogobj->updateVtxTable(stack);
				edit_topodialogobj->updateFceTable(Fstack);
				edit_topodialogobj->updateEdgTable(Estack);
			}
		}


		//---
		// Vertex moove mode
		//---
		if(edit_topodialogobj->utensil==U_DND)
		{
			drag_stack.clear();
			if(!drag_click)
			{
				drag_click = true;

				Vtx vtx = getVisibleVertexNearestToMouse(stack);
				drag_vtx = vtx;

				for(int f=0; f<Fstack.count(); f++)
				{
					Fce fc = Fstack.at(f);
					if(fc.containsVtx(vtx))
						drag_stack.push_back(fc);
				}
			}
			else
			{
				drag_click = false;

				// Get new vertex coords
				Point3f temp_vert;
				if (Pick(mousePos.x(), mouseRealY, temp_vert))
				{
					Vtx newV;
					newV.V = temp_vert;
					newV.vName = drag_vtx.vName;

					for(int v=0; v<stack.count(); v++)
						if(stack.at(v)==drag_vtx)
							stack.removeAt(v);

					stack.push_back(newV);

					int edgToRemove=0;	
					for(int e=0; e<Estack.count(); e++)
						if(Estack.at(e).containsVtx(drag_vtx))
							edgToRemove++;

					for(int i=0; i<edgToRemove; i++)
						for(int e=0; e<Estack.count(); e++)
							if(Estack.at(e).containsVtx(drag_vtx))
							{
								Edg newE = Estack.at(e);
								if(newE.v[0].V==drag_vtx.V)
									newE.v[0]=newV;
								if(newE.v[1].V==drag_vtx.V)
									newE.v[1]=newV;
								Estack.removeAt(e);
								Estack.push_back(newE);
							}

					int fceToRemove=0;
					for(int f=0; f<Fstack.count(); f++)
						if(Fstack.at(f).containsVtx(drag_vtx))
							fceToRemove++;

					for(int i=0; i<fceToRemove; i++)
						for(int f=0; f<Fstack.count(); f++)
							if(Fstack.at(f).containsVtx(drag_vtx))
							{
								Fce newF = Fstack.at(f);
								for(int e=0; e<3; e++)
									for(int v=0; v<2; v++)
										if(newF.e[e].v[v].V == drag_vtx.V)
											newF.e[e].v[v] = newV;
								Fstack.removeAt(f);
								Fstack.push_back(newF);
							}

					edit_topodialogobj->updateVtxTable(stack);
					edit_topodialogobj->updateFceTable(Fstack);
					edit_topodialogobj->updateEdgTable(Estack);
				
					if(first_model_generated)
						on_mesh_create();
				}		
			}
		}


		//---
		// edge split mode
		//---
		if(edit_topodialogobj->utensil==U_EDG_SPLIT)
		{
			Edg minE;
			if(getVisibleEdgeNearestToMouse(Estack, minE))
			{
				Vtx newVtx;
				Point3f new3f;
				
				double tx,ty,tz;
				gluProject(minE.v[0].V.X(),minE.v[0].V.Y(),minE.v[0].V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p0 = QPointF(tx, ty);
				gluProject(minE.v[1].V.X(),minE.v[1].V.Y(),minE.v[1].V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p1 = QPointF(tx, ty);

				QPointF Qmid = (p1 + p0)/2;

				if(Pick(Qmid.x(), Qmid.y(), new3f))
				{
					newVtx.V = new3f;
					newVtx.vName = QString("V%1").arg(nameVtxCount++);
				
					int fCount = 0;
					for(int f=0; f<Fstack.count(); f++)
						if(Fstack.at(f).containsEdg(minE))
							fCount++;
				
					for(int i=0; i<fCount; i++)
					{
						bool found=false;
						for(int f=0; ((f<Fstack.count())&&(!found)); f++)
						{
							Fce fc = Fstack.at(f);
							if(fc.containsEdg(minE))
							{
								Fce newF1 = fc;
								Fce newF2 = fc;

								Vtx oldV1 = minE.v[0];
								Vtx oldV2 = minE.v[1];

								// Edit new face 01 and 02
								for(int e=0; e<3; e++)
									for(int v=0; v<2; v++)
									{
										if(newF1.e[e].v[v]==oldV1)
											newF1.e[e].v[v]=newVtx;
										if(newF2.e[e].v[v]==oldV2)
											newF2.e[e].v[v]=newVtx;
									}
	
								Edg newEdgMid;
								QList<Vtx> allv;
								for(int e=0; e<3; e++)
									for(int v=0; v<2; v++)
										if(!allv.contains(fc.e[e].v[v]))
											allv.push_back(fc.e[e].v[v]);
							
								Vtx oldVtx;
								for(int i=0; i<3; i++)
									if((allv.at(i)!=oldV1)&&(allv.at(i)!=oldV2))
										oldVtx=allv.at(i);
							
								newEdgMid.v[0]=oldVtx;
								newEdgMid.v[1]=newVtx;
	
								Estack.push_back(newEdgMid);
								Fstack.removeAt(f);
								Fstack.push_back(newF1);
								Fstack.push_back(newF2);

								found = true;
							}
						}
					}
	
					Edg newEdg1 = minE;
					Edg newEdg2 = minE;

					newEdg1.v[0] = newVtx;
					newEdg2.v[1] = newVtx;

					for(int e=0; e<Estack.count(); e++)
						if(Estack.at(e)==minE)
							Estack.removeAt(e);

					Estack.push_back(newEdg1);
					Estack.push_back(newEdg2);

					stack.push_back(newVtx);

					edit_topodialogobj->updateVtxTable(stack);
					edit_topodialogobj->updateFceTable(Fstack);
					edit_topodialogobj->updateEdgTable(Estack);
				}
			}		
		}


		//---
		// edge collapse mode
		//---
		if(edit_topodialogobj->utensil==U_EDG_COLLAPSE)
		{
			Edg toColl;
			if(getVisibleEdgeNearestToMouse(Estack, toColl))
			{
				Vtx oldVtx1 = toColl.v[0];
				Vtx oldVtx2 = toColl.v[1];

				Vtx newVtx;
				Point3f new3f;
				
				double tx,ty,tz;
				gluProject(oldVtx1.V.X(),oldVtx1.V.Y(),oldVtx1.V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p0 = QPointF(tx, ty);
				gluProject(oldVtx2.V.X(),oldVtx2.V.Y(),oldVtx2.V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p1 = QPointF(tx, ty);

				QPointF Qmid = (p1 + p0)/2;

				if(Pick(Qmid.x(), Qmid.y(), new3f))
				{
					newVtx.V = new3f;
					newVtx.vName = QString("V%1").arg(nameVtxCount++);

					// Remove faces containing the coll edge
					int times = 0;
					for(int f=0; f<Fstack.count(); f++)
						if(Fstack.at(f).containsEdg(toColl))
							times++;

					for(int i=0; i<times; i++)
						for(int f=0; f<Fstack.count(); f++)
							if(Fstack.at(f).containsEdg(toColl))
								Fstack.removeAt(f);	

					// Update faces stack with the new vertex
					times = 0;
					for(int f=0; f<Fstack.count(); f++)
						if(Fstack.at(f).containsVtx(oldVtx1)||Fstack.at(f).containsVtx(oldVtx2))
							times++;

					for(int i=0; i<times; i++)
						for(int f=0; f<Fstack.count(); f++)
							if(Fstack.at(f).containsVtx(oldVtx1)||Fstack.at(f).containsVtx(oldVtx2))
							{
								Fce fc = Fstack.at(f);
								for(int e=0; e<3; e++)
									for(int v=0; v<2; v++)
										if((fc.e[e].v[v]==oldVtx1)||(fc.e[e].v[v]==oldVtx2))
											fc.e[e].v[v] = newVtx;

								Fstack.removeAt(f);
								Fstack.push_back(fc);
							}

					// Update edges stack
					times = 0;
					for(int e=0; e<Estack.count(); e++)
						if(Estack.at(e).containsVtx(oldVtx1)||Estack.at(e).containsVtx(oldVtx2))
							times++;

					for(int i=0; i<times; i++)
						for(int e=0; e<Estack.count(); e++)					
							if(Estack.at(e).containsVtx(oldVtx1)||Estack.at(e).containsVtx(oldVtx2))
							{
								Edg ed = Estack.at(e);
								for(int v=0; v<2; v++)
									if((ed.v[v]==oldVtx1)||(ed.v[v]==oldVtx2))
										ed.v[v]=newVtx;

								Estack.removeAt(e);
								if(!Estack.contains(ed))
									if(ed.v[0]!=ed.v[1])
										Estack.push_back(ed);
							}					

					// Update vtx stack;
					for(int i=0; i<2; i++)
						for(int v=0; v<stack.count(); v++)
							if((stack.at(v)==oldVtx1)||(stack.at(v)==oldVtx2))
								stack.removeAt(v);
					stack.push_back(newVtx);

					edit_topodialogobj->updateVtxTable(stack);
					edit_topodialogobj->updateFceTable(Fstack);
					edit_topodialogobj->updateEdgTable(Estack);
				}
			}
		}
	}// end if click

	
	
	
	
	
	/*** Decorations ***/
	

	//---
	// Even if there's something selected: show edges	
	//---
	if((edit_topodialogobj->utensil==U_VTX_CONNECT)||(edit_topodialogobj->utensil==U_VTX_DE_CONNECT)
			||(edit_topodialogobj->utensil==U_DND)||(edit_topodialogobj->utensil==U_EDG_SPLIT)
			||(edit_topodialogobj->utensil==U_EDG_COLLAPSE))
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


	//---
	// First step: show only vertices
	//---
	if((edit_topodialogobj->utensil==U_VTX_SEL_FREE)||(edit_topodialogobj->utensil==U_VTX_DEL)
		||(edit_topodialogobj->utensil==U_VTX_SEL))
	{
		if(stack.count()!=0)
			drawPoint(m, 3.0f, Color4b::DarkRed, Color4b::Red, stack);

		if(stack.count()!=0)
			drawLabel(stack);	
	}


	//---
	// Drag and drop vertices mode
	//---
	if(edit_topodialogobj->utensil==U_DND)
	{
		if(drag_click)
		{
			Point3f pmouse;
			if(Pick(mousePos.x(), mouseRealY, pmouse))
			{			
				for(int i=0; i<drag_stack.count(); i++)
				{
					Fce fc = drag_stack.at(i);

					QList<Vtx> allv;
					for(int e=0; e<3; e++)
						for(int v=0; v<2; v++)
							if(!allv.contains(fc.e[e].v[v]))
								allv.push_back(fc.e[e].v[v]);

					QVector<Vtx> v = allv.toVector();
				
					for(int i=0; i<3; i++)
						if(v[i] == drag_vtx)
							v[i].V = pmouse;

					drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Yellow, v[0].V, v[1].V);
					drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Yellow, v[1].V, v[2].V);
					drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Yellow, v[2].V, v[0].V);
				}
			}
		}
		else
		{
			Vtx vtx = getVisibleVertexNearestToMouse(stack);

			if(vtx.V != Point3f(0,0,0))
				drawPoint(m, 4.0f, Color4b::Yellow, Color4b::Yellow, vtx.V);

		}
	}


	//---
	// Face selection mode
	//---
	if(edit_topodialogobj->utensil==U_FCE_SEL)
	{
		// DEBUG
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
		// /DEBUG

		if(Fstack.count()!=0)
		{
			bool got = false;
			Fce nearest;
			for(int i=0; i<Fstack.count(); i++)
			{
				Fce f = Fstack.at(i);

				QList<Vtx> allv;
				for(int e=0; e<3; e++)
					for(int v=0; v<2; v++)
						if(!allv.contains(f.e[e].v[v]))
							allv.push_back(f.e[e].v[v]);

				if(f.selected)
				{
					drawLine(m, 2.0f, 3.0f, Color4b::Blue, Color4b::Green, allv.at(0).V, allv.at(1).V);
					drawLine(m, 2.0f, 3.0f, Color4b::Blue, Color4b::Green, allv.at(1).V, allv.at(2).V);
					drawLine(m, 2.0f, 3.0f, Color4b::Blue, Color4b::Green, allv.at(2).V, allv.at(0).V);	

					Point3f mid = (allv.at(0).V + allv.at(1).V + allv.at(2).V) / 3;
					drawPoint(m, 5.0f, Color4b::DarkGreen, Color4b::Green, mid);
				}
				else
				{
					drawLine(m, 2.0f, 3.0f, Color4b::DarkRed, Color4b::Red, allv.at(0).V, allv.at(1).V);
					drawLine(m, 2.0f, 3.0f, Color4b::DarkRed, Color4b::Red, allv.at(1).V, allv.at(2).V);
					drawLine(m, 2.0f, 3.0f, Color4b::DarkRed, Color4b::Red, allv.at(2).V, allv.at(0).V);	
				}
			}

			double tx,ty,tz;
			for(int f=0; f<Fstack.count(); f++)
			{
				Fce fc = Fstack.at(f);
	
				QList<Vtx> allv;
				for(int e=0; e<3; e++)
					for(int v=0; v<2; v++)
						if(!allv.contains(fc.e[e].v[v]))
							allv.push_back(fc.e[e].v[v]);

				
				gluProject(allv.at(0).V.X(),allv.at(0).V.Y(),allv.at(0).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p0 = QPointF(tx, ty);
				gluProject(allv.at(1).V.X(),allv.at(1).V.Y(),allv.at(1).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p1 = QPointF(tx, ty);
				gluProject(allv.at(2).V.X(),allv.at(2).V.Y(),allv.at(2).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p2 = QPointF(tx, ty);

				QPoint p = QPoint(mousePos.x(), mouseRealY);

				if(pointInTriangle(p, p0, p1, p2))
				{
					nearest = fc;
					got = true;
				}
			}

			if(got)
			{
				QList<Vtx> allv;
				for(int e=0; e<3; e++)
					for(int v=0; v<2; v++)
						if(!allv.contains(nearest.e[e].v[v]))
							allv.push_back(nearest.e[e].v[v]);

				drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Red, allv.at(0).V, allv.at(1).V);
				drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Red, allv.at(1).V, allv.at(2).V);
				drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Red, allv.at(2).V, allv.at(0).V);	
			}
		}
	}


	//---
	// Yellow pointer (vertex selection)
	//---
	if(edit_topodialogobj->utensil==U_VTX_SEL)
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


	//---
	// Vertex de-selection (Yellow pointeR)
	//---
	if((edit_topodialogobj->utensil==U_VTX_DEL)&&(stack.count()!=0))
	{
		Vtx vtx = getVisibleVertexNearestToMouse(stack);

		if(vtx.V != Point3f(0,0,0))
			drawPoint(m, 4.0f, Color4b::DarkGreen, Color4b::Green, vtx.V);
	}


	//---
	// edge mode (vtx connection)
	//---
	if(edit_topodialogobj->utensil==U_VTX_CONNECT)
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


	//---
	// edge deselection mode
	//---
	if(edit_topodialogobj->utensil==U_VTX_DE_CONNECT)
	{
		Edg minE;
		if(getVisibleEdgeNearestToMouse(Estack, minE))
			drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Green, minE.v[0].V, minE.v[1].V);
	}


	//---
	// edge split mode
	//---
	if(edit_topodialogobj->utensil==U_EDG_SPLIT)
	{
		Edg minE;
		if(getVisibleEdgeNearestToMouse(Estack, minE))
			drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Green, minE.v[0].V, minE.v[1].V);
	}


	//---
	// edge collapse mode
	//---
	if(edit_topodialogobj->utensil==U_EDG_COLLAPSE)
	{
		Edg minE;
		if(getVisibleEdgeNearestToMouse(Estack, minE))
			drawLine(m, 2.0f, 3.0f, Color4b::Yellow, Color4b::Green, minE.v[0].V, minE.v[1].V);
	}




}









































void edit_topo::StartEdit(QAction *, MeshModel &m, GLArea *gla)
{	
	parentGla = gla;
	gla->setCursor(QCursor(QPixmap(":/images/cursor_paint.png"),1,1));	

	// Init uniform grid
	float dist = m.cm.bbox.Diag();//10; //trgMesh ???//edit_topodialogobj->dist(0);

	m.updateDataMask(MeshModel::MM_FACEMARK);
	tri::UpdateNormals<CMeshO>::PerFaceNormalized(m.cm);
	tri::UpdateFlags<CMeshO>::FaceProjection(m.cm);

	rm.init(&m, dist);



	/*if (edit_topodialogobj==0)
		edit_topodialogobj=new edit_topodialog(parent->window()); */

// Create an istance of the interface
	if (edit_topodialogobj == 0) 
	{ 
		edit_topodialogobj = new edit_topodialog(gla->window()); 
		dock = new QDockWidget(gla->window());
		dock->setAllowedAreas(Qt::NoDockWidgetArea);
		dock->setWidget(edit_topodialogobj);
		QPoint p = gla->window()->mapToGlobal(QPoint(0,0));
		dock->setGeometry(-5+p.x()+gla->window()->width()-edit_topodialogobj->width(),p.y(),edit_topodialogobj->width(),edit_topodialogobj->height());
		dock->setFloating(true);
	}
	dock->setVisible(true);
	dock->layout()->update();	

	// Initialize the texture using the intere model
	// InitTexture(m);

	gla->update();


	gla->setMouseTracking(true);
//	edit_topodialogobj->show();

	connect(edit_topodialogobj, SIGNAL( mesh_create() ),
          this, SLOT( on_mesh_create() ) );

	connect(edit_topodialogobj, SIGNAL( fuffa() ),
          this, SLOT( on_fuffa() ) );
}

void edit_topo::EndEdit(QAction *, MeshModel &, GLArea *)
{
	stack.clear();
	Estack.clear();
	Fstack.clear();
	delete edit_topodialogobj;
	edit_topodialogobj = 0;
}



void edit_topo::on_mesh_create()
{
	in.clear();
	out.clear();

	if(first_model_generated)
	{
		parentGla->meshDoc.meshList.pop_back();
	}

	MeshModel *mm= new MeshModel();
	parentGla->meshDoc.meshList.push_back(mm);
	first_model_generated = true;


	MeshModel *m     = parentGla->meshDoc.meshList.back();	// destination = last
	MeshModel *currentMesh  = parentGla->meshDoc.mm();		// source = current		


// DEBUG: Force MY vertex params
/*
	Vtx v1;
	v1.V = Point3f(	-2.81, 7.59, -1496.97);
	v1.vName = QString("V0");

	Vtx v2;
	v2.V = Point3f(-11.42, -100.35, -1503.1);
	v2.vName = QString("V1");

	Vtx v3;
	v3.V = Point3f(	29.21, -47.72, -1517.43);
	v3.vName = QString("V2");

	Edg e1;
	e1.v[0] = v1;
	e1.v[1] = v2;

	Edg e2;
	e2.v[0] = v2;
	e2.v[1] = v3;

	Edg e3;
	e3.v[0] = v3;
	e3.v[1] = v1;

	Fce f;
	f.e[0] = e1;
	f.e[1] = e2;
	f.e[2] = e3;

	stack.clear();
	stack.push_back(v1);stack.push_back(v2);stack.push_back(v3);
	Estack.clear();
	Estack.push_back(e1);Estack.push_back(e2);Estack.push_back(e3);
	Fstack.clear();
	Fstack.push_back(f);*/
	// /DEBUG!!!!!

	// Mesh creation




	double dist = m->cm.bbox.Diag();//10; //trgMesh ???//edit_topodialogobj->dist(0);


	if(edit_topodialogobj->isDEBUG())
	{
		rm.Lin.clear();
		rm.Lout.clear();

		int iter = edit_topodialogobj->getIterations();
		rm.midSampler->dist_upper_bound = edit_topodialogobj->dist();
		rm.createRefinedMesh(*m, *currentMesh, dist, iter, Fstack, stack, edit_topodialogobj, true);
		in = rm.Lin;
		out = rm.Lout;
	}
	else
	{
		int iter = edit_topodialogobj->getIterations();
		rm.createRefinedMesh(*m, *currentMesh, dist, iter, Fstack, stack, edit_topodialogobj, false);
	}

	m->cm.Tr = currentMesh->cm.Tr;	

	parentGla->update();
}









// DEBUG!!!
void edit_topo::on_fuffa()
{
	parentGla->update();

	
	/*
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
void edit_topo::mousePressEvent(QAction *, QMouseEvent * event, MeshModel &m, GLArea * gla) 
{
	mousePos=event->pos();
	click=false;
	gla->update();
}

void edit_topo::mouseMoveEvent(QAction *,QMouseEvent * event, MeshModel &m, GLArea * gla)
{
	mousePos=event->pos();
	mouseRealY = gla->curSiz.height() - mousePos.y();
	reDraw=true;
	gla->update();
}

void edit_topo::mouseReleaseEvent(QAction *,QMouseEvent * event, MeshModel &, GLArea * gla)
{
	click=true;
	gla->update();
	mousePos=event->pos();
	reDraw=true;
}














//
// --- Mesh coords methods ---
//
int edit_topo::getNearest(QPointF center, QPointF *points,int num) {
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

bool edit_topo::getFaceAtMouse(MeshModel &m, CMeshO::FacePointer& val) {
	QPoint mid=QPoint(mousePos.x(), mouseRealY);
	return (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, val,2,2));
}

bool edit_topo::getVertexAtMouse(MeshModel &m,CMeshO::VertexPointer& value) {
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


Vtx edit_topo::getVisibleVertexNearestToMouse(QList<Vtx> list)
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


bool edit_topo::getVisibleEdgeNearestToMouse(QList<Edg> listE, Edg &ret)
{

			Fce nearest;
			bool got = false;
			double tx,ty,tz;
			int at = 0;
			for(int f=0; f<Fstack.count(); f++)
			{
				Fce fc = Fstack.at(f);
	
				QList<Vtx> allv;
				for(int e=0; e<3; e++)
					for(int v=0; v<2; v++)
						if(!allv.contains(fc.e[e].v[v]))
							allv.push_back(fc.e[e].v[v]);

				
				gluProject(allv.at(0).V.X(),allv.at(0).V.Y(),allv.at(0).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p0 = QPointF(tx, ty);
				gluProject(allv.at(1).V.X(),allv.at(1).V.Y(),allv.at(1).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p1 = QPointF(tx, ty);
				gluProject(allv.at(2).V.X(),allv.at(2).V.Y(),allv.at(2).V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
				QPointF p2 = QPointF(tx, ty);

				QPoint p = QPoint(mousePos.x(), mouseRealY);

				if(pointInTriangle(p, p0, p1, p2))
				{
					nearest = fc;
					got = true;
					at = f;
				}
			}

			if(got)
			{
				Edg minE;
				float bestD = -1;
				bool found = false;
				double tx,ty,tz;

				for(int i=0; i<3; i++)
				{
					Edg e = nearest.e[i];
		
					gluProject(e.v[0].V.X(),e.v[0].V.Y(),e.v[0].V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
					QPointF p0 = QPointF(tx, ty);
					gluProject(e.v[1].V.X(),e.v[1].V.Y(),e.v[1].V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
					QPointF p1 = QPointF(tx, ty);

					float dist = distancePointSegment(QPointF(mousePos.x(),mouseRealY), p1, p0);
					if((dist < bestD)||(bestD==-1))
					{
						bestD = dist;
						minE = e;
						found = true;
					}
				}
	
				ret = minE;
				return found;
			}

			return false;


/*	Edg minE;
	float bestD = -1;
	bool found = false;
	double tx,ty,tz;

	for(int i=0; i<listE.count(); i++)
	{
		Edg e = listE.at(i);
		
		gluProject(e.v[0].V.X(),e.v[0].V.Y(),e.v[0].V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
		QPointF p0 = QPointF(tx, ty);
		gluProject(e.v[1].V.X(),e.v[1].V.Y(),e.v[1].V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
		QPointF p1 = QPointF(tx, ty);

		float dist = distancePointSegment(QPointF(mousePos.x(),mouseRealY), p1, p0);
		if((dist < bestD)||(bestD==-1))
		{
			bestD = dist;
			minE = e;
			found = true;
		}
	}
	
	ret = minE;
	return found; */
}
float edit_topo::distancePointSegment(QPointF p, QPointF segmentP1,QPointF segmentP2)
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

	return fabs((y0 - m*x0 -q) / (sqrt(1 + m*m)));
}
float edit_topo::distancePointPoint(QPointF P1, QPointF P2)
{ 	
	return sqrt(pow((P1.x()-P2.x()),2)+pow((P1.y()-P2.y()),2));
}
//
// --- Plugin decorations methods ---
//
void edit_topo::drawLabel(QList<Vtx> list)
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

void edit_topo::drawLabel(Vtx v)
{
	double tx,ty,tz;

	gluProject(v.V.X(),v.V.Y(),v.V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
	parentGla->renderText(tx+5, (parentGla->curSiz.height() - 5 - ty), v.vName, QFont());
}


void edit_topo::drawPoint(MeshModel &m, float pSize, Color4b colorBack, Color4b colorFront, Point3f p)
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


void edit_topo::drawPoint(MeshModel &m, float pSize, Color4b colorBack, Color4b colorFront, QList<Vtx> list)
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


void edit_topo::drawLine(MeshModel &m, float pSize, float lSize, Color4b colorBack, Color4b colorFront, Point3f p1, Point3f p2)
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
    glColor(colorBack);
    glBegin(GL_LINES);
		glVertex(p1);
		glVertex(p2);
	glEnd();
    glBegin(GL_POINTS);
		glVertex(p1);
		glVertex(p2);
    glEnd();    
	glDisable(GL_DEPTH_TEST);

	glLineWidth(0.7);
    glPointSize(1.4);
  
	//glColor(colorFront);
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


void edit_topo::drawFace(CMeshO::FacePointer fp, MeshModel &m, GLArea * gla)
{
    glPointSize(3.0f);
	
	glBegin(GL_POINTS); //GL_LINE_LOOP);
		glVertex(fp->P(0)); 
		glVertex(fp->P(1));
		glVertex(fp->P(2));
	glEnd();
}











Q_EXPORT_PLUGIN(edit_topo)