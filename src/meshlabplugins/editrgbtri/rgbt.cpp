/****************************************************************************
 * Rgb Triangulations Plugin                                                 *
 *                                                                           *
 * Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
 * Copyright(C) 2007                                                         *
 * DISI - Department of Computer Science                                     *
 * University of Genova                                                      *
 *                                                                           *
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
 ****************************************************************************/

#include <QtGui>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "rgbt.h"
#include <stdio.h>
#include <wrap/gl/pick.h>
#include <limits>
#include <vcg/space/color4.h>

#include "rgbPrimitives.h"
#include "controlPoint.h"
#include "subDialog.h"

#include<vcg/complex/trimesh/update/topology.h>
#include<vcg/complex/trimesh/update/bounding.h>

namespace rgbt
{

using namespace vcg;

RgbTPlugin::RgbTPlugin()
{
	m = 0;
	rgbInfo = 0;
	widgetRgbT=0;
	to = 0;
	isDragging = false;
	//qDebug() << "RgbTPlugin"<< endl;
	ie = 0;
	rgbie = 0;
}

/** the destructor is never called */
RgbTPlugin::~RgbTPlugin()
{
	//qDebug() << "~RgbTPlugin"<< endl;
}

const QString RgbTPlugin::Info()
{
	return tr("Selective Mesh Refinement with Rgb Triangulations");
}

bool RgbTPlugin::StartEdit(MeshModel &m, GLArea * parent)
{
	// select the type of subdivison surface
	SubDialog sd;
	int r = sd.exec();
	if (r == QDialog::Accepted)
		RgbPrimitives::stype = RgbPrimitives::LOOP;
	else
		RgbPrimitives::stype = RgbPrimitives::MODBUTFLY;
	// -----
	
	
	this->m = &m;
	current_gla = parent;

	if (widgetRgbT==0)
	{
		widgetRgbT = new WidgetRgbT(parent->window(),this);
		paint_dock = new QDockWidget(parent->window());
		paint_dock->setAllowedAreas(Qt::NoDockWidgetArea);
		paint_dock->setWidget(widgetRgbT);
		QPoint p=parent->window()->mapToGlobal(QPoint(0, 0));
		paint_dock->setGeometry(-5+p.x()+parent->window()->width()-widgetRgbT->width(), p.y(), widgetRgbT->width(),
		widgetRgbT->height());
		paint_dock->setFloating(true);
	}
	paint_dock->setVisible(true);
	paint_dock->layout()->update();

	parent->setCursor(QCursor(QPixmap(":/images/sel_rect.png"),1,1));

	m.updateDataMask(MeshModel::MM_FACEFACETOPO);
	m.updateDataMask(MeshModel::MM_VERTFACETOPO); // It update also the FV relation
			m.updateDataMask(MeshModel::MM_FACECOLOR);

#ifdef RGBCOLOR 
			parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerFace;
#endif
			parent->getCurrentRenderMode().lighting=true;
			parent->getCurrentRenderMode().drawMode = vcg::GLW::DMFlatWire;
			//parent->mm()->ioMask|=MeshModel::IOM_FACECOLOR;

			if (rgbInfo) delete rgbInfo;
			rgbInfo = new RgbInfo(m.cm.vert.size(),m.cm.face.size());

			if (to) delete to;
			to = new TopologicalOpC(m.cm,&(rgbInfo->vert),&(rgbInfo->face));

			if (ie) delete ie;
			ie = new InteractiveEdit();

			if (rgbie) delete rgbie;
			rgbie = new RgbInteractiveEdit(m.cm,*rgbInfo,*to);

			for (unsigned int i = 0; i < m.cm.face.size(); ++i)
			{
				if (!m.cm.face[i].IsD())
				{
					RgbTriangleC t = RgbTriangleC(m.cm,*rgbInfo,i);
					t.setFaceColor(FaceInfo::FACE_GREEN);
					t.setFaceLevel(0);
				}
			}
			int x = 0;
			for (unsigned int i = 0; i < m.cm.vert.size(); ++i)
			{

				if (!m.cm.vert[i].IsD())
				{
					RgbVertexC v = RgbVertexC(m.cm,*rgbInfo,i);
					v.setLevel(0);

					if (!RgbPrimitives::isVertexInternal(v))
					{
						v.setIsBorder(true);
						x++;
					}

				}

			}
			
			ModButterfly::init(m.cm,*rgbInfo);
			
			ControlPoint::init(m.cm,*rgbInfo);

			parent->update();

			updateSelectedFaces(m);
			return false;
		}

		// this is called only when we change editor or we want to close it.
		void RgbTPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/)
		{
			//qDebug() <<"RgbTPlugin::ENDEDIT"<<endl;

			if (widgetRgbT!=0)
			{
				delete widgetRgbT;
				delete paint_dock;
				widgetRgbT=0;
				paint_dock=0;
			}
		}

		void RgbTPlugin::mousePressEvent(QMouseEvent *event, MeshModel &m, GLArea * gla)
		{
			cur=event->pos();
			isDragging = true;
			selMode = SMClear;
			if(event->modifiers()==Qt::ControlModifier) selMode=SMAdd;
			if(event->modifiers()==Qt::ShiftModifier) selMode=SMSub;

			if (widgetRgbT->tool == TOOL_BRUSH || widgetRgbT->tool == TOOL_ERASER)
			{
				// Interactive Editing
				ie->has_track=gla->isTrackBallVisible();
				gla->showTrackBall(false);

				ie->first = true;
				ie->isDragging = true;
				ie->pressed = 1;

				ie->visited_vertexes.clear();
				ie->start=event->pos();
				ie->cur=ie->start;
				ie->prev=ie->start;
				ie->inverse_y=gla->curSiz.height()-cur.y();
				ie->curr_mouse=event->button();
				ie->current_gla=gla;

				ie->pen.painttype=PEN;
				ie->pen.paintutensil=SELECT;
				ie->pen.backface= 0;
				ie->pen.invisible= 0;
				ie->pen.radius= widgetRgbT->sRadius->value()*0.5;
				ie->curSel.clear();
			}
		}

		void RgbTPlugin::mouseMoveEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea *gla)
		{
			isDragging = true;
			cur=event->pos();

			if (widgetRgbT->tool == TOOL_BRUSH || (widgetRgbT->tool == TOOL_ERASER))
			{
				if (!ie->isDragging)
				{
					ie->prev=ie->cur; /** to prevent losses when two mouseEvents occur befor one decorate */
				}
				ie->cur=event->pos();
				ie->isDragging = true;
				//gla->update();
			}

			gla->update();
		}

		void RgbTPlugin::mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea *gla)
		{
			if (widgetRgbT->tool == TOOL_BRUSH || (widgetRgbT->tool == TOOL_ERASER))
			{
				gla->showTrackBall(ie->has_track);
				ie->visited_vertexes.clear();
				gla->update();
				ie->prev=ie->cur;
				ie->cur=event->pos();
				ie->pressed=2;
				ie->isDragging=false;
			}
		}

		void RgbTPlugin::DrawXORRect(GLArea * gla, bool doubleDraw)
		{
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0,gla->curSiz.width(),gla->curSiz.height(),0,-1,1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_COLOR_LOGIC_OP);
			glLogicOp(GL_XOR);
			glColor3f(1,1,1);
			if(doubleDraw)
			{
				glBegin(GL_LINE_LOOP);
				glVertex2f(startp.x(),startp.y());
				glVertex2f(prevp.x(),startp.y());
				glVertex2f(prevp.x(),prevp.y());
				glVertex2f(startp.x(),prevp.y());
				glEnd();
			}
			glBegin(GL_LINE_LOOP);
			glVertex2f(startp.x(),startp.y());
			glVertex2f(cur.x(),startp.y());
			glVertex2f(cur.x(),cur.y());
			glVertex2f(startp.x(),cur.y());
			glEnd();
			glDisable(GL_LOGIC_OP);

			// Closing 2D
			glPopAttrib();
			glPopMatrix(); // restore modelview
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);

		}

		/** only in decorare it is possible to obtain the correct zbuffer values and the other opengl stuff */
		void RgbTPlugin::Decorate(MeshModel &m, GLArea * gla)
		{
			if (!widgetRgbT) 
				return; // it is possible that the widget is not altready build when decorate il called
			QPoint mid=QPoint(cur.x(),gla->curSiz.height()-cur.y());
			CMeshO::FaceIterator it;

			switch (widgetRgbT->tool)
			{
				case TOOL_SELECTIONSINGLE:
				if (!isDragging) return;
				CFaceO * fp;
				if (getFaceAtMouse(m,mid,fp))
				{
					switch (selMode)
					{
						case SMAdd:
						if (!fp->IsS())
						{
							selectedFaces.push_back(fp);
							fp->SetS();
						}

						break;
						case SMSub:
						if (fp->IsS())
						{
							selectedFaces.remove(fp);
							fp->ClearS();
						}

						break;
						case SMClear:
						for (it = m.cm.face.begin(); it != m.cm.face.end(); ++it)
						{
							it->ClearS();
						}
						selectedFaces.clear();
						fp->SetS();
						selectedFaces.push_back(fp);
						break;
						default:
						break;
					}
				}

				break;

				case TOOL_BRUSH:
				case TOOL_ERASER:

				ie->updateMatrixes();
				QPoint mid=QPoint(cur.x(),gla->curSiz.height()- cur.y());
				if (ie->first)
				{
					ie->first=false;
					if (ie->pixels!=0)
					{
						free(ie->pixels);
					}
					ie->pixels=(GLfloat *)malloc(sizeof(GLfloat)*gla->curSiz.width()*gla->curSiz.height());
					glReadPixels(0,0,gla->curSiz.width(),gla->curSiz.height(),GL_DEPTH_COMPONENT,GL_FLOAT,ie->pixels);
				}
				if(ie->isDragging)
				{
					ie->isDragging=false;

					ie->DrawXORCircle(gla,false);

					vector<Vert_Data>::iterator vpo;
					vector<Vert_Data> newSel;

					vector<CMeshO::FacePointer> faceSel;

					if (ie->first)
						ie->curSel.clear();

					ie->pen.backface = false;
					ie->pen.invisible = false;

					ie->getInternFaces(m,&ie->curSel,&newSel,&faceSel,gla,ie->pen,ie->cur,ie->prev,ie->pixels,ie->mvmatrix,ie->projmatrix,ie->viewport);

					// Faces
					if (widgetRgbT->tool == TOOL_BRUSH)
					{
						vector<CMeshO::FacePointer>::iterator fpo;
						typedef std::pair<int,int> intpair;
						list<intpair> lp;
						list<intpair>::iterator lpit;

						//std::cout << "FACES << ";
						for(fpo=faceSel.begin();fpo!=faceSel.end();++fpo)
						{
							for (int i = 0; i<3; i++)
							{
								int v1 = (*fpo)->V(i%3)-&(m.cm.vert[0]);
								int v2 = (*fpo)->V((i+1)%3)-&(m.cm.vert[0]);
								lp.push_back(intpair(v1,v2));
							}
							//std::cout << (*fpo)->Index() << " - ";
						}
						//std::cout << ">> " << std::endl;
						for (lpit = lp.begin(); lpit != lp.end(); ++lpit)
						{
							int wlevel = widgetRgbT->sBrushLevel->value();
							double wlength = widgetRgbT->sBrushLength->value();
							int* plevel = widgetRgbT->cLevelEnabled->isChecked() ? &wlevel : 0;
							double* plength = widgetRgbT->cLengthEnabled->isChecked() ? &wlength : 0;
							rgbie->processEdge((*lpit).first,(*lpit).second,plevel,plength);
						}

					}
					
					if (widgetRgbT->tool == TOOL_ERASER)
					{
						// Vert
						list<int> li;
						list<int>::iterator liit;
						
						//std::cout << "VERT << ";
						for(vpo=newSel.begin();vpo!=newSel.end();++vpo)
						{
							int i = (vpo->v) - &(m.cm.vert[0]);
							li.push_back(i);
							//std::cout << i << "(" << vpo->distance << ") - ";
						}
						for (liit = li.begin(); liit != li.end(); ++liit)
						{
							int wlevel = widgetRgbT->sBrushLevel->value();
							double wlength = widgetRgbT->sBrushLength->value();
							int* plevel = widgetRgbT->cLevelEnabled->isChecked() ? &wlevel : 0;
							double* plength = widgetRgbT->cLengthEnabled->isChecked() ? &wlength : 0;
							rgbie->processVertex(*liit,plevel,plength);
						}
						
						//std::cout << ">> " << std::endl;
					}
					
					ie->pressed=0;

				}
				ie->isDragging = false;
			}
		}

		bool RgbTPlugin::getFaceAtMouse(MeshModel &m,QPoint &mid ,CMeshO::FacePointer& val)
		{
			return (GLPickTri<CMeshO>::PickNearestFace(mid.x(), mid.y(), m.cm, val,2,2));
		}

		bool RgbTPlugin::getFacesAtMouse(MeshModel &m,QPoint &mid ,vector<CMeshO::FacePointer> & val)
		{
			val.clear();
			GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, val,2,2);
			return (val.size()>0);
		}

		void RgbTPlugin::updateSelectedFaces(MeshModel &m)
		{
			selectedFaces.clear();
			CMeshO::FaceIterator it;
			for (it = m.cm.face.begin(); it != m.cm.face.end(); ++it)
			{
				if (it->IsS())
				selectedFaces.push_back(&*it);
			}

		}

		// Public Slots

		void RgbTPlugin::edgeSplit()
		{
			if (selectedFaces.size() == 2)
			{
				std::list<CMeshO::FacePointer>::iterator it = selectedFaces.begin();
				CMeshO::FacePointer f0 = *it;
				(*it)->ClearS();
				++it;
				(*it)->ClearS();
				CMeshO::FacePointer f1 = *it;

				typedef EdgeFIType EdgeFIType;
				EdgeFIType e;
				if (commonEdge(f0,f1,&e))
				{
					RgbTriangleC t = RgbTriangleC(m->cm,*rgbInfo,e.fp->Index());
					RgbPrimitives::recursiveEdgeSplit(t,e.i,*to);
				}
			}
			else if (selectedFaces.size() == 1)
			{
				std::list<CMeshO::FacePointer>::iterator it = selectedFaces.begin();
				CMeshO::FacePointer f = *it;
				(*it)->ClearS();

				RgbTriangleC t = RgbTriangleC(&m->cm,rgbInfo,f->Index());

				for(int i = 0; i < 3; i++)
				{
					PointType p;
					PointType p1, p2;
					p1 = (t.V(i).vert().P());
					p2 = (t.V((i+1)%3).vert().P());

					for (int i2 = 0; i2 < 3; ++i2)
					{
						p[i2] = (p1[i2] + p2[i2]) / 2;
					}

					if (t.getEdgeIsBorder(i))
					{
						RgbPrimitives::recursiveEdgeSplit(t,i,*to);
					}

				}

			}

			selectedFaces.clear();
			current_gla->update();
		}

		void RgbTPlugin::vertexRemoval()
		{
			if (selectedFaces.size() > 2)
			{
				std::list<FacePointer>::iterator it = selectedFaces.begin();
				std::list<FacePointer>::iterator end = selectedFaces.end();
				vector<FacePointer> sf;
				while(it != end)
				{
					sf.push_back(*it);
					++it;
				}

				EdgeFIType e;
				if (commonVertex(sf,&e))
				{
					RgbTriangleC t = RgbTriangleC(m->cm,*rgbInfo,e.fp->Index());
					RgbPrimitives::vertexRemoval(t,e.i,*to);
				}
			}
			else if (selectedFaces.size() == 1)
			{
				std::list<CMeshO::FacePointer>::iterator it = selectedFaces.begin();
				CMeshO::FacePointer f = *it;
				(*it)->ClearS();

				RgbTriangleC t = RgbTriangleC(&m->cm,rgbInfo,f->Index());

				for(int i = 0; i < 3; i++)
				{
					if (t.getVertexIsBorder(i))
					{
						if (RgbPrimitives::vertexRemoval_Possible(t,i))
						{
							RgbPrimitives::vertexRemoval(t,i,*to);
							break;
						}

					}

				}

			}
			selectedFaces.clear();
			current_gla->update();
		}

		bool RgbTPlugin::pickEdgeLenght(double& d)
		{
			if (selectedFaces.size() == 2)
			{
				std::list<CMeshO::FacePointer>::iterator it = selectedFaces.begin();
				CMeshO::FacePointer f0 = *it;
				++it;
				CMeshO::FacePointer f1 = *it;

				typedef EdgeFIType EdgeFIType;
				EdgeFIType e;
				if (commonEdge(f0,f1,&e))
				{
					Point3f v1 = e.fp->V(e.i)->P();
					Point3f v2 = e.fp->V((e.i+1)%3)->P();

					d = (v2 - v1).Norm();

					return true;
				}

				return false;
			}
			else
			return false;
		}

		bool RgbTPlugin::commonEdge(CMeshO::FacePointer fp1, CMeshO::FacePointer fp2, EdgeFIType* edge)
		{
		    assert(fp1);
		    assert(fp2);
		    
		    for (int i = 0; i < 3; ++i) {
		        if (fp1->FFp(i) == fp2)
		        {
		            if (edge)
		                *edge = EdgeFIType(fp1,i);
		            return true;
		        }
		    }
		    return false;
		}

		bool RgbTPlugin::commonVertex(vector<FacePointer> fc, EdgeFIType* vert)
		{
		    if (fc.size() < 2)
		        return false;
		    
		    FacePointer f = fc[0];
		    
		    for (int i = 0; i < 3; ++i) 
		    {
		        VertexPointer v = f->V(i);
		        vector<FacePointer>::iterator it = fc.begin();
		        vector<FacePointer>::iterator end = fc.end();
		        ++it; // skip the first
		        bool isCommon = true;
		        for (; it != end; ++it) 
		        {
		            bool isInTriangle = false;
		            for (int j = 0; j < 3; ++j) 
		            {
		                
		                if ((*it)->V(j) == v)
		                    isInTriangle = true;
		            }
		            if (!isInTriangle)
		                isCommon = false;
		            
		        }
		        
		        if (isCommon)
		        {
		            if(vert)
		            {
		                vert->fp = f;
		                vert->i = i;
		            }
		            return true;
		        }
		    }
		    
		    return false;
		}
		
	}

