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
// --- "Edit plugin" specific methods ---
//



//
//	Edit plugin constructor
//
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
}

//
//	Edit plugin destructor
//
edit_topo::~edit_topo() 
{
	stack.clear();
	Estack.clear();
	Fstack.clear();


	if (edit_topodialogobj != 0) 
	{
		delete edit_topodialogobj;
		edit_topodialogobj = 0;
	}

}

const QString edit_topo::Info() 
{
  return tr("Allow to re-top a model");
}

/************************************************************************************/
//
// -- Edit topology Methods:
//		Those methods are used by the plugin to edit
//		the new topology defined by the user
//		
//		All of the methods here are directly used to build
//		the base topology mesh as defined by the user
//

//
//	Adds a vertex to the new topo vertices list
//		The new vertex will be selected from one of the existing mesh
//
void edit_topo::editAddVertex(MeshModel &m)
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

//
//	Adds a vertex to the new topo vertices list
//		The new vertex is freely selected over the existing mesh
//
void edit_topo::editAddVertexFree()
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

//
//	Removes a vertex from the new topo vertices list
//		The operation will affect also Edges and Faces lists
//
void edit_topo::editDeleteVertex()
{
	Vtx vtx;
	if(getVisibleVertexNearestToMouse(stack, vtx))
	{
		// Remove vertex from Vert list
		for(int i=0; i<stack.count(); i++)
			if(stack.at(i) == vtx)
			{
				stack.removeAt(i);
				edit_topodialogobj->updateVtxTable(stack);
			}

		int EtoDel = 0;
		int FtoDel = 0;

		// And delete all the edges and faces where this vertex is present
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
					edit_topodialogobj->updateEdgTable(Estack);
					del = true;
				}}

		for(int i=0; i<FtoDel; i++)
		{	bool del = false;			
			for(int f=0; f<Fstack.count(); f++)
				if((Fstack.at(f).containsVtx(vtx))&&(!del))
				{
					Fstack.removeAt(f);
					edit_topodialogobj->updateFceTable(Fstack);
					del = true;
				}}
			if(stack.count()==0)
				nameVtxCount = 0;
		}
}

//
//	Connects two vertices and creates a new edge
//		The operation will also create a new face in the faces list
//		if the selected edge is connected to other two edges
//
void edit_topo::editConnectVertex()
{
	if(connectStart.V==Point3f(0,0,0))
	{
		Vtx vtx;
		// First click
		if(getVisibleVertexNearestToMouse(stack, vtx))
		{
			connectStart.V = vtx.V;
			connectStart.vName = vtx.vName;
		}	
	}
	else
	{	
		Vtx vtx;
		// Second click (insert the edge in the Ed. list)
		if(getVisibleVertexNearestToMouse(stack, vtx))
		if(vtx.V != connectStart.V)
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

				//--> Check if the "just now" selected edge is connected to others. Then, create a new face
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


//
//	Selects or deselects the face selected by the user
//
void edit_topo::editSelectFace()
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

//
//	Removes an edge from the edge's list
//		The operation will also remove each face using that
//		edge (if present), but will not remove any vertex
//
void edit_topo::editDeconnectEdge()
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


//
//	Manage the drag&drop process used to move a vertex to a new
//		position as selected with mouse
//		The method will delete old vertex and insert a new vertex,
//		and also adjourn each edge/face that is using it
//
void edit_topo::editDragAndDropVertex()
{
	drag_stack.clear();
	if(!drag_click)
	{
		// First click: select vertex
		drag_click = true;

		Vtx vtx;
		if(getVisibleVertexNearestToMouse(stack, vtx))
		{
			drag_vtx = vtx;

			for(int f=0; f<Fstack.count(); f++)
			{
				Fce fc = Fstack.at(f);
				if(fc.containsVtx(vtx))
					drag_stack.push_back(fc);
			}
		} 
		else 
			drag_click = false;
	}
	else
	{
		// second click: new vertex position
		drag_click = false;

		// Get new vertex coords
		Point3f temp_vert;
		if (Pick(mousePos.x(), mouseRealY, temp_vert))
		{
			// remove the old vertex
			Vtx newV;
			newV.V = temp_vert;
			newV.vName = drag_vtx.vName;
			for(int v=0; v<stack.count(); v++)
				if(stack.at(v)==drag_vtx)
					stack.removeAt(v);

			// add the new vertex
			stack.push_back(newV);

			// count the number of edges involved
			int edgToRemove=0;
			for(int e=0; e<Estack.count(); e++)
				if(Estack.at(e).containsVtx(drag_vtx))
					edgToRemove++;
			
			// edit each edge
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

			// count faces number
			int fceToRemove=0;
			for(int f=0; f<Fstack.count(); f++)
				if(Fstack.at(f).containsVtx(drag_vtx))
					fceToRemove++;

			// update faces with new vertex and edges
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

		/* Uncomment this if you want the mesh to be auto-recreated
			if(first_model_generated)
				on_mesh_create();*/
		}		
	}
}


//
//	Split an edge on its mid point
//		The process will remove the original edge and add two new
//		faces. This will also add a new vertex and create (eventually)
//		two new faces.
//
void edit_topo::editEdgeSplit()
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

//
//	Collapse an edge over its mid point
//		This will also remove faces using the old edge
//
void edit_topo::editEdgeCollapse()
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




/************************************************************************************/
//
// -- Decoration Plugin methods
//			Those methods are used by the plugin to draw decorations
//			on each plugin's user states
//
//			Each method declared here is used only in decorations functions,
//			so, each method here will be called by the Decorate() func


//
//	Standard decoration mode
//		Draws points and edges as defined by the user.
//		And, also, vertices labels
//
void edit_topo::editDecoStandard(MeshModel &m)
{
	if(stack.count()!=0)
		drawPoint(m, 3.0f, Color4b::Red, stack);

	if(stack.count()!=0)
		drawLabel(stack);

	if(Estack.count()!=0)
	{
		for(int i=0; i<Estack.count(); i++)
		{
			Edg e = Estack.at(i);
			Vtx p1 = e.v[0];
			Vtx p2 = e.v[1];

			drawLine(Color4b::Blue, Color4b::Black, p1.V, p2.V);
		}
	}
}

//
//	Only vertices deco mode
//		Draws only vertices and labels, but not edges
//
void edit_topo::editDecoOnlyVertex(MeshModel &m)
{
	if(stack.count()!=0)
		drawPoint(m, 3.0f, Color4b::Red, stack);

	if(stack.count()!=0)
		drawLabel(stack);
}

//
//	Drag and drop mode decorations
//		Before click: standard vertex selection pointer (yellow)
//		After select: draw each new edge with yellow lines
//
void edit_topo::editDecoDragAndDropVertex(MeshModel &m)
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

				drawLine(Color4b::Yellow, Color4b::Yellow, v[0].V, v[1].V);
				drawLine(Color4b::Yellow, Color4b::Yellow, v[1].V, v[2].V);
				drawLine(Color4b::Yellow, Color4b::Yellow, v[2].V, v[0].V);
			}
		}
	}
	else
	{
		Vtx vtx;
		if(getVisibleVertexNearestToMouse(stack, vtx))
			drawPoint(m, 4.0f, Color4b::Yellow, vtx.V);

	}
}

//
//	Face selection decoration mode
//		Draws each face (no vertices, no labels) in blue,
//		and the face nearest to the mouse in yellow (and, so, 
//		face will be selectable)
//
//	This method will also draw the "auto" elaborated vertices
//	as defined by the edit topo algorithm
//
void edit_topo::editDecoFaceSelect(MeshModel &m)
{
	// Draw auto-generated new mesh vertices
	if(out.count()!=0)
	{
		for(int i=0; i<out.count(); i++)
			drawPoint(m, 2.0f, Color4b::Yellow, out.at(i));
	}

	// Draw selected faces in blue, non selected in black, and mouse nearest in yellow
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
				drawLine(Color4b::Blue, Color4b::Black, allv.at(0).V, allv.at(1).V);
				drawLine(Color4b::Blue, Color4b::Black, allv.at(1).V, allv.at(2).V);
				drawLine(Color4b::Blue, Color4b::Black, allv.at(2).V, allv.at(0).V);	
				
				Point3f mid = (allv.at(0).V + allv.at(1).V + allv.at(2).V) / 3;
				if(isVertexVisible(allv.at(0).V)&&isVertexVisible(allv.at(1).V)&&isVertexVisible(allv.at(2).V))
					drawPoint(m, 5.0f, Color4b::Green, mid);
			}
			else
			{
				drawLine(Color4b::DarkRed, Color4b::Black, allv.at(0).V, allv.at(1).V);
				drawLine(Color4b::DarkRed, Color4b::Black, allv.at(1).V, allv.at(2).V);
				drawLine(Color4b::DarkRed, Color4b::Black, allv.at(2).V, allv.at(0).V);	
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

			drawLine(Color4b::Yellow, Color4b::Red, allv.at(0).V, allv.at(1).V);
			drawLine(Color4b::Yellow, Color4b::Red, allv.at(1).V, allv.at(2).V);
			drawLine(Color4b::Yellow, Color4b::Red, allv.at(2).V, allv.at(0).V);	
		}
	}
}

//
//	Vertex selection decoration
//		Draws in yellow the selectable vertex (the selectable vertex is the nearest to the mouse)
//
void edit_topo::editDecoVertexSelect(MeshModel &m)
{
	Point3f p = Point3f(0,0,0);

	glPushMatrix();
	glMultMatrix(m.cm.Tr);
		
	CVertexO * temp_vert=0;
	if (getVertexAtMouse(m, temp_vert))
	{
		cursorPoint = temp_vert->P();
		drawPoint(m, 4.0f, Color4b::Yellow, cursorPoint);
	}
}

//
//	Vertex deletion decoration
//		Draws in yellow the deletable vertex (the deletable vertex is the nearest to the mouse)
//
void edit_topo::editDecoDeleteVertexSelect(MeshModel &m)
{
	Vtx vtx;
	if(getVisibleVertexNearestToMouse(stack, vtx))
		drawPoint(m, 4.0f, Color4b::Green, vtx.V);
}

//
//	Edge deletion decoration
//		Draws in yellow the deletable edge
//
void edit_topo::editDecoDeleteVertexConnect(MeshModel &m)
{
	if(connectStart.V==Point3f(0,0,0) && connectEnd.V==Point3f(0,0,0))
	{
		Vtx vtx;
		if(getVisibleVertexNearestToMouse(stack, vtx))
			drawPoint(m, 4.0f, Color4b::Green, vtx.V);			
	}

	if(connectStart.V!=Point3f(0,0,0) && connectEnd.V==Point3f(0,0,0))
	{
		drawPoint(m, 4.0f, Color4b::LightBlue, connectStart.V);

		Vtx vtx;
		if(getVisibleVertexNearestToMouse(stack, vtx))
		{
			drawPoint(m, 4.0f, Color4b::Green, vtx.V);
			drawLine(Color4b::Blue, Color4b::Green, connectStart.V, vtx.V);
		}
	}
}

//
//	Edge deletion decoration
//
void edit_topo::editDecoDeleteVertexDeconnect(MeshModel &)
{
	Edg minE;
	if(getVisibleEdgeNearestToMouse(Estack, minE))
		drawLine(Color4b::Yellow, Color4b::Green, minE.v[0].V, minE.v[1].V);
}

//
//	Split selection decoration
//		Draws in yellow the splittable edge
//
void edit_topo::editDecoSplit(MeshModel &)
{
	Edg minE;
	if(getVisibleEdgeNearestToMouse(Estack, minE))
		drawLine(Color4b::Yellow, Color4b::Green, minE.v[0].V, minE.v[1].V);
}

//
//	Collapse selection decoration
//		Draws in yellow the collapse able edge
//
void edit_topo::editDecoCollapse(MeshModel &)
{
	Edg minE;
	if(getVisibleEdgeNearestToMouse(Estack, minE))
		drawLine(Color4b::Yellow, Color4b::Green, minE.v[0].V, minE.v[1].V);
}

//
//	Main decoration method
//
void edit_topo::Decorate(MeshModel &m, GLArea *)
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
			editAddVertex(m);
		
		//---
		// Free vertex selection (no pointer)
		//---
		if(edit_topodialogobj->utensil==U_VTX_SEL_FREE)
			editAddVertexFree();
		
		//---
		// Remove selected vertex mode (Yellow pointer)
		//---
		if(edit_topodialogobj->utensil==U_VTX_DEL)
			editDeleteVertex();
		
		//---
		// Edge mode (vertex connection)
		//---
		if(edit_topodialogobj->utensil==U_VTX_CONNECT)
			editConnectVertex();
		
		//---
		// Face Select/Deselect		
		//---
		if(edit_topodialogobj->utensil==U_FCE_SEL)
			editSelectFace();
		
		//---
		// edge deselection mode
		//---
		if(edit_topodialogobj->utensil==U_VTX_DE_CONNECT)
			editDeconnectEdge();
		
		//---
		// Vertex moove mode
		//---
		if(edit_topodialogobj->utensil==U_DND)
			editDragAndDropVertex();
		
		//---
		// edge split mode
		//---
		if(edit_topodialogobj->utensil==U_EDG_SPLIT)
			editEdgeSplit();
		
		//---
		// edge collapse mode
		//---
		if(edit_topodialogobj->utensil==U_EDG_COLLAPSE)
			editEdgeCollapse();		
	}// end if click
	
	
	/*** Decorations ***/
	

	//---
	// Even if there's something selected: show edges	
	//---
	if((edit_topodialogobj->utensil==U_VTX_CONNECT)||(edit_topodialogobj->utensil==U_VTX_DE_CONNECT)
			||(edit_topodialogobj->utensil==U_DND)||(edit_topodialogobj->utensil==U_EDG_SPLIT)
			||(edit_topodialogobj->utensil==U_EDG_COLLAPSE)) 	
		editDecoStandard(m);

	//---
	// First step: show only vertices
	//---
	if((edit_topodialogobj->utensil==U_VTX_SEL_FREE)||(edit_topodialogobj->utensil==U_VTX_DEL)
		||(edit_topodialogobj->utensil==U_VTX_SEL))
		editDecoOnlyVertex(m);

	//---
	// Drag and drop vertices mode
	//---
	if(edit_topodialogobj->utensil==U_DND)
		editDecoDragAndDropVertex(m);

	//---
	// Face selection mode
	//---
	if(edit_topodialogobj->utensil==U_FCE_SEL)
		editDecoFaceSelect(m);

	//---
	// Yellow pointer (vertex selection)
	//---
	if(edit_topodialogobj->utensil==U_VTX_SEL)
		editDecoVertexSelect(m);

	//---
	// Vertex de-selection (Yellow pointeR)
	//---
	if((edit_topodialogobj->utensil==U_VTX_DEL)&&(stack.count()!=0))	
		editDecoDeleteVertexSelect(m);
	
	//---
	// edge mode (vtx connection)
	//---
	if(edit_topodialogobj->utensil==U_VTX_CONNECT)
		editDecoDeleteVertexConnect(m);

	//---
	// edge deselection mode
	//---
	if(edit_topodialogobj->utensil==U_VTX_DE_CONNECT)
		editDecoDeleteVertexDeconnect(m);

	//---
	// edge split mode
	//---
	if(edit_topodialogobj->utensil==U_EDG_SPLIT)
		editDecoSplit(m);

	//---
	// edge collapse mode
	//---
	if(edit_topodialogobj->utensil==U_EDG_COLLAPSE)
		editDecoCollapse(m);
}

//
//	Plugin init
//
bool edit_topo::StartEdit(MeshModel &m, GLArea *gla)
{	
	parentGla = gla;
	gla->setCursor(QCursor(QPixmap(":/images/cursor_paint.png"),1,1));	

	// Init uniform grid
	float dist = m.cm.bbox.Diag();

	// Init data masks
	m.updateDataMask(MeshModel::MM_FACEMARK);
	tri::UpdateNormals<CMeshO>::PerFaceNormalized(m.cm);
	tri::UpdateFlags<CMeshO>::FaceProjection(m.cm);

	// Init retopology model builder object
	rm.init(&m, dist);

	// Init miminum visible distance param (used only for labels rendering)
	_md = 0.03;

	// Init ui
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

	gla->update();
	gla->setMouseTracking(true);

	// Connect slots
	connect(edit_topodialogobj, SIGNAL( mesh_create() ),
          this, SLOT( on_mesh_create() ) );

	connect(edit_topodialogobj, SIGNAL( update_request() ),
          this, SLOT( on_update_request() ) );
	return true;
}

//
//	End edit
//
void edit_topo::EndEdit(MeshModel &, GLArea *)
{
	stack.clear();
	Estack.clear();
	Fstack.clear();

	reDraw = false;
	click = false;
	first_model_generated=false;

	nameVtxCount = 0;

	drag_click=false;
	drag_stack.clear();	

	lastPoint.V = Point3f(0,0,0);
	lastPoint.vName = "--";
	
	connectStart.V = Point3f(0,0,0);
	connectStart.vName = "--";
	connectEnd.V = Point3f(0,0,0);
	connectEnd.vName = "--";

	if (edit_topodialogobj != 0) 
	{ 
		delete edit_topodialogobj; 
		delete dock; 
		edit_topodialogobj = 0;
		dock = 0;
	 }
}


/************************************************************************************/
//
// --- Slot implementation methods ---
//		Those two methods are invoked by the gui
//

//
//	"Create new mesh" click
//		This method calls the retopology algoritm, and creates
//		the new mesh using the user defined new topology
//
void edit_topo::on_mesh_create()
{
	out.clear();

	if(first_model_generated)
		parentGla->meshDoc.meshList.pop_back();
	

	MeshModel *mm= new MeshModel();
	parentGla->meshDoc.meshList.push_back(mm);
	first_model_generated = true;


	MeshModel *m = parentGla->meshDoc.meshList.back();	// destination = new mesh
	MeshModel *currentMesh  = parentGla->meshDoc.mm();	// source = current	mesh

	// if debug value is true, the algorithm will respond with all the
	// auto generated vertices.
	// The isDEBUG value is forced by the "Draw auto-filled vertices" gui checkbox
	if(edit_topodialogobj->isDEBUG())
	{
		rm.Lout.clear();
    	int iter = edit_topodialogobj->getIterations();
		float dist = edit_topodialogobj->dist()/100;

		// Retopology algorithm call
		rm.createRefinedMesh(*m, /* *currentMesh,*/ dist, iter, Fstack, stack, edit_topodialogobj, true);
		out = rm.Lout;
	}
	else
	{
		int iter = edit_topodialogobj->getIterations();
		float dist = edit_topodialogobj->dist()/100;

		// Retopology algorithm call
		rm.createRefinedMesh(*m, /* *currentMesh, */ dist, iter, Fstack, stack, edit_topodialogobj, false);
	}

	m->cm.Tr = currentMesh->cm.Tr;
	parentGla->update();
}


//
//	General update requested by some gui functions
//
void edit_topo::on_update_request()
{
	parentGla->update();
}





/************************************************************************************/
//
// --- Plugin events methods ---
//
void edit_topo::mousePressEvent(QMouseEvent * event, MeshModel &m, GLArea * gla) 
{
	mousePos=event->pos();
	click=false;
	gla->update();
}

void edit_topo::mouseMoveEvent(QMouseEvent * event, MeshModel &m, GLArea * gla)
{
	mousePos=event->pos();
	mouseRealY = gla->curSiz.height() - mousePos.y();
	reDraw=true;
	gla->update();
}

void edit_topo::mouseReleaseEvent(QMouseEvent * event, MeshModel &, GLArea * gla)
{
	if(event->button() == Qt::LeftButton)
	{
		click=true;
		reDraw=true;
	}
	else if(event->button() == Qt::RightButton)
	{
		connectStart.V=Point3f(0,0,0);

		drag_stack.clear();
		drag_click = false;	
	}
	gla->update();
	mousePos=event->pos();
}


/************************************************************************************/
//
// --- New topology mesh methods ---
//			Those methods are used by the edit plugin 
//			to elaborate topology related operations
//


//
//	Get nearest 2d point of the given array
//	Returns: array index
int edit_topo::getNearest(QPointF center, QPointF *points,int num) {
	int nearestInd=0;
	float dist=fabsf(center.x()-points[0].x())*fabsf(center.x()-points[0].x())+fabsf(center.y()-points[0].y())*fabsf(center.y()-points[0].y());
	for (int lauf=1; lauf<num; lauf++) 
	{
		float temp=fabsf(center.x()-points[lauf].x())*fabsf(center.x()-points[lauf].x())+
				fabsf(center.y()-points[lauf].y())*fabsf(center.y()-points[lauf].y());
		if (temp<dist) 
		{
				nearestInd=lauf;
				dist=temp;
		}
	}
	return nearestInd;
}

//
//	Get nearest 3d face over the meshmodel
//	Returns: true if found, and facepointer
bool edit_topo::getFaceAtMouse(MeshModel &m, CMeshO::FacePointer& val) 
{
	QPoint mid=QPoint(mousePos.x(), mouseRealY);
	return (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, val,2,2));
}

//
//	Get existing nearest 3d vertex on the given mesh model
//	Returns: true if found, and vertexpointer value
bool edit_topo::getVertexAtMouse(MeshModel &m,CMeshO::VertexPointer& value) 
{
	CFaceO * temp=0;

	QPoint mid=QPoint(mousePos.x(), mouseRealY);
	double tx,ty,tz;
	if (getFaceAtMouse(m,temp)) 
	{
		QPointF point[3];
		for (int lauf=0; lauf<3; lauf++) 
		{
			gluProject(temp->V(lauf)->P()[0],temp->V(lauf)->P()[1],temp->V(lauf)->P()[2],mvmatrix,projmatrix,viewport,&tx,&ty,&tz);
			point[lauf]=QPointF(tx,ty);
		}
		value=temp->V(getNearest(mid,point,3));
		return true;
	}
	return false;
}


//
//	Check if the given vertex is visible on the current user view
//			This method is used for rendering vertices labels, and
//			rendering edges/faces
//	Returns: true if visible
bool edit_topo::isVertexVisible(Point3f v)
{
	float   pix;
	double tx,ty,tz;

	gluProject(v.X(),v.Y(),v.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
	glReadPixels(tx,ty,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&pix);

	float ff = fabs(tz - pix);

	return ((ff < 0.003));
}

//
//	Get visible vertex nearest to mouse position (from a given vertices list)
//	Returns: true if visible
bool edit_topo::getVisibleVertexNearestToMouse(QList<Vtx> list, Vtx &out)
{
	bool found = false;
	double minDist = 100000;
	int minIdx = -1;
	Point3f t;

	QList<Vtx> visib;

	for(int i=0; i<list.count(); i++)
		if(isVertexVisible(list.at(i).V))
			visib.push_back(list.at(i));

	QPoint mPos = QPoint(mousePos.x(), mouseRealY);
	for(int i=0; i<visib.count(); i++)
	{
		Point3f p = visib.at(i).V;
		double tx,ty,tz;
		gluProject(p.X(),p.Y(),p.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);

		QPoint qp = QPoint(tx, ty);

		// faster distance
		//double dx = fabs((double)(qp.x() - mPos.x()));
		//double dy = fabs((double)(qp.y() - mPos.y()));
		//double dist = (dy > dx) ? 0.41*dx+0.941246*dy : 0.41*dy+0.941246*dx;

		double dist = sqrt((double)(math::Sqr(qp.x() - mPos.x()) + math::Sqr(qp.y() - mPos.y())));
			
		if(dist<minDist)
		{
			minDist = dist;
			minIdx = i;
			found = true;						
		}
	}

	if(found)
	{
		for(int j=0; j<list.count(); j++)
			if(list.at(j).vName==visib.at(minIdx).vName)
			{
				out = list.at(j);
				return true;
			}
	}
	return false;
}

//
//	Get visible edge nearest to mouse (from a given edges list)
//	Returns: true if found
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
}

//
//	Checks if the given point is in the given triangle
//
bool edit_topo::pointInTriangle(const QPointF &p, const QPointF &a, const QPointF &b, const QPointF &c) 
{
	float fab=(p.y()-a.y())*(b.x()-a.x()) - (p.x()-a.x())*(b.y()-a.y());
	float fbc=(p.y()-c.y())*(a.x()-c.x()) - (p.x()-c.x())*(a.y()-c.y());
	float fca=(p.y()-b.y())*(c.x()-b.x()) - (p.x()-b.x())*(c.y()-b.y());

	return (fab*fbc>0 && fbc*fca>0);
}

//
//	2d distance p-seg
//
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

//
//	2d distance p-p
//
float edit_topo::distancePointPoint(QPointF P1, QPointF P2)
{ 	
	return sqrt(pow((P1.x()-P2.x()),2)+pow((P1.y()-P2.y()),2));
}

/************************************************************************************/
//
// --- Plugin rendering methods ---
//		Those methods are used by the plugin to draw labels
//		vertices, edges, faces, ... in opengl

//
//	Draws user selected vertices labels
//
void edit_topo::drawLabel(QList<Vtx> list)
{
	
	QVector<Vtx> v = list.toVector();
	int pCount = list.count();

	for(int i=0; i<pCount; i++)
		drawLabel(list.at(i));
}

//
//	Draws user selected vertex label
//
void edit_topo::drawLabel(Vtx v)
{
	if(isVertexVisible(v.V)&&(edit_topodialogobj->drawLabels()))
	{
		double tx,ty,tz;
		gluProject(v.V.X(),v.V.Y(),v.V.Z(), mvmatrix,projmatrix,viewport, &tx,&ty,&tz);
		int x,y;
		x = tx+5;
		y=(parentGla->curSiz.height() - 5 - ty);

		// new style
		QString text = v.vName;
		QFont font;
		font.setFamily("Helvetica");
		font.setPixelSize(10);
		QFontMetrics fm(font);
		QRect brec=fm.boundingRect(text);
		glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT );
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0,parentGla->width(),parentGla->height(),0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glColor4f(0,0,0,0.6f);
		glBegin(GL_QUADS);
		glVertex2f(x+brec.left(),y+brec.bottom());
		glVertex2f(x+brec.right(),y+brec.bottom());
		glVertex2f(x+brec.right(),y+brec.top());
		glVertex2f(x+brec.left(),y+brec.top());
		glEnd();
		int offset=2;
		glColor4f(0,0,0,0.3f);
		glBegin(GL_QUADS);
			glVertex2f(x+brec.left()-offset,y+brec.bottom()+offset);
			glVertex2f(x+brec.right()+offset,y+brec.bottom()+offset);
			glVertex2f(x+brec.right()+offset,y+brec.top()-offset);
			glVertex2f(x+brec.left()-offset,y+brec.top()-offset);
		glEnd();
		glColor3f(1,1,1);
		parentGla->renderText(x,y, text,QFont());
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPopAttrib();
	}
}

//
//	Draws a point
//
void edit_topo::drawPoint(MeshModel &m, float pSize, Color4b colorFront, Point3f p)
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
/*			
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);
	glColor(colorBack);
		
	glPointSize(0.3);

	glBegin(GL_POINT);
		glVertex(cursorPoint);
		glVertex(cursorPoint);
	glEnd();
*/		
	glPopAttrib();
	glPopMatrix();
}

//
//	Draws all vertices
//
void edit_topo::drawPoint(MeshModel &m, float pSize, Color4b colorFront, QList<Vtx> list)
{
	glPushMatrix();
	glMultMatrix(m.cm.Tr);
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
	glLineWidth(2.0f);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST); 
	glDepthMask(GL_FALSE);
		
	glDisable(GL_LIGHTING);
	glColor(colorFront);
		
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
	glColor(Color4b::White);
		
	glPointSize(1.3f);

	glBegin(GL_POINTS);
		for(int i=0; i<pCount; i++)
			glVertex(v[i].V);
	glEnd();
		
	glPopAttrib();
	glPopMatrix();
}

//
//	Creates recursively the dotted line points
//
QVector<Point3f> vectSub(int part, Point3f p1, Point3f p2)
{
	if(part==2)
	{
		QVector<Point3f> toRet(3);
		toRet[0]=p1;
		toRet[1]=(p1+p2)/2;
		toRet[2]=p2;
		return toRet;
	}
	else
	{
		QVector<Point3f> L;
		QVector<Point3f> R;

		int np=(int)(part/2);

		L = vectSub(np, p1, (p1+p2)/2);
		R = vectSub(np, (p1+p2)/2, p2);
		
		QVector<Point3f> toRet;
		for(int i=0; i<L.size(); i++)
			if(!toRet.contains(L.at(i)))
				toRet.push_back(L.at(i));
		for(int i=0; i<R.size(); i++)
			if(!toRet.contains(R.at(i)))
				toRet.push_back(R.at(i));

		return toRet;
	}
}

//
//	Draws a line (used for edges and faces)
//
void edit_topo::drawLine(Color4b colorFront, Color4b colorBack, Point3f p1, Point3f p2)
{		
	// Drawing of the current line
	if(isVertexVisible(p1)&&isVertexVisible(p2))
	{
		glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDepthMask(false);
		glLineWidth(2.5f);
		glPointSize(1.4f);
  
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

		// Fill the intermed. points to draw a dotted line
		QVector<Point3f> trattP;
		int part = 32;
		float dist = (p1-p2).Norm();
		if(dist>10) part*=2;
		if(dist>50) part*=2;
		if(dist>100) part*=2;
		if(dist>400) part*=2;

		Point3f pp1, pp2;
		trattP = vectSub(part, p1, p2);

		for(int i=0; i<(trattP.size()-1); i+=2)
		{
			pp1 = trattP[i];
			pp2 = trattP[i+1];

			glDisable(GL_DEPTH_TEST);

			glLineWidth(0.5f);
			glPointSize(0.3f);
			glBegin(GL_LINES);
				glVertex(pp1);
				glVertex(pp2);
			glEnd();
			glBegin(GL_POINTS);
				glVertex(pp1);
				glVertex(pp2);
			glEnd(); 
		}
		glPopAttrib();
	}
	else if(edit_topodialogobj->drawEdges())
	{
		glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDepthMask(false);
		glLineWidth(1.5f);
		glPointSize(0.4f);

		// Fill the intermed. points to draw a dotted line
		QVector<Point3f> trattP;
		int part = 8;
		float dist = (p1-p2).Norm();
		if(dist>10) part*=2;
		if(dist>50) part*=2;
		if(dist>100) part*=2;
		if(dist>400) part*=2;
	
		Point3f pp1, pp2;	
		trattP = vectSub(part, p1, p2);
	
		glColor(colorBack);
		glDisable(GL_DEPTH_TEST);

		for(int i=0; i<(trattP.size()-1); i+=2)
		{
			pp1 = trattP[i];
			pp2 = trattP[i+1];

			glBegin(GL_LINES);
				glVertex(pp1);
				glVertex(pp2);
			glEnd();
			glBegin(GL_POINTS);
				glVertex(pp1);
				glVertex(pp2);
			glEnd(); 
		}
		glPopAttrib();
	}
}

//
//	Draws a triangle
//
void edit_topo::drawFace(CMeshO::FacePointer fp)
{
    glPointSize(3.0f);
	
	glBegin(GL_POINTS); //GL_LINE_LOOP);
		glVertex(fp->P(0)); 
		glVertex(fp->P(1));
		glVertex(fp->P(2));
	glEnd();
}
