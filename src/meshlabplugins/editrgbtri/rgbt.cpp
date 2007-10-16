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
    sr = 0;
    to = 0;
    srThread = 0;
    actionList << new QAction(QIcon(":/images/rgbt.png"),"Rgb Triangulations Plugin", this);
    QAction *editAction;
    foreach(editAction, actionList)
    editAction->setCheckable(true);
    isDragging = false;
    //qDebug() << "RgbTPlugin"<< endl;
}

/** the destructor is never called */
RgbTPlugin::~RgbTPlugin()
{
    //qDebug() << "~RgbTPlugin"<< endl;
}

QList<QAction *> RgbTPlugin::actions() const
{
    return actionList;
}

const QString RgbTPlugin::Info(QAction */*action*/)
{
    return tr("Selective Mesh Refinement with Rgb Triangulations");
}

const PluginInfo &RgbTPlugin::Info()
{
    static PluginInfo ai;
    ai.Date=tr(__DATE__);
    ai.Version = tr("0.3");
    ai.Author = ("Daniele Panozzo");
    return ai;
}

void RgbTPlugin::StartEdit(QAction * /*mode*/, MeshModel &m, GLArea * parent)
{
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

	m.updateDataMask(MeshModel::MM_FACETOPO);
	m.updateDataMask(MeshModel::MM_VERTFACETOPO); // It update also the FV relation
	m.updateDataMask(MeshModel::MM_FACECOLOR);
	

	parent->getCurrentRenderMode().colorMode=vcg::GLW::CMPerFace;
	parent->getCurrentRenderMode().lighting=true;
	parent->getCurrentRenderMode().drawMode = vcg::GLW::DMFlatWire;
	parent->mm()->ioMask|=MeshModel::IOM_FACECOLOR;

	if (rgbInfo) delete rgbInfo;
	rgbInfo = new RgbInfo(m.cm.vert.size(),m.cm.face.size());

	if (to) delete to;
	to = new TopologicalOpC(m.cm,&(rgbInfo->vert),&(rgbInfo->face));

	if (sr) delete sr;
	sr = new SelectiveRefinement(m.cm,*rgbInfo,*to);

	for (unsigned int i = 0; i < m.cm.face.size(); ++i)
	{
		if (!m.cm.face[i].IsD())
		{
			RgbTriangleC t =  RgbTriangleC(m.cm,*rgbInfo,i);
			t.setFaceColor(FaceInfo::FACE_GREEN);
			t.setFaceLevel(0);
		}
	}
	int x = 0;
    for (unsigned int i = 0; i < m.cm.vert.size(); ++i)
    {
    	
        if (!m.cm.vert[i].IsD())
        {
            RgbVertexC v =  RgbVertexC(m.cm,*rgbInfo,i);
            v.setLevel(0);
            
            if (!RgbPrimitives::isVertexInternal(v))
            {
                v.setIsBorder(true);
                x++;
            }
            
        }
        
    }
    std::cerr << "vertex on border: " << x << std::endl;
	ControlPoint::init(m.cm,*rgbInfo);
	
	parent->update();
	
	updateSelectedFaces(m);
}

// this is called only when we change editor or we want to close it.
void RgbTPlugin::EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/)
{
	qDebug() <<"RgbTPlugin::ENDEDIT"<<endl;

	if (widgetRgbT!=0)
	{
		delete widgetRgbT;
		delete paint_dock;
		widgetRgbT=0;
		paint_dock=0;
	}
}

void RgbTPlugin::mousePressEvent(QAction * /*ac*/, QMouseEvent *event,
		MeshModel &m, GLArea * /*gla*/)
{
	cur=event->pos();
	isDragging = true;
	selMode = SMClear;
	if(event->modifiers()==Qt::ControlModifier) selMode=SMAdd;
	if(event->modifiers()==Qt::ShiftModifier) selMode=SMSub;

	if (widgetRgbT->tool == TOOL_SELECTIONRECT)
	{
		if(event->modifiers() == Qt::ControlModifier || event->modifiers() == Qt::ShiftModifier )
		{
			updateSelectedFaces(m);
		}  
		startp=event->pos();
		cur=startp;
	}
}

void RgbTPlugin::mouseMoveEvent(QAction *, QMouseEvent * event,
		MeshModel &/*m*/, GLArea *gla)
{
	isDragging = true;
	cur=event->pos();


	if (widgetRgbT->tool == TOOL_SELECTIONRECT)
	{
		prevp=cur;
		isDragging = true;

		static int lastRendering=clock();
		int curT = clock();
		if(gla->lastRenderingTime() < 50 || (curT - lastRendering) > 1000 )
		{
			lastRendering=curT;
			gla->update();
		}
		else
		{
			gla->makeCurrent();
			glDrawBuffer(GL_FRONT);
			DrawXORRect(gla,true);
			glDrawBuffer(GL_BACK);
			glFlush();
		}

	}

	gla->update();
}

void RgbTPlugin::mouseReleaseEvent(QAction *, QMouseEvent *event,
		MeshModel &/*m*/, GLArea *gla)
{
	if (widgetRgbT->tool == TOOL_SELECTIONRECT)
	{
		gla->update();
		prevp=cur;
		cur=event->pos();
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
void RgbTPlugin::Decorate(QAction * /*ac*/, MeshModel &m, GLArea * gla)
{
	if (!isDragging) return;
	QPoint mid=QPoint(cur.x(),gla->curSiz.height()-cur.y());
	CMeshO::FaceIterator it;

	switch (widgetRgbT->tool)
	{
	case TOOL_SELECTIONSINGLE:

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
	case TOOL_SELECTIONRECT:
		if(isDragging)
		{
			DrawXORRect(gla,false);
			list<CMeshO::FacePointer>::iterator fpi;
			vector<CMeshO::FacePointer>::iterator fpiv;
			// Starting Sel 
			vector<CMeshO::FacePointer> NewSel;  
			QPoint mid=(startp+cur)/2;
			mid.setY(gla->curSiz.height()-  mid.y());
			QPoint wid=(startp-cur);
			if(wid.x()<0)  wid.setX(-wid.x());
			if(wid.y()<0)  wid.setY(-wid.y());

			CMeshO::FaceIterator fi;
			for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
				if(!(*fi).IsD()) (*fi).ClearS();

			GLPickTri<CMeshO>::PickFace(mid.x(), mid.y(), m.cm, NewSel, wid.x(), wid.y());

			switch(selMode)
			{ 
			case SMSub :
				for(fpi=selectedFaces.begin();fpi!=selectedFaces.end();++fpi)
					(*fpi)->SetS();
				for(fpiv=NewSel.begin();fpiv!=NewSel.end();++fpiv)
					(*fpiv)->ClearS();
				break;
			case SMAdd :
				for(fpi=selectedFaces.begin();fpi!=selectedFaces.end();++fpi)
					(*fpi)->SetS(); 
			case SMClear :
				for(fpiv=NewSel.begin();fpiv!=NewSel.end();++fpiv)
					(*fpiv)->SetS();
				break;
			}
			isDragging=false;
		}   
		break;
	}
	isDragging = false;
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
void RgbTPlugin::debugColorCGRG()
{
			std::list<CMeshO::FacePointer>::iterator it;
	
            for (it = selectedFaces.begin(); it != selectedFaces.end(); ++it)
            {
            	RgbTriangleC t = RgbTriangleC(m->cm,*rgbInfo,(*it)->Index());
            	std::cerr << t << std::endl;
            }
	/*
            std::list<CMeshO::FacePointer>::iterator it;
            std::cerr << "sel faces" << selectedFaces.size() << endl;
            for (it = selectedFaces.begin(); it != selectedFaces.end(); ++it)
            {
            	RgbTriangleC t = RgbTriangleC(m->cm,*rgbInfo,(*it)->Index());
            	std::cerr << t;
            	
            	for (int i = 0; i < 3; i++)
            	{
            	    if ((*it)->FFp(i) == (*it))
            	        std::cerr << "bug!" << endl;
            	    else
            	        std::cerr << "ok";
            	}
            	
                //CFaceO* fp = *it;
                //Color4b &color = fp->C();
                //color.SetRGB(255,0,0);
            }
            current_gla->update();
	 
	
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
		if (Utilities::commonVertex(sf,&e))
		{
			RgbTriangleC t = RgbTriangleC(m->cm,*rgbInfo,e.fp->Index());
			//vector<RgbTriangleC> fc;
			//RgbPrimitives::vf(t,e.i,fc);
			//fc[0].setFaceColor(FaceInfo::FACE_BLUE_GGR);
			//fc[1].setFaceColor(FaceInfo::FACE_BLUE_GGR);
			//fc[2].setFaceColor(FaceInfo::FACE_RED_GGR);
			//if (RgbPrimitives::rb_Pattern_Removal_Possible(t,e.i))
			//{
			//	RgbPrimitives::rb_Pattern_Removal(t,e.i,*to);
			//	std::cerr << "ok" << std::endl;
			//}
			
			//std::cerr << t.V(e.i).getCount() << std::endl;
			RgbVertexC v = t.V(e.i);
			std::cerr << "vindex: " << v.index << std::endl;
			std::cerr << "vlevel: " << v.getLevel() << std::endl;
			std::cerr << "vcount: " << v.getCount() << std::endl;
			
			vector<RgbVertexC> vv;
			ControlPoint::VV(v,vv);
			std::cout << vv.size() << std::endl;
			
			Point3f p(0,0,0);
			v.setPinf(p);
			v.setCount(0);
			v.setIsPinfReady(false);
			
			for (int i = 0; i < vv.size(); ++i) 
			{
				ControlPoint::addContributeIfPossible(v,vv[i]);
			}
			
			
		}
		selectedFaces.clear();
		

		current_gla->update();
	}
*/
	

}

void RgbTPlugin::debugEdgeCollapse()
{
	if (selectedFaces.size() == 2)
	{
		std::list<CMeshO::FacePointer>::iterator it = selectedFaces.begin();
		CMeshO::FacePointer f0 = *it;
		++it;
		CMeshO::FacePointer f1 = *it;

		typedef Utilities::EdgeFIType EdgeFIType;
		EdgeFIType e;
		if (Utilities::commonEdge(f0,f1,&e))
		{
			Utilities::PointType p;
			Utilities::PointType p1, p2;
			p1 = (e.fp->V(e.i)->P());
			p2 = (e.fp->V((e.i+1)%3)->P());

			for (int i = 0; i < 3; ++i)
			{
				p[i] = (p1[i] + p2[i]) / 2;
			}

			Utilities::doCollapse(m->cm,e,p);
		}
		selectedFaces.clear();

		current_gla->update();
	}
	
}

void RgbTPlugin::debugEdgeSplit()
{
	if (selectedFaces.size() == 2)
	{
		std::list<CMeshO::FacePointer>::iterator it = selectedFaces.begin();
		CMeshO::FacePointer f0 = *it;
		(*it)->ClearS();
		++it;
		(*it)->ClearS();
		CMeshO::FacePointer f1 = *it;

		typedef Utilities::EdgeFIType EdgeFIType;
		EdgeFIType e;
		if (Utilities::commonEdge(f0,f1,&e))
		{
			Utilities::PointType p;
			Utilities::PointType p1, p2;
			p1 = (e.fp->V(e.i)->P());
			p2 = (e.fp->V((e.i+1)%3)->P());

			for (int i = 0; i < 3; ++i)
			{
				p[i] = (p1[i] + p2[i]) / 2;
			}

			Utilities::doSplit(m->cm,e,p);
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
            Utilities::PointType p;
            Utilities::PointType p1, p2;
            p1 = (t.V(i).vert().P());
            p2 = (t.V((i+1)%3).vert().P());

            for (int i2 = 0; i2 < 3; ++i2)
            {
                p[i2] = (p1[i2] + p2[i2]) / 2;
            }
            
            if (t.getEdgeIsBorder(i))
            {
                to->doSplitBoundary(t.face(),i,p);
                
            }
            
        }
        
	}
	
    selectedFaces.clear();
    current_gla->update();
}

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

		typedef Utilities::EdgeFIType EdgeFIType;
		EdgeFIType e;
		if (Utilities::commonEdge(f0,f1,&e))
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
            Utilities::PointType p;
            Utilities::PointType p1, p2;
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
		if (Utilities::commonVertex(sf,&e))
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

void RgbTPlugin::start()
{
	sr->maxTriangles = widgetRgbT->sMaxTriangles->value();
	sr->maxEdgeLenght = widgetRgbT->sPickEdgeOutside ->value();
	sr->maxEdgeLenghtInBox = widgetRgbT->sPickEdgeBox->value();
	sr->maxEdgeLevelInBox = widgetRgbT->sMaxEdgeLevel ->value();
	sr->minEdgeLevel = widgetRgbT->sMinEdgeLevel->value();
	
	sr->start(widgetRgbT->cbInteractive->isChecked(),widgetRgbT->algo_simple->isChecked());
	
	if (widgetRgbT->cbInteractive->isChecked())
	{
		startThread();
	}
	
	current_gla->update();
}
void RgbTPlugin::stop()
{
	killThread();
	sr->stop();
	current_gla->update();
}
void RgbTPlugin::step()
{
	if (!sr->step())
		sr->stop();
	current_gla->update();
}

void RgbTPlugin::resume()
{
	startThread();
}

void RgbTPlugin::pause()
{
	killThread();
}

void RgbTPlugin::pickEdgeOutside()
{
	double d;
	if (pickEdgeLenght(d))
	{
		widgetRgbT->sPickEdgeOutside->setValue(d);
	}
}

void RgbTPlugin::startThread()
{
	if (srThread)
	{
		if (srThread->isFinished())
		{
			delete srThread;
		}
		else
			return;
	}
	//std::cerr << widgetRgbT->sSpeed->value() << std::endl;
	srThread = new SelRefThread(current_gla,sr,widgetRgbT->sSpeed->value());
	srThread->start();
}

void RgbTPlugin::killThread()
{
	if (!srThread)
		return;
	srThread->reqTerm = true;
}


void RgbTPlugin::pickEdgeBox()
{
	double d;
	if (pickEdgeLenght(d))
	{
		widgetRgbT->sPickEdgeBox->setValue(d);
	}
}

void RgbTPlugin::pickFace()
{
	widgetRgbT->sMaxTriangles->setValue(m->cm.fn);
}

bool RgbTPlugin::pickEdgeLenght(double& d)
{
	if (selectedFaces.size() == 2)
	{
		std::list<CMeshO::FacePointer>::iterator it = selectedFaces.begin();
		CMeshO::FacePointer f0 = *it;
		++it;
		CMeshO::FacePointer f1 = *it;

		typedef Utilities::EdgeFIType EdgeFIType;
		EdgeFIType e;
		if (Utilities::commonEdge(f0,f1,&e))
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

void SelRefThread::run()
{
	reqTerm = false;
	bool b = true;
	while(b)
	{
		
		if (reqTerm)
			return;
		b = sr->step();
		if (!b)
			sr->stop();
		gla->update();
		msleep(50+delay);
	}
}

Q_EXPORT_PLUGIN(RgbTPlugin)

}

