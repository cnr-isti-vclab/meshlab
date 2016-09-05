/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#include "decorate_base.h"
#include <wrap/gl/addons.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/bitquad_support.h>
#include <meshlab/glarea.h>
#include <wrap/qt/checkGLError.h>
#include <wrap/qt/gl_label.h>
#include <QGLShader>
#include <meshlab/glarea_setting.h>
#include <wrap/gl/gl_type_name.h>
using namespace vcg;
using namespace std;


QString DecorateBasePlugin::decorationInfo(FilterIDType filter) const
{
    switch(filter)
    {
    case DP_SHOW_AXIS:              return tr("Draw XYZ axes in world coordinates");
    case DP_SHOW_BOX_CORNERS:       return tr("Draw object's bounding box corners");
    case DP_SHOW_NORMALS:           return tr("Draw per vertex/face normals");
    case DP_SHOW_CURVATURE:         return tr("Draw per vertex/face principal curvature directions");
    case DP_SHOW_LABEL:             return tr("Draw on all the vertex/edge/face a label with their index<br> Useful for debugging<br>(WARNING: do not use it on large meshes)");
    case DP_SHOW_QUALITY_HISTOGRAM: return tr("Draw a (colored) Histogram of the per vertex/face quality");
    case DP_SHOW_QUALITY_CONTOUR:   return tr("Draw quality contours, e.g. the isolines of the quality field defined over the surface ");
    case DP_SHOW_CAMERA:            return tr("Draw the position of the mesh camera and raster cameras");
    case DP_SHOW_TEXPARAM:          return tr("Draw an overlayed flattened version of the current mesh that show the current parametrization");
    case DP_SHOW_SELECTED_MESH:     return tr("Highlight the current mesh");
    }
    assert(0);
    return QString();
}

QString DecorateBasePlugin::decorationName(FilterIDType filter) const
{
    switch(filter)
    {
    case DP_SHOW_NORMALS:           return QString("Show Normal");
    case DP_SHOW_CURVATURE:         return QString("Show Curvature");
    case DP_SHOW_BOX_CORNERS:       return QString("Show Box Corners");
    case DP_SHOW_AXIS:              return QString("Show Axis");
    case DP_SHOW_LABEL:             return QString("Show Labels");
    case DP_SHOW_CAMERA:            return QString("Show Camera");
    case DP_SHOW_TEXPARAM:          return QString("Show UV Tex Param");
    case DP_SHOW_QUALITY_HISTOGRAM: return QString("Show Quality Histogram");
    case DP_SHOW_QUALITY_CONTOUR:   return QString("Show Quality Contour");
    case DP_SHOW_SELECTED_MESH:     return QString("Show Current Mesh");
    default: assert(0);
    }
    return QString("error!");
}

void DecorateBasePlugin::decorateDoc(QAction *a, MeshDocument &md, RichParameterSet *rm, GLArea *gla, QPainter *painter,GLLogStream &/*_log*/)
{
 QFont qf;

 switch (ID(a))
 {

    case DP_SHOW_CAMERA:
	{
		bool showCameraDetails = rm->getBool(ShowCameraDetails());

		// draw all visible mesh cameras
		if(rm->getBool(ShowMeshCameras()))
		{
			foreach(MeshModel *meshm,  md.meshList)
			{
				if (meshm != md.mm() || (!showCameraDetails))   // non-selected meshes, only draw 
				{
					if (meshm->visible) DrawCamera(meshm, meshm->cm.shot, Color4b::DarkRed, md.mm()->cm.Tr, rm, painter, qf);
				}
				else                          // selected mesh, draw & display data
				{
					DrawCamera(meshm, meshm->cm.shot, Color4b::Magenta, md.mm()->cm.Tr, rm, painter, qf);
					DisplayCamera(meshm->label(), meshm->cm.shot, 1);
				}
			}

			if (md.meshList.size() == 0)
				this->RealTimeLog("Show Mesh Camera", md.mm()->label(), "There are no Mesh Layers");
		}

		// draw all visible raster cameras
		if(rm->getBool(ShowRasterCameras()))
		{
			foreach(RasterModel *raster, md.rasterList)
			{
				if(raster != md.rm() || !showCameraDetails )   // non-selected raster, only draw
				{
					if(raster->visible) DrawCamera(NULL, raster->shot, Color4b::DarkBlue, md.mm()->cm.Tr, rm, painter,qf);
				}
				else                          // selected raster, draw & display data
				{
					DrawCamera(NULL, raster->shot, Color4b::Cyan, md.mm()->cm.Tr, rm, painter, qf);
					DisplayCamera(raster->label(), raster->shot, 2);
				}
			}

			if (md.rasterList.size() == 0)
				this->RealTimeLog("Show Raster Camera", md.mm()->label(), "There are no Rasters");
		}
	} break;

	case DP_SHOW_SELECTED_MESH:
	{
		if ((gla == NULL) || (gla->mvc() == NULL))
		return;

		glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		vcg::Color4b q = rm->getColor4b(selectedMeshBlendingColor());
		glBlendColor(q.X() / 255.0f, q.Y() / 255.0f, q.Z() / 255.0f, q.W() / 255.0f);
		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE);
		glDepthRange (0.0, 0.9999);
		glDepthFunc(GL_LEQUAL);
		glPointSize(3);
	
		MLSceneGLSharedDataContext* sharedcont = gla->mvc()->sharedDataContext();
		if ((sharedcont != NULL) && (md.mm() != NULL))
		{
			sharedcont->draw(md.mm()->id(),gla->context());
		}
		glPopAttrib();
	} break;

	case DP_SHOW_AXIS:
	{
		CoordinateFrame(md.bbox().Diag()/2.0).Render(gla,painter);
	} break;

 } // end switch
}

void DecorateBasePlugin::decorateMesh(QAction *a, MeshModel &m, RichParameterSet *rm, GLArea *gla, QPainter *painter,GLLogStream &_log)
{
    this->setLog(&_log);
    QFont qf;

    textColor = rm->getColor4b( GLAreaSetting::textColorParam());

    glPushMatrix();
    glMultMatrix(m.cm.Tr);
    switch (ID(a))
    {
    case DP_SHOW_CURVATURE:
        {
            // Note the standard way for adding extra per-mesh data using the per-mesh attributes.
            CMeshO::PerMeshAttributeHandle< vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<PointPC> >(m.cm,"CurvatureVector");
            DrawLineVector(bvH());
        } break;
    case DP_SHOW_NORMALS:
        {
            glPushAttrib(GL_ENABLE_BIT );
            float NormalLen=rm->getFloat(NormalLength());
            float LineLen = m.cm.bbox.Diag()*NormalLen;
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_LINES);
            if(rm->getBool(NormalVertFlag())) // vert Normals
            {
                glColor4f(.4f,.4f,1.f,.6f);
                for(CMeshO::VertexIterator vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
                {
                    glVertex((*vi).P());
                    glVertex((*vi).P()+(*vi).N()*LineLen);
                }
            }
            if(rm->getBool(NormalFaceFlag())) // face Normals
            {
                glColor4f(.1f,.4f,4.f,.6f);
                for(CMeshO::FaceIterator fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
                {
                    Point3m b=Barycenter(*fi);
                    glVertex(b);
                    glVertex(b+(*fi).N()*LineLen);
                }
            }
            glEnd();
            glPopAttrib();
        } break;
    case DP_SHOW_BOX_CORNERS:
        {
			bool untrasformed = rm->getBool(this->BBAbsParam());
            DrawBBoxCorner(m, untrasformed);

			Point3m bmin, bmax;
			bmin = m.cm.bbox.min;
			bmax = m.cm.bbox.max;

            this->RealTimeLog("Bounding Box", m.label(), "<table>"
                "<tr><td>Min: </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
                "<tr><td>Max: </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
				"<tr><td>Size: </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
				"<tr><td>Center: </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
				"</table>""Warning: values do not consider transformation", bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], 
				math::Abs(bmax[0] - bmin[0]), math::Abs(bmax[1] - bmin[1]), math::Abs(bmax[2] - bmin[2]), 
				(bmax[0] + bmin[0]) / 2.0, (bmax[1] + bmin[1]) / 2.0, (bmax[2] + bmin[2]) / 2.0);
        }
        break;
    case DP_SHOW_LABEL:
        {
            if(rm->getBool(LabelVertFlag())) DrawVertLabel(m,painter);
            if(rm->getBool(LabelEdgeFlag())) DrawEdgeLabel(m,painter);
            if(rm->getBool(LabelFaceFlag())) DrawFaceLabel(m,painter);
        } break;
    case DP_SHOW_TEXPARAM : this->DrawTexParam(m,gla,painter,rm,qf); break;

    case DP_SHOW_QUALITY_HISTOGRAM :
        {
            CMeshO::PerMeshAttributeHandle<CHist > qH;
            qH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<CHist>(m.cm,"QualityHist");
            CHist &ch=qH();
            this->DrawColorHistogram(ch,gla, painter,rm,qf);
        } break;
    case DP_SHOW_QUALITY_CONTOUR :
        {
            glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
            glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthRange (0.0, 0.9999);
            glDepthFunc(GL_LEQUAL);
            //      glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
            QGLShaderProgram *glp=this->contourShaderProgramMap[&m];

            CMeshO::PerMeshAttributeHandle< pair<float,float> > mmqH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<pair<float,float> >(m.cm,"minmaxQ");
			this->RealTimeLog("Quality Contour", m.label(),
                "min Q %f -- max Q %f",mmqH().first,mmqH().second);

            float stripe_num = rm->getFloat(this->ShowContourFreq());
            float stripe_width = rm->getFloat(this->ShowContourWidth());
            float stripe_alpha = rm->getFloat(this->ShowContourAlpha());
            bool stripe_ramp = rm->getBool(this->ShowContourRamp());
            glp->bind();
            glp->setUniformValue("quality_min",mmqH().first);
            glp->setUniformValue("quality_max",mmqH().second);
            glp->setUniformValue("stripe_num",stripe_num);
            glp->setUniformValue("stripe_width",stripe_width);
            glp->setUniformValue("stripe_alpha",stripe_alpha);
            glp->setUniformValue("stripe_ramp",stripe_ramp);


            int vert_quality = glp->attributeLocation("vert_quality");
            glBegin(GL_TRIANGLES);
            for(CMeshO::FaceIterator fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
            {
                glp->setAttributeValue(vert_quality,fi->V(0)->Q());
                glVertex(fi->V(0)->P());
                glp->setAttributeValue(vert_quality,fi->V(1)->Q());
                glVertex(fi->V(1)->P());
                glp->setAttributeValue(vert_quality,fi->V(2)->Q());
                glVertex(fi->V(2)->P());
            }
            glEnd();
            glp->release();
            glPopAttrib();

        } break;

    } // end switch;
    glPopMatrix();
}

void DecorateBasePlugin::DrawLineVector(std::vector<PointPC> &EV)
{
    glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT| GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.f);
    glDepthRange (0.0, 0.999);
    if (EV.size() > 0)
    {
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_COLOR_ARRAY);

        glVertexPointer(3,vcg::GL_TYPE_NM<Scalarm>::SCALAR(),sizeof(PointPC),&(EV.begin()[0].first));
        glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(EV.begin()[0].second));
        glDrawArrays(GL_LINES,0,EV.size());
        glDisableClientState (GL_COLOR_ARRAY);
        glDisableClientState (GL_VERTEX_ARRAY);
    }
    glPopAttrib();
}

/**
Draw a line with labeled ticks.
\param a,b the two endpoints of the line (in 3D)
\param aVal,bVal the two values at the line endpoints.
\param tickScalarDistance the distance between labeled ticks ( 1/10 small ticks are added between them).

for example if you want the a quoted line between 0.363 and 1.567 tickScalarDistance == 0.1 it means that
you will have a line with labeled ticks at 0.4 0.5 etc.
*/



void DecorateBasePlugin::drawQuotedLine(const Point3d &a,const Point3d &b, float aVal, float bVal, float tickScalarDistance, QPainter *painter, QFont qf,float angle,bool rightAlign)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE);
    float labelMargin =tickScalarDistance /4.0;
    float firstTick;
    // fmod returns the floating-point remainder of numerator/denominator (with the sign of the dividend)
    // fmod ( 104.5 , 10) returns 4.5     --> aVal - fmod(aval/tick) = 100
    // fmod ( -104.5 , 10) returns -4.5
    // So it holds that

    if(aVal > 0 ) firstTick = aVal - fmod(aVal,tickScalarDistance) + tickScalarDistance;
    if(aVal ==0 ) firstTick = tickScalarDistance;
    if(aVal < 0 ) firstTick = aVal + fmod(fabs(aVal),tickScalarDistance);

    // now we are sure that aVal < firstTick
    // let also be sure that there is enough space
    if ( (firstTick-aVal) < (labelMargin) )
        firstTick +=tickScalarDistance;


    float tickDistTen=tickScalarDistance /10.0f;
    float firstTickTen;
    if(aVal > 0) firstTickTen = aVal - fmod(aVal,tickDistTen) + tickDistTen;
    else firstTickTen = aVal - fmod(aVal,tickDistTen);

    int neededZeros=0;

    Point3d Zero = a-((b-a)/(bVal-aVal))*aVal; // 3D Position of Zero.
    Point3d v(b-a);
    //v.Normalize();
    v = v*(1.0/(bVal-aVal));
    glLabel::Mode md(qf,textColor,angle,rightAlign);
    if(tickScalarDistance > 0)   // Draw lines only if the two endpoint are not coincident
    {
        neededZeros = ceil(max(0.0,-log10(double(tickScalarDistance))));
        glPointSize(3);
        float i;
        glBegin(GL_POINTS);
        for(i=firstTick;i<bVal;i+=tickScalarDistance)
            glVertex(Zero+v*i);
        glEnd();
        for(i=firstTick; (i+labelMargin)<bVal;i+=tickScalarDistance)
            glLabel::render(painter,Point3m::Construct(Zero+v*i),tr("%1   ").arg(i,4+neededZeros,'f',neededZeros),md);
        glPointSize(1);
        glBegin(GL_POINTS);
        for(i=firstTickTen;i<bVal;i+=tickDistTen)
            glVertex(Zero+v*i);
        glEnd();
    }

    // Draws bigger ticks at 0 and at max size
    glPointSize(6);

    glBegin(GL_POINTS);
    glVertex(a);
    glVertex(b);
    if(bVal*aVal<0) glVertex(Zero);
    glEnd();


    // bold font at beginning and at the end
    md.qFont.setBold(true);
    glLabel::render(painter,Point3f::Construct(a), tr("%1   ").arg(aVal,4+neededZeros,'f',neededZeros) ,md);
    glLabel::render(painter,Point3f::Construct(b), tr("%1   ").arg(bVal,4+neededZeros,'f',neededZeros) ,md);

    glPopAttrib();
}


float DecorateBasePlugin::niceRound2(float Val,float base)	{return powf(base,ceil(log10(Val)/log10(base)));}
float DecorateBasePlugin::niceRound(float val)	{return powf(10.f,ceil(log10(val)));}

void DecorateBasePlugin::DrawBBoxCorner(MeshModel &m, bool absBBoxFlag)
{
    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);
    glColor(vcg::Color4b(vcg::Color4b::Cyan));
    Box3f b;
    b.Import(m.cm.bbox);
    if(absBBoxFlag) {
        glColor(vcg::Color4b(vcg::Color4b::Cyan));
    } else {
        glColor(vcg::Color4b(vcg::Color4b::Green));
    }
    Point3f mi=b.min;
    Point3f ma=b.max;
    Point3f d3=(b.max-b.min)/4.0;
    Point3f zz(0,0,0);

    glPushMatrix();
    if (absBBoxFlag)
		glMultMatrix(Inverse(m.cm.Tr));

    glBegin(GL_LINES);
    glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+d3[0],mi[1]+zz[1],mi[2]+zz[2]);
    glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+zz[0],mi[1]+d3[1],mi[2]+zz[2]);
    glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+zz[0],mi[1]+zz[1],mi[2]+d3[2]);

    glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]-d3[0],mi[1]+zz[1],mi[2]+zz[2]);
    glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]+zz[0],mi[1]+d3[1],mi[2]+zz[2]);
    glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]+zz[0],mi[1]+zz[1],mi[2]+d3[2]);

    glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+d3[0],ma[1]+zz[1],mi[2]+zz[2]);
    glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+zz[0],ma[1]-d3[1],mi[2]+zz[2]);
    glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+zz[0],ma[1]+zz[1],mi[2]+d3[2]);

    glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]-d3[0],ma[1]+zz[1],mi[2]+zz[2]);
    glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]+zz[0],ma[1]-d3[1],mi[2]+zz[2]);
    glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]+zz[0],ma[1]+zz[1],mi[2]+d3[2]);

    glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+d3[0],mi[1]+zz[1],ma[2]+zz[2]);
    glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+zz[0],mi[1]+d3[1],ma[2]+zz[2]);
    glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+zz[0],mi[1]+zz[1],ma[2]-d3[2]);

    glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]-d3[0],mi[1]+zz[1],ma[2]+zz[2]);
    glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]+zz[0],mi[1]+d3[1],ma[2]+zz[2]);
    glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]+zz[0],mi[1]+zz[1],ma[2]-d3[2]);

    glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+d3[0],ma[1]+zz[1],ma[2]+zz[2]);
    glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+zz[0],ma[1]-d3[1],ma[2]+zz[2]);
    glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+zz[0],ma[1]+zz[1],ma[2]-d3[2]);

    glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]-d3[0],ma[1]+zz[1],ma[2]+zz[2]);
    glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]+zz[0],ma[1]-d3[1],ma[2]+zz[2]);
    glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]+zz[0],ma[1]+zz[1],ma[2]-d3[2]);
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

int DecorateBasePlugin::getDecorationClass(QAction *action) const
{
    switch(ID(action))
    {
    case DP_SHOW_NORMALS :
    case DP_SHOW_CURVATURE :
    case DP_SHOW_QUALITY_HISTOGRAM :
    case DP_SHOW_QUALITY_CONTOUR :
    case DP_SHOW_BOX_CORNERS :
    case DP_SHOW_LABEL :
    case DP_SHOW_TEXPARAM : return DecorateBasePlugin::PerMesh;
    case DP_SHOW_AXIS : return DecorateBasePlugin::PerDocument;
    case DP_SHOW_CAMERA : return DecorateBasePlugin::PerDocument;
    case DP_SHOW_SELECTED_MESH : return DecorateBasePlugin::PerDocument;
    }
    assert (0);
    return 0;
}

bool DecorateBasePlugin::isDecorationApplicable(QAction *action, const MeshModel& m, QString &ErrorMessage) const
{
    if( ID(action) == DP_SHOW_LABEL )
    {
        if(m.cm.vn <1000 && m.cm.fn<2000) 
            return true;
        else 
        {
            ErrorMessage=QString("<br>CANNOT START DECORATOR: the layer contains too many faces and vertices.<br>Printing on the screen thousand of numbers would be useless and VERY SLOW");
            return false;
        }
    }
	if (ID(action) == DP_SHOW_QUALITY_HISTOGRAM || ID(action) == DP_SHOW_QUALITY_CONTOUR)
	{
		if (m.hasDataMask(MeshModel::MM_FACEQUALITY) || m.hasDataMask(MeshModel::MM_VERTQUALITY))
			return true;
		else
		{
			ErrorMessage = QString("<br>CANNOT START DECORATOR: the layer contains neither vertex nor face quality");
			return false;
		}
	}

	if (ID(action) == DP_SHOW_CURVATURE)
	{
		if (m.hasDataMask(MeshModel::MM_VERTCURVDIR) || m.hasDataMask(MeshModel::MM_FACECURVDIR))
			return true;
		else
		{
			ErrorMessage = QString("<br>CANNOT START DECORATOR: the layer contains neither vertex nor face curvature attribute");
			return false;
		}
	}

    return true;
}

bool DecorateBasePlugin::startDecorate(QAction * action, MeshDocument &, RichParameterSet *, GLArea *)
{
    switch(ID(action))
    {
    case DP_SHOW_AXIS :
    case DP_SHOW_CAMERA :
        return true;
    case DP_SHOW_SELECTED_MESH :
        {
            return (glewInit() == GLEW_OK);
        }
    
    }
    return true;
}


void DecorateBasePlugin::endDecorate(QAction * action, MeshModel &m, RichParameterSet *, GLArea *)
{
    switch(ID(action))
    {
    case DP_SHOW_QUALITY_CONTOUR :
        if(this->contourShaderProgramMap[&m]!=0)
        {
            delete this->contourShaderProgramMap[&m];
            this->contourShaderProgramMap[&m]=0;
        }
        break;
    default: break;
    }
}

bool DecorateBasePlugin::startDecorate(QAction * action, MeshModel &m, RichParameterSet *rm, GLArea *gla)
{
    switch(ID(action))
    {
    case DP_SHOW_CURVATURE :
        {
            CMeshO::PerMeshAttributeHandle< vector<PointPC> > cvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<PointPC> >(m.cm,"CurvatureVector");
            vector<PointPC> *CVp = &cvH();
            CVp->clear();
            float NormalLen=rm->getFloat(CurvatureLength());
            float LineLen = m.cm.bbox.Diag()*NormalLen;
            if(rm->getBool(this->ShowPerVertexCurvature()))
            {
                for(CMeshO::VertexIterator vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
                    if(!(*vi).IsD())
                    {
                        CVp->push_back(make_pair((*vi).P(),
                            Color4b::Green));
                        CVp->push_back(make_pair((*vi).P() +Point3m::Construct((*vi).PD1()/Norm((*vi).PD1())*LineLen*0.25),
                            Color4b::Green));
                        CVp->push_back(make_pair((*vi).P(),
                            Color4b::Red));
                        CVp->push_back(make_pair((*vi).P()+Point3m::Construct((*vi).PD2()/Norm((*vi).PD2())*LineLen*0.25),
                            Color4b::Red));
                    }
            }
            if(rm->getBool(this->ShowPerFaceCurvature()))
            {
                for(CMeshO::FaceIterator fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
                    if(!(*fi).IsD())
                    {
                        Point3m bar =  Barycenter(*fi);
                        CVp->push_back(make_pair(bar, Color4b::Green));
                        CVp->push_back(make_pair(bar +(*fi).PD1()/Norm((*fi).PD1())*LineLen*0.25,
                            Color4b::Green));
                        CVp->push_back(make_pair(bar, Color4b::Red));
                        CVp->push_back(make_pair(bar +(*fi).PD2()/Norm((*fi).PD2())*LineLen*0.25,
                            Color4b::Red));
                    }
            }
        } break;

    case DP_SHOW_QUALITY_HISTOGRAM :
        {
            bool perVertFlag = rm->getEnum(HistTypeParam()) == 0;
            if( perVertFlag && !(tri::HasPerVertexQuality(m.cm) && tri::HasPerVertexColor(m.cm)) ) return false;
            if(!perVertFlag && !(tri::HasPerFaceQuality(m.cm) && tri::HasPerFaceColor(m.cm)) ) return false;
            CMeshO::PerMeshAttributeHandle<CHist > qH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<CHist>(m.cm,"QualityHist");

            CHist *H = &qH();
            std::pair<float,float> minmax;
            if(perVertFlag) minmax = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
            else minmax = tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m.cm);
            if(rm->getBool(HistFixedParam())) {
                minmax.first=rm->getFloat(HistFixedMinParam());
                minmax.second=rm->getFloat(HistFixedMaxParam());
            }

            H->SetRange( minmax.first, minmax.second, rm->getInt(HistBinNumParam()));
            if(perVertFlag)
            {
                if(rm->getBool(HistAreaParam()))
                {
                    for(CMeshO::FaceIterator fi = m.cm.face.begin(); fi!= m.cm.face.end();++fi) if(!(*fi).IsD())
                    {
                        float area6=DoubleArea(*fi)/6.0f;
                        for(int i=0;i<3;++i)
                            H->Add((*fi).V(i)->Q(),(*fi).V(i)->C(),area6);
                    }
                } else {
                    for(CMeshO::VertexIterator vi = m.cm.vert.begin(); vi!= m.cm.vert.end();++vi) if(!(*vi).IsD())
                    {
                        H->Add((*vi).Q(),(*vi).C(),1.0f);
                    }
                }
            }
            else{
                if(rm->getBool(HistAreaParam())) {
                    for(CMeshO::FaceIterator fi = m.cm.face.begin(); fi!= m.cm.face.end();++fi) if(!(*fi).IsD())
                        H->Add((*fi).Q(),(*fi).C(),DoubleArea(*fi)*0.5f);
                } else {
                    for(CMeshO::FaceIterator fi = m.cm.face.begin(); fi!= m.cm.face.end();++fi) if(!(*fi).IsD())
                        H->Add((*fi).Q(),(*fi).C(),1.0f);
                }
            }
        }
        break;
    case DP_SHOW_QUALITY_CONTOUR :
        {
            tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm);
            CMeshO::PerMeshAttributeHandle< pair<float,float> > mmqH;
            mmqH = vcg::tri::Allocator<CMeshO>::FindPerMeshAttribute<pair<float,float> >(m.cm,"minmaxQ");
            if(this->contourShaderProgramMap[&m] == 0)
            {
                bool ret=true;
                this->contourShaderProgramMap[&m] = new QGLShaderProgram(gla);
                QGLShaderProgram *gsp =  this->contourShaderProgramMap[&m];

                ret &= gsp->addShaderFromSourceFile(QGLShader::Vertex,":/decorate/contour.vert");
                //      qDebug("Compiled shader. Log is %s", qPrintable(contourShaderProgram->log()));
                ret &= gsp->addShaderFromSourceFile(QGLShader::Fragment,":/decorate/contour.frag");
                //      qDebug("Compiled shader. Log is %s", qPrintable(contourShaderProgram->log()));
                ret &= gsp->link();
                //      qDebug("Linked shader. Log is %s", qPrintable(contourShaderProgram->log()));
                if(!ret) return false;
            }
        } break;

    case DP_SHOW_CAMERA :
        {
            connect(gla,SIGNAL(transmitShot(QString,vcg::Shotf)),this,SLOT(setValue(QString,vcg::Shotf)));
            connect(this,SIGNAL(askViewerShot(QString)),gla,SLOT(sendViewerShot(QString)));
        } break;
    }
    return true;
}

void DecorateBasePlugin::DrawFaceLabel(MeshModel &m, QPainter *painter)
{
    glPushAttrib(GL_LIGHTING_BIT  | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_LIGHTING);
    glColor3f(.4f,.4f,.4f);
    for(size_t i=0;i<m.cm.face.size();++i)
        if(!m.cm.face[i].IsD())
        {
            Point3m bar=Barycenter(m.cm.face[i]);
            glLabel::render(painter, bar,tr("%1").arg(i),glLabel::Mode(textColor));
        }
        glPopAttrib();
}

void DecorateBasePlugin::DrawEdgeLabel(MeshModel &m,QPainter *painter)
{
    glPushAttrib(GL_LIGHTING_BIT  | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_LIGHTING);
    glColor3f(.4f,.4f,.4f);
    for(size_t i=0;i<m.cm.edge.size();++i)
        if(!m.cm.edge[i].IsD())
        {
            Point3m bar=(m.cm.edge[i].V(0)->P()+m.cm.edge[i].V(0)->P())/2.0f;
            glLabel::render(painter, bar,tr("%1").arg(i),glLabel::Mode(textColor));
        }
        glPopAttrib();
}


void DecorateBasePlugin::DrawVertLabel(MeshModel &m,QPainter *painter)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_LIGHTING);
    glColor3f(.4f,.4f,.4f);
    for(size_t i=0;i<m.cm.vert.size();++i){
        if(!m.cm.vert[i].IsD())
            glLabel::render(painter, m.cm.vert[i].P(),tr("%1").arg(i),glLabel::Mode(textColor));
    }
    glPopAttrib();
}

void DecorateBasePlugin::setValue(QString /*name*/,Shotf newVal)
{
    curShot=newVal;
}


void DecorateBasePlugin::DisplayCamera(QString who, Shotm &ls, int cameraSourceId)
{
    if(!ls.IsValid())
    {
        if(cameraSourceId == 1 )
			this->RealTimeLog("Show Mesh Camera", who, "Current Mesh Has an invalid Camera");
        else if(cameraSourceId == 2 )
			this->RealTimeLog("Show Raster Camera", who, "Current Raster Has an invalid Camera");
        else
			this->RealTimeLog("Show Camera", who, "Current TrackBall Has an invalid Camera");
        return;
    }

    const char *typeBuf;
    if(ls.Intrinsics.cameraType == Camera<float>::PERSPECTIVE) typeBuf="Persp";
    if(ls.Intrinsics.cameraType == Camera<float>::ORTHO)       typeBuf="Ortho";

    Point3m vp = ls.GetViewPoint();
    Point3m ax0 = ls.Axis(0);
    Point3m ax1 = ls.Axis(1);
    Point3m ax2 = ls.Axis(2);
    float fov = ls.GetFovFromFocal();
    float focal = ls.Intrinsics.FocalMm;
    //  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("ViewPoint %1 %2 %3").arg(vp[0]).arg(vp[1]).arg(vp[2]));
    //  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("axis 0 - %1 %2 %3").arg(ax0[0]).arg(ax0[1]).arg(ax0[2]));
    //  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("axis 1 - %1 %2 %3").arg(ax1[0]).arg(ax1[1]).arg(ax1[2]));
    //  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("axis 2 - %1 %2 %3").arg(ax2[0]).arg(ax2[1]).arg(ax2[2]));
    //  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("Fov %1 ( %2 x %3) ").arg(fov).arg(ls.Intrinsics.ViewportPx[0]).arg(ls.Intrinsics.ViewportPx[1]));
    //  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("Focal Lenght %1 (pxsize %2 x %3) ").arg(focal).arg(ls.Intrinsics.PixelSizeMm[0]).arg(ls.Intrinsics.PixelSizeMm[1]));


	this->RealTimeLog("Camera Info", who,
        "<table>"
        "<tr><td>Viewpoint: </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
        "<tr><td>axis 0:    </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
        "<tr><td>axis 1:    </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
        "<tr><td>axis 2:    </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
        "</table><br>"
        "<table>"
        "<tr><td>FOV (%s): %7.4f     </td><td> Viewport (%i  x %i)</td></tr>"
        "<tr><td>Focal Length %7.4f  </td><td> PxSize (%.4f x %.4f)</td></tr>"
        "</table>",
        vp[0],vp[1],vp[2],
        ax0[0],ax0[1],ax0[2],
        ax1[0],ax1[1],ax1[2],
        ax2[0],ax2[1],ax2[2],
        typeBuf, fov,  ls.Intrinsics.ViewportPx[0], ls.Intrinsics.ViewportPx[1],
        focal,ls.Intrinsics.PixelSizeMm[0],ls.Intrinsics.PixelSizeMm[1]);
}

void DecorateBasePlugin::DrawCamera(MeshModel *m, Shotm &ls, vcg::Color4b camcolor, Matrix44m &currtr, RichParameterSet *rm, QPainter */*painter*/, QFont /*qf*/)
{
    if(!ls.IsValid())  // no drawing if camera not valid
        return;

    Point3m vp = ls.GetViewPoint();
    Point3m ax0 = ls.Axis(0);
    Point3m ax1 = ls.Axis(1);
    Point3m ax2 = ls.Axis(2);

    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
    glDepthFunc(GL_ALWAYS);
    glDisable(GL_LIGHTING);

    if(ls.Intrinsics.cameraType == Camera<float>::PERSPECTIVE)
    {
        // draw scale
        float drawscale = 1.0;
        if(rm->getEnum(CameraScaleParam()) == 1)  // fixed scale
        {
            drawscale = rm->getFloat(FixedScaleParam());
        }

        // arbitrary size to draw axis
        float len;
        len = ls.Intrinsics.FocalMm * drawscale;

        glPushMatrix();
        glMultMatrix(Inverse(currtr));  //remove current mesh transform

        // grey axis, aligned with scene axis
        glColor3f(.7f,.7f,.7f);
        glBegin(GL_LINES);
        glVertex3f(vp[0]-(len/2.0),vp[1],vp[2]); 	glVertex3f(vp[0]+(len/2.0),vp[1],vp[2]);
        glVertex3f(vp[0],vp[1]-(len/2.0),vp[2]); 	glVertex3f(vp[0],vp[1]+(len/2.0),vp[2]);
        glVertex3f(vp[0],vp[1],vp[2]-(len/2.0)); 	glVertex3f(vp[0],vp[1],vp[2]+(len/2.0));
        glEnd();

        if(m!=NULL) //if mesh camera, apply mesh transform
            glMultMatrix(m->cm.Tr);

        // RGB axis, aligned with camera axis
        glBegin(GL_LINES);
        glColor3f(1.0,0,0); glVertex(vp); 	glVertex(vp+ax0*len);
        glColor3f(0,1.0,0); glVertex(vp); 	glVertex(vp+ax1*len);
        glColor3f(0,0,1.0); glVertex(vp); 	glVertex(vp+ax2*len);
        glEnd();

        // Now draw the frustum
        Point3m viewportCenter = vp - (ax2*ls.Intrinsics.FocalMm * drawscale);
        Point3m viewportHorizontal = ax0* float(ls.Intrinsics.ViewportPx[0]*ls.Intrinsics.PixelSizeMm[0]/2.0f * drawscale);
        Point3m viewportVertical   = ax1* float(ls.Intrinsics.ViewportPx[1]*ls.Intrinsics.PixelSizeMm[1]/2.0f * drawscale);

        glBegin(GL_LINES);
        glColor(camcolor);
        glVertex3f(vp[0],vp[1],vp[2]); glVertex(viewportCenter);
        glColor(camcolor);
        glVertex(vp); glVertex(viewportCenter+viewportHorizontal+viewportVertical);
        glVertex(vp); glVertex(viewportCenter+viewportHorizontal-viewportVertical);
        glVertex(vp); glVertex(viewportCenter-viewportHorizontal+viewportVertical);
        glVertex(vp); glVertex(viewportCenter-viewportHorizontal-viewportVertical);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glVertex(viewportCenter+viewportHorizontal+viewportVertical);
        glVertex(viewportCenter+viewportHorizontal-viewportVertical);
        glVertex(viewportCenter-viewportHorizontal-viewportVertical);
        glVertex(viewportCenter-viewportHorizontal+viewportVertical);
        glEnd();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(.8f,.8f,.8f,.2f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex(vp);
        glVertex(viewportCenter+viewportHorizontal+viewportVertical);
        glVertex(viewportCenter+viewportHorizontal-viewportVertical);
        glVertex(viewportCenter-viewportHorizontal-viewportVertical);
        glVertex(viewportCenter-viewportHorizontal+viewportVertical);
        glVertex(viewportCenter+viewportHorizontal+viewportVertical);
        glEnd();
        glDisable(GL_BLEND);

        // remove mesh transform
        glPopMatrix();
    }

    glPopAttrib();
}

void DecorateBasePlugin::DrawColorHistogram(CHist &ch, GLArea *gla, QPainter *painter, RichParameterSet *par, QFont qf)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    float ratio = float(gla->width())/gla->height();
    glOrtho(0,ratio,0,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    float len = ch.MaxV() - ch.MinV();
    float maxWide = ch.MaxCount();
    float histWide=maxWide;
    if(par->getBool(this->HistFixedParam()))
    {
        histWide = par->getFloat(this->HistFixedWidthParam());
        if(histWide ==0 ) histWide = maxWide;
    }
    float bn = ch.BinNum();

    float border = 0.15;
    float histH = 1.0f - 2.f*border;
    float histW = 0.3f;

    glBegin(GL_QUAD_STRIP);
    for(float i =0;i<bn;++i)
    {
        float val = ch.MinV() + (i/bn)*(ch.MaxV() - ch.MinV());
        float wide = histW *float(ch.BinCount(val))/histWide;
        wide= std::min(0.5f,wide);
        float ypos  = ( i   /bn)*histH;
        float ypos2 = ((i+1)/bn)*histH;

        glColor(ch.BinColorAvg(val));
        glVertex3f(border,    border+ypos,0);
        glVertex3f(border+wide, border+ypos,0);
        glVertex3f(border,   border+ypos2,0);
        glVertex3f(border+wide,border+ypos2,0);
    }

    glEnd();

    glColor(textColor);
    drawQuotedLine(Point3d(border*4/5.0,border,0),Point3d(border*4/5.0,1.0-border,0),ch.MinV(),ch.MaxV(),len/20.0,painter,qf,0,true);
    glLabel::render(painter,Point3f(border,1-border*0.5,0),QString("MinV %1 MaxV %2 MaxC %3").arg(ch.MinElem()).arg(ch.MaxElem()).arg(maxWide),glLabel::Mode(textColor));
    // Closing 2D
    glPopAttrib();
    glPopMatrix(); // restore modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// This function performs the Scale/Translation transform
// that is needed to correctly draw a single texture.
// When more than a single texture is used they are stacked vertically
void DecorateBasePlugin::PlaceTexParam(int /*TexInd*/, int /*TexNum*/)
{

}


void DecorateBasePlugin::DrawTexParam(MeshModel &m, GLArea *gla, QPainter *painter,  RichParameterSet *rm, QFont qf)
{
    if(!m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)) return;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    float ratio = float(gla->width())/gla->height();
    glOrtho(-ratio,ratio,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(ratio-1.0,0.0f,0.0f);
    glScalef(0.9f,0.9f,0.9f);

    QString textureName("-- no texture --");
    /*if(!m.glw.TMId.empty())
    textureName = qPrintable(QString(m.cm.textures[0].c_str()))+QString("  ");*/
    glLabel::render(painter,Point3f(0.0,-0.10,0.0),textureName,glLabel::Mode(textColor));
    checkGLError::debugInfo("DrawTexParam");
    drawQuotedLine(Point3d(0,0,0),Point3d(0,1,0),0,1,0.1,painter,qf,0,true);
    drawQuotedLine(Point3d(0,0,0),Point3d(1,0,0),0,1,0.1,painter,qf,90.0f);


    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if( rm->getBool(this->TextureStyleParam()) )
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	bool faceColor = rm->getBool(this->TextureFaceColorParam());
	if (faceColor && !m.hasDataMask(MeshModel::MM_FACECOLOR))
	{
		this->RealTimeLog("Show UV Tex Param","The model has no face color", "The model has no Face Color");
		faceColor = false;
	}

	
    /*if(!m.glw.TMId.empty())
    {
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, m.glw.TMId.back() );
    }*/

    glBegin(GL_TRIANGLES);
    for(size_t i=0;i<m.cm.face.size();++i)
        if(!m.cm.face[i].IsD())
        {
            if(faceColor) glColor(m.cm.face[i].C());
            glTexCoord(m.cm.face[i].WT(0).P());
            glVertex(m.cm.face[i].WT(0).P());
            glTexCoord(m.cm.face[i].WT(1).P());
            glVertex(m.cm.face[i].WT(1).P());
            glTexCoord(m.cm.face[i].WT(2).P());
            glVertex(m.cm.face[i].WT(2).P());
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);

        // Closing 2D
        glPopAttrib();

        glPopMatrix(); // restore modelview
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

}

void DecorateBasePlugin::initGlobalParameterSet(QAction *action, RichParameterSet &parset)
{

    switch(ID(action)){
    case DP_SHOW_BOX_CORNERS :
        {
            parset.addParam(new RichBool(this->BBAbsParam(), false, "Draw Untrasformed","If true the bbox is drawn in the original, untrasformed position "
                "(instead of the position obtained by transforming it using the matrix associated to the current Layer)"));
        }    break;
    case DP_SHOW_TEXPARAM : {
        assert(!parset.hasParameter(TextureStyleParam()));
        parset.addParam(new RichBool(TextureStyleParam(), true,"Texture Param Wire","if true the parametrization is drawn in a textured wireframe style"));
        parset.addParam(new RichBool(TextureFaceColorParam(), false,"Face Color","if true the parametrization is drawn with a per face color (useful if you want display per face parametrization distortion)"));
                            } break;
    case DP_SHOW_LABEL :
        {
            parset.addParam(new RichBool(LabelVertFlag(),true,"Per Vertex",""));
            parset.addParam(new RichBool(LabelEdgeFlag(),true,"Per Edge",""));
            parset.addParam(new RichBool(LabelFaceFlag(),true,"Per Face",""));
        } break;
    case DP_SHOW_NORMALS : {
        parset.addParam(new RichFloat(NormalLength(),0.05,"Vector Length","The length of the normal expressed as a percentage of the bbox of the mesh"));
        parset.addParam(new RichBool(NormalVertFlag(),true,"Per Vertex",""));
        parset.addParam(new RichBool(NormalFaceFlag(),true,"Per Face",""));
                           } break;
    case DP_SHOW_CURVATURE : {
        parset.addParam(new RichFloat(CurvatureLength(),0.05,"Vector Length","The length of the normal expressed as a percentage of the bbox of the mesh"));
        parset.addParam(new RichBool(ShowPerVertexCurvature(),true,"Per Vertex",""));
        parset.addParam(new RichBool(ShowPerFaceCurvature(),true,"Per Face",""));
                             } break;
    case DP_SHOW_QUALITY_HISTOGRAM :
        {
            parset.addParam(new RichEnum(HistTypeParam(),0,QStringList()<<"Per Vertex"<<"Per Face","Quality Src","Set the source of the quality, it can be either per vertex or per face."));
            parset.addParam(new RichInt(HistBinNumParam(), 256,"Histogram Bins","If true the parametrization is drawn in a textured wireframe style"));
            parset.addParam(new RichBool(HistAreaParam(), false,"Area Weighted","If true the histogram is computed according to the surface of the involved elements.<br>"
                "e.g. each face contribute to the histogram proportionally to its area and each vertex with 1/3 of sum of the areas of the incident triangles."));
            parset.addParam(new RichBool(HistFixedParam(), false,"Fixed Width","if true the parametrization is drawn in a textured wireframe style"));
            parset.addParam(new RichFloat(HistFixedMinParam(), 0,"Min Hist Value","Used only if the Fixed Histogram Width Parameter is checked"));
            parset.addParam(new RichFloat(HistFixedMaxParam(), 0,"Max Hist Value","Used only if the Fixed Histogram Width Parameter is checked"));
            parset.addParam(new RichFloat(HistFixedWidthParam(), 0,"Hist Width","If not zero, this value is used to scale histogram width  so that it is the indicated value.<br>"
                "Useful only if you have to compare multiple histograms.<br>"
                "Warning, with wrong values the histogram can become excessively flat or it can overflow"));
        } break;

    case DP_SHOW_CAMERA :
        {
            QStringList methods; methods << "Trackball" << "Mesh Camera" << "Raster Camera";
            QStringList scale; scale << "No Scale" << "Fixed Factor";
            parset.addParam(new RichEnum(this->CameraScaleParam(), 0, scale,"Camera Scale Method","Change rendering scale for better visibility in the scene"));
            parset.addParam(new RichFloat(this->FixedScaleParam(), 5.0,"Scale Factor","Draw scale. Used only if the Fixed Factor scaling is chosen"));
            parset.addParam(new RichBool(this->ShowMeshCameras(), false, "Show Mesh Cameras","if true, valid cameras are shown for all visible mesh layers"));
            parset.addParam(new RichBool(this->ShowRasterCameras(), true, "Show Raster Cameras","if true, valid cameras are shown for all visible raster layers"));
            parset.addParam(new RichBool(this->ShowCameraDetails(), false, "Show Current Camera Details","if true, prints on screen all intrinsics and extrinsics parameters for current camera"));
        } break;
    case DP_SHOW_QUALITY_CONTOUR :
        {
            parset.addParam(new RichFloat       (this->ShowContourFreq(), 20, "Number of Contours","The number of contours that are drawn between min and max of the quality values."));
            parset.addParam(new RichDynamicFloat(this->ShowContourWidth(), 0.5f,0.0f,1.0f, "Width","Relative width of the contours; in the 0..1 range."));
            parset.addParam(new RichDynamicFloat(this->ShowContourAlpha(), 0.5f,0.0f,1.0f, "Alpha of Contours","Transparency of che contours that are overdrawn over the mesh."));
            parset.addParam(new RichBool(this->ShowContourRamp(), true, "Ramp Contour","If enabled show a ramp that gives you info about the gradient of the quality field (transparent to opaque means increasing values) "));
        } break;
    case DP_SHOW_SELECTED_MESH :
        {
            parset.addParam(new RichColor(selectedMeshBlendingColor(),QColor(255, 178,0, 50),QString("Curr Mesh Blend Color"),QString("Current Mesh Blending Color")));
        } break;
    }
}


MESHLAB_PLUGIN_NAME_EXPORTER(DecorateBasePlugin)
