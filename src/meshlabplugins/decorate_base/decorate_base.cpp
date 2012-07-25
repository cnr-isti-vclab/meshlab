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

using namespace vcg;
using namespace std;

typedef pair<Point3f,Color4b> PointPC; // this type is used to have a simple coord+color pair to rapidly draw non manifold faces

QString ExtraMeshDecoratePlugin::decorationInfo(FilterIDType filter) const
 {
  switch(filter)
  {
    case DP_SHOW_AXIS :									return tr("Draws XYZ axes in world coordinates");
    case DP_SHOW_BOX_CORNERS:						return tr("Draws object's bounding box corners");
    case DP_SHOW_VERT:									return tr("Draw the vertices of the mesh as round dots");
    case DP_SHOW_NON_FAUX_EDGE:				return tr("Draws the edge of the mesh that are tagged as 'real edges' (useful for quadmeshes).");
    case DP_SHOW_BOUNDARY:								return tr("Draws the edge of the mesh that are on the boundary.");
    case DP_SHOW_BOUNDARY_TEX:					return tr("Draws the edge where there is a texture seam.");
    case DP_SHOW_NON_MANIF_EDGE:					return tr("Draws the non manifold edges of the current mesh");
    case DP_SHOW_NON_MANIF_VERT:					return tr("Draws the non manifold vertices of the current mesh");
    case DP_SHOW_BOX_CORNERS_ABS  :			return tr("Show Box Corners (Abs)");
    case DP_SHOW_VERT_NORMALS:					return tr("Draws object vertex normals");
    case DP_SHOW_VERT_PRINC_CURV_DIR :	return tr("Show Vertex Principal Curvature Directions");
    case DP_SHOW_FACE_NORMALS:					return tr("Draws object face normals");
    case DP_SHOW_QUOTED_BOX:						return tr("Draws quoted box");
    case DP_SHOW_VERT_LABEL:						return tr("Draws all the vertex indexes<br> Useful for debugging<br>(do not use it on large meshes)");
    case DP_SHOW_EDGE_LABEL:						return tr("Draws all the edge indexes<br> Useful for debugging<br>(do not use it on large meshes)");
    case DP_SHOW_VERT_QUALITY_HISTOGRAM:						return tr("Draws a (colored) Histogram of the per vertex quality");
    case DP_SHOW_FACE_QUALITY_HISTOGRAM:						return tr("Draws a (colored) Histogram of the per face quality");
    case DP_SHOW_FACE_LABEL:						return tr("Draws all the face indexes, <br> Useful for debugging <br>(do not use it on large meshes)");
    case DP_SHOW_CAMERA:								return tr("Draw the position of the camera, if present in the current mesh");
    case DP_SHOW_TEXPARAM: return tr("Draw an overlayed flattened version of the current mesh that show the current parametrization");
	 }
  assert(0);
  return QString();
 }

QString ExtraMeshDecoratePlugin::decorationName(FilterIDType filter) const
{
    switch(filter)
    {
    case DP_SHOW_VERT      :	return QString("Show Vertex Dots");
    case DP_SHOW_NON_FAUX_EDGE :	return QString("Show Non-Faux Edges");
    case DP_SHOW_BOUNDARY :	return QString("Show Boundary Edges");
    case DP_SHOW_BOUNDARY_TEX :	return QString("Show Texture Seams");
    case DP_SHOW_NON_MANIF_EDGE :	return QString("Show Non Manif Edges");
    case DP_SHOW_NON_MANIF_VERT:	return QString("Show Non Manif Vertices");
    case DP_SHOW_VERT_NORMALS      :	return QString("Show Vertex Normals");
    case DP_SHOW_VERT_PRINC_CURV_DIR :	return QString("Show Vertex Principal Curvature Directions");
    case DP_SHOW_FACE_NORMALS      :	return QString("Show Face Normals");
    case DP_SHOW_BOX_CORNERS  :			return QString("Show Box Corners");
    case DP_SHOW_BOX_CORNERS_ABS  :		return QString("Show Box Corners (Abs)");
    case DP_SHOW_AXIS         :			return QString("Show Axis");
    case DP_SHOW_QUOTED_BOX		:	return QString("Show Quoted Box");
    case DP_SHOW_VERT_LABEL:		return tr("Show Vertex Label");
    case DP_SHOW_EDGE_LABEL:		return tr("Show Edge Label");
    case DP_SHOW_FACE_LABEL:			return tr("Show Face Label");
    case DP_SHOW_CAMERA:			return tr("Show Camera");
    case DP_SHOW_TEXPARAM:			return tr("Show UV Tex Param");
    case DP_SHOW_VERT_QUALITY_HISTOGRAM:			return tr("Show Vert Quality Histogram");
    case DP_SHOW_FACE_QUALITY_HISTOGRAM:			return tr("Show Face Quality Histogram");

    default: assert(0);
    }
    return QString("error!");
}

void ExtraMeshDecoratePlugin::decorate(QAction *a, MeshDocument &md, RichParameterSet *rm, GLArea *gla, QPainter *painter)
{
  MeshModel &m=*(md.mm());
  QFont qf;
	glPushMatrix();
	glMultMatrix(m.cm.Tr);
    switch (ID(a))
    {
    case DP_SHOW_FACE_NORMALS:
    case DP_SHOW_VERT_NORMALS:
    case DP_SHOW_VERT_PRINC_CURV_DIR:
        {
            glPushAttrib(GL_ENABLE_BIT );
            float NormalLen=rm->getFloat(NormalLength());
            float LineLen = m.cm.bbox.Diag()*NormalLen;
            CMeshO::VertexIterator vi;
            CMeshO::FaceIterator fi;
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_LINES);
            if(ID(a) == DP_SHOW_VERT_NORMALS)
			{
				glColor4f(.4f,.4f,1.f,.6f);
                for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
                {
                    glVertex((*vi).P());
                    glVertex((*vi).P()+(*vi).N()*LineLen);
                }
			}
            else
                if( ID(a) == DP_SHOW_VERT_PRINC_CURV_DIR){
                if(m.hasDataMask(MeshModel::MM_VERTCURVDIR))
                    for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi) if(!(*vi).IsD())
                    {
					glColor4f(1.0,0.0,0.0,.8f);
					glVertex((*vi).P());
					glColor4f(0.0,1.0,0.0,.1f);
					glVertex((*vi).P()+(*vi).PD1()*LineLen*0.25);
					glColor4f(0.0,1.0,0.0,.8f);
					glVertex((*vi).P());
					glColor4f(0.0,1.0,0.0,.1f);
					glVertex((*vi).P()+(*vi).PD2()*LineLen*0.25);
				}
            }
            else
                if(ID(a) == DP_SHOW_FACE_NORMALS)
                {
                glColor4f(.1f,.4f,4.f,.6f);
                for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
                {
                    Point3f b=Barycenter(*fi);
                    glVertex(b);
                    glVertex(b+(*fi).N()*LineLen);
                }
            }
			
            glEnd();
            glPopAttrib();
        } break;
    case DP_SHOW_BOX_CORNERS:
      {
        DrawBBoxCorner(m);
        this->RealTimeLog("Bounding Box","<table>"
                          "<tr><td>Min: </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
                          "<tr><td>Max: </td><td width=70 align=right>%7.4f</td><td width=70 align=right> %7.4f</td><td width=70 align=right> %7.4f</td></tr>"
                          "</table>",m.cm.bbox.min[0],m.cm.bbox.min[1],m.cm.bbox.min[2],m.cm.bbox.max[0],m.cm.bbox.max[1],m.cm.bbox.max[2]);
      }
      break;
    case DP_SHOW_CAMERA:
      {
        // draw all mesh cameras
       bool showCameraDetails = rm->getBool(ShowCameraDetails());
        if(rm->getBool(ShowMeshCameras()))
        {
          foreach(MeshModel *meshm,  md.meshList)
          {
            if(meshm != md.mm() || (!showCameraDetails) )   // non-selected meshes
             DrawCamera(meshm, meshm->cm.shot, Color4b::DarkRed, md.mm()->cm.Tr, rm, painter,qf);
            else                          // selected mesh, draw & display data
            {
              DrawCamera(meshm, meshm->cm.shot, Color4b::Magenta, md.mm()->cm.Tr, rm, painter,qf);
              DisplayCamera(meshm, meshm->cm.shot, 1, painter, qf);
            }
          }
        }

        // draw all visible raster cameras
        // current camera also.
        if(rm->getBool(ShowRasterCameras()))
        {
          foreach(RasterModel *raster, md.rasterList)
          if(raster->visible)
            {
              if(raster != md.rm() || !showCameraDetails )   // non-selected raster
              {
                if(raster->visible) DrawCamera(NULL, raster->shot, Color4b::DarkBlue, md.mm()->cm.Tr, rm, painter,qf);
              }
              else
              {
                DrawCamera(NULL, raster->shot, Color4b::Cyan, md.mm()->cm.Tr, rm, painter,qf);
                DisplayCamera(md.mm(), raster->shot, 2, painter, qf);
              }
            }
        }


      }break;
    case DP_SHOW_QUOTED_BOX:		DrawQuotedBox(m,painter,qf);break;
    case DP_SHOW_VERT_LABEL:	DrawVertLabel(m,painter);break;
    case DP_SHOW_EDGE_LABEL:	DrawEdgeLabel(m,painter);break;
    case DP_SHOW_FACE_LABEL:	DrawFaceLabel(m,painter);break;
    case DP_SHOW_VERT:	{
			glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
			glDisable(GL_LIGHTING);
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor(Color4b::Black);
			glDepthRange (0.0, 0.9999);
			glDepthFunc(GL_LEQUAL);
      float baseSize = rm->getDynamicFloat(this->VertDotSizeParam());

			glPointSize(baseSize+0.5);
			m.glw.DrawPointsBase<GLW::NMNone,GLW::CMNone>();
			glColor(Color4b::White);
			glPointSize(baseSize-1);
			m.glw.DrawPointsBase<GLW::NMNone,GLW::CMNone>();			
			glPopAttrib();
        } break;

    case DP_SHOW_NON_FAUX_EDGE :	{
			glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
			glDisable(GL_LIGHTING);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glLineWidth(1.f);
      Color4b cc=Color4b::DarkGray;
      cc[3]=128;
      glColor(cc);
			glDepthRange (0.0, 0.999);
			m.glw.DrawWirePolygonal<GLW::NMNone,GLW::CMNone>();
			glPopAttrib();

      CMeshO::PerMeshAttributeHandle< vector<PointPC> > vvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<PointPC> >(m.cm,"ExtraordinaryVertexVector");
      if(rm->getBool(this->ShowNonRegular()) && vcg::tri::Allocator<CMeshO>::IsValidHandle (m.cm, vvH))
      {
        RealTimeLog("Faux Edges","%i non regurlar vertices",vvH().size());
        vector<PointPC> *vvP = &vvH();
        glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthRange (0.0, 0.999);
		
		if (vvP->size() != 0)
		{
			glEnableClientState (GL_VERTEX_ARRAY);
			glEnableClientState (GL_COLOR_ARRAY);

			glEnable(GL_POINT_SMOOTH);
			glPointSize(6.f);
			glVertexPointer(3,GL_FLOAT,sizeof(PointPC),&(vvP->begin()[0].first));
			glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(vvP->begin()[0].second));
	
//        glDrawArrays(GL_POINTS,0,vvP->size());
			glDrawArrays(GL_TRIANGLES,0,vvP->size());
			glDisableClientState (GL_COLOR_ARRAY);
			glDisableClientState (GL_VERTEX_ARRAY);
		}
        glPopAttrib();
      }
      CMeshO::PerMeshAttributeHandle< vector<PointPC> > sgH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<PointPC> >(m.cm,"SeparatrixGraph");
      if(rm->getBool(this->ShowSeparatrix()) && vcg::tri::Allocator<CMeshO>::IsValidHandle (m.cm, sgH))
      {
        vector<PointPC> *vvP = &sgH();
        glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(2.f);
        glDepthRange (0.0, 0.999);
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_COLOR_ARRAY);

        glEnable(GL_POINT_SMOOTH);
        glPointSize(6.f);
        glVertexPointer(3,GL_FLOAT,sizeof(PointPC),&(vvP->begin()[0].first));
        glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(vvP->begin()[0].second));
        glDrawArrays(GL_LINES,0,vvP->size());
        glDisableClientState (GL_COLOR_ARRAY);
        glDisableClientState (GL_VERTEX_ARRAY);
        glPopAttrib();
      }
    } break;
    case DP_SHOW_TEXPARAM : this->DrawTexParam(m,gla,painter,rm,qf); break;

    case DP_SHOW_VERT_QUALITY_HISTOGRAM :
      {
        CMeshO::PerMeshAttributeHandle<CHist > qH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<CHist>(m.cm,"VertQualityHist");
        if(vcg::tri::Allocator<CMeshO>::IsValidHandle (m.cm, qH)) {
          CHist &ch=qH();
          this->DrawColorHistogram(ch,gla, painter,rm,qf);
        }
      }
      break;
    case DP_SHOW_FACE_QUALITY_HISTOGRAM :
      {
        CMeshO::PerMeshAttributeHandle<CHist > qH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<CHist>(m.cm,"FaceQualityHist");
        if(vcg::tri::Allocator<CMeshO>::IsValidHandle (m.cm, qH)) {
          CHist &ch=qH();
	        this->DrawColorHistogram(ch,gla, painter,rm,qf);
	        }
      }
      break;
    case DP_SHOW_NON_MANIF_VERT :
    {
      // Note the standard way for adding extra per-mesh data using the per-mesh attributes.
      CMeshO::PerMeshAttributeHandle< vector<PointPC> > vvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<PointPC> >(m.cm,"NonManifVertVertVector");
      CMeshO::PerMeshAttributeHandle< vector<PointPC> > tvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<PointPC> >(m.cm,"NonManifVertTriVector");
      if(vcg::tri::Allocator<CMeshO>::IsValidHandle (m.cm, vvH))
      {
        vector<PointPC> *vvP = &vvH();
        vector<PointPC> *tvP = &tvH();
        glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(1.f);
        glDepthRange (0.0, 0.999);
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_COLOR_ARRAY);

        glEnable(GL_POINT_SMOOTH);
        if (vvP->size() > 0)
		{
			glPointSize(6.f);
			glVertexPointer(3,GL_FLOAT,sizeof(PointPC),&(vvP->begin()[0].first));
			glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(vvP->begin()[0].second));
			glDrawArrays(GL_POINTS,0,vvP->size());
		}

		if (tvP->size() > 0)
		{
			glVertexPointer(3,GL_FLOAT,sizeof(PointPC),&(tvP->begin()[0].first));
			glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(tvP->begin()[0].second));
			glDrawArrays(GL_TRIANGLES,0,tvP->size());
		}
		this->RealTimeLog("Non Manifold Vertices",
						  "<b>%i</b> non manifold vertices<br> "
						  "<b>%i</b> faces over non manifold vertices",vvP->size(),tvP->size()/3);

        glDisableClientState (GL_COLOR_ARRAY);
        glDisableClientState (GL_VERTEX_ARRAY);
        glPopAttrib();
      }
    }break;
    case DP_SHOW_NON_MANIF_EDGE :
    {
      // Note the standard way for adding extra per-mesh data using the per-mesh attributes.
      CMeshO::PerMeshAttributeHandle< vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<PointPC> >(m.cm,"NonManifEdgeVector");
      CMeshO::PerMeshAttributeHandle< vector<PointPC> > fvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<PointPC> >(m.cm,"NonManifFaceVector");
      if(vcg::tri::Allocator<CMeshO>::IsValidHandle (m.cm, bvH))
      {
        vector<PointPC> *BVp = &bvH();
        vector<PointPC> *FVp = &fvH();
        if (BVp->size() != 0)
        {
          glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
          glDisable(GL_LIGHTING);
          glDepthFunc(GL_LEQUAL);
          glEnable(GL_LINE_SMOOTH);
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glLineWidth(1.f);
          glDepthRange (0.0, 0.999);
          glEnableClientState (GL_VERTEX_ARRAY);
          glEnableClientState (GL_COLOR_ARRAY);

          glVertexPointer(3,GL_FLOAT,sizeof(PointPC),&(BVp->begin()[0].first));
          glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(BVp->begin()[0].second));
          glDrawArrays(GL_LINES,0,BVp->size());

          glVertexPointer(3,GL_FLOAT,sizeof(PointPC),&(FVp->begin()[0].first));
          glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(FVp->begin()[0].second));
          glDrawArrays(GL_TRIANGLES,0,FVp->size());

          glDisableClientState (GL_COLOR_ARRAY);
          glDisableClientState (GL_VERTEX_ARRAY);
          glPopAttrib();
        }
        this->RealTimeLog("Non Manifold Edges",
                          " <b>%i</b> non manifold edges<br>"
                          " <b>%i</b> faces over non manifold edges",BVp->size()/2,FVp->size()/3);
      }
    } break;
    case DP_SHOW_BOUNDARY :
    {
      bool showBorderFlag = rm->getBool(ShowBorderFlag());
      // Note the standard way for adding extra per-mesh data using the per-mesh attributes.
      CMeshO::PerMeshAttributeHandle< vector<Point3f> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<Point3f> >(m.cm,"BoundaryVertVector");
      CMeshO::PerMeshAttributeHandle< vector<Point3f> > bfH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<Point3f> >(m.cm,"BoundaryFaceVector");
      if(vcg::tri::Allocator<CMeshO>::IsValidHandle (m.cm, bvH))
      {
        vector<Point3f> *BVp = &bvH();
        vector<Point3f> *BFp = &bfH();
        if (BVp->size() != 0)
        {
          glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
          glDisable(GL_LIGHTING);
          glDepthFunc(GL_LEQUAL);
          glEnable(GL_LINE_SMOOTH);
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glLineWidth(1.f);
          glColor(Color4b::Green);
          glDepthRange (0.0, 0.999);
          glEnableClientState (GL_VERTEX_ARRAY);
          glVertexPointer(3,GL_FLOAT,sizeof(Point3f),&(BVp->begin()[0]));
          glDrawArrays(GL_LINES,0,BVp->size());

          glColor4f(0.0f,1.0f,0.0f,0.5f);
          if(showBorderFlag)
          {
            glVertexPointer(3,GL_FLOAT,sizeof(Point3f),&(BFp->begin()[0]));
            glDrawArrays(GL_TRIANGLES,0,BFp->size());
          }
          glDisableClientState (GL_VERTEX_ARRAY);
          glPopAttrib();
        }
        this->RealTimeLog("Boundary","<b>%i</b> boundary edges",BVp->size()/2);
      }
    } break;
    case DP_SHOW_BOUNDARY_TEX :
    {
      // Note the standard way for adding extra per-mesh data using the per-mesh attributes.
      CMeshO::PerMeshAttributeHandle< vector<Point3f> > btvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vector<Point3f> >(m.cm,"BoundaryTexVector");
      if(vcg::tri::Allocator<CMeshO>::IsValidHandle (m.cm, btvH))
      {
        vector<Point3f> *BTVp = &btvH();
        if (BTVp->size() != 0)
        {
          glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT|	  GL_CURRENT_BIT |  GL_DEPTH_BUFFER_BIT);
          glDisable(GL_LIGHTING);
          glDisable(GL_TEXTURE_2D);
          glDepthFunc(GL_LEQUAL);
          glEnable(GL_LINE_SMOOTH);
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glLineWidth(1.f);
          glColor(Color4b::Green);
          glDepthRange (0.0, 0.999);
          glEnableClientState (GL_VERTEX_ARRAY);
          glVertexPointer(3,GL_FLOAT,sizeof(Point3f),&(BTVp->begin()[0]));
          glDrawArrays(GL_LINES,0,BTVp->size());
          glDisableClientState (GL_VERTEX_ARRAY);
          glPopAttrib();
        }
      }
    } break;
    } // end switch;
	glPopMatrix();

  if(ID(a) == DP_SHOW_AXIS)	CoordinateFrame(m.cm.bbox.Diag()/2.0).Render(gla,painter);
  if(ID(a) == DP_SHOW_BOX_CORNERS_ABS)	DrawBBoxCorner(m,false);
}

void ExtraMeshDecoratePlugin::DrawQuotedBox(MeshModel &m,QPainter *gla,QFont qf)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	// Get gl state values
	double mm[16],mp[16];
	GLint vp[4];
	glGetDoublev(GL_PROJECTION_MATRIX,mp);
	glGetDoublev(GL_MODELVIEW_MATRIX,mm);
	glGetIntegerv(GL_VIEWPORT,vp);

	// Mesh boundingBox
	Box3f b(m.cm.bbox);
	glColor(Color4b::LightGray);
	glBoxWire(b);

	glLineWidth(1.f);
	glPointSize(3.f);

	Point3d p1,p2;

	Point3f c = b.Center();
		
	float s = 1.15f;
  const float LabelSpacing = 30;
	chooseX(b,mm,mp,vp,p1,p2);					// Selects x axis candidate
	glPushMatrix();
	glScalef(1,s,s);
	glTranslatef(0,c[1]/s-c[1],c[2]/s-c[2]);
	drawQuotedLine(p1,p2,b.min[0],b.max[0],CoordinateFrame::calcSlope(p1,p2,b.DimX(),LabelSpacing,mm,mp,vp),gla,qf);	// Draws x axis
	glPopMatrix();

	chooseY(b,mm,mp,vp,p1,p2);					// Selects y axis candidate
	glPushMatrix();
	glScalef(s,1,s);
	glTranslatef(c[0]/s-c[0],0,c[2]/s-c[2]);
	drawQuotedLine(p1,p2,b.min[1],b.max[1],CoordinateFrame::calcSlope(p1,p2,b.DimY(),LabelSpacing,mm,mp,vp),gla,qf);	// Draws y axis
	glPopMatrix();

	chooseZ(b,mm,mp,vp,p1,p2);					// Selects z axis candidate	
	glPushMatrix();
	glScalef(s,s,1);
	glTranslatef(c[0]/s-c[0],c[1]/s-c[1],0);
	drawQuotedLine(p1,p2,b.min[2],b.max[2],CoordinateFrame::calcSlope(p1,p2,b.DimZ(),LabelSpacing,mm,mp,vp),gla,qf);	// Draws z axis
	glPopMatrix();

	glPopAttrib();

}

void ExtraMeshDecoratePlugin::chooseX(Box3f &box,double *mm,double *mp,GLint *vp,Point3d &x1,Point3d &x2)
{
	float d = -std::numeric_limits<float>::max();
	Point3d c;
	// Project the bbox center
	gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
	c[2] = 0;

	Point3d out1,out2;
	Point3f in1,in2;

	for (int i=0;i<8;i+=2)
	{
		// find the furthest axis
		in1 = box.P(i);
		in2 = box.P(i+1);

		gluProject((double)in1[0],(double)in1[1],(double)in1[2],mm,mp,vp,&out1[0],&out1[1],&out1[2]);
		gluProject((double)in2[0],(double)in2[1],(double)in2[2],mm,mp,vp,&out2[0],&out2[1],&out2[2]);
		out1[2] = out2[2] = 0;

		float currDist = Distance(c,(out1+out2)*.5f);

		if(currDist > d)
		{
			d = currDist;
			x1.Import(in1);
			x2.Import(in2);
		}
	}
}


void ExtraMeshDecoratePlugin::chooseY(Box3f &box,double *mm,double *mp,GLint *vp,Point3d &y1,Point3d &y2)
{
	float d = -std::numeric_limits<float>::max();
	Point3d c;
	// Project the bbox center
	gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
	c[2] = 0;

	Point3d out1,out2;
	Point3f in1,in2;

	for (int i=0;i<6;++i)
	{
		if(i==2) i = 4;	// skip
		// find the furthest axis
		in1 = box.P(i);
		in2 = box.P(i+2);

		gluProject((double)in1[0],(double)in1[1],(double)in1[2],mm,mp,vp,&out1[0],&out1[1],&out1[2]);
		gluProject((double)in2[0],(double)in2[1],(double)in2[2],mm,mp,vp,&out2[0],&out2[1],&out2[2]);
		out1[2] = out2[2] = 0;

		float currDist = Distance(c,(out1+out2)*.5f);

		if(currDist > d)
		{
			d = currDist;
			y1.Import(in1);
			y2.Import(in2);
		}
	}
}

void ExtraMeshDecoratePlugin::chooseZ(Box3f &box,double *mm,double *mp,GLint *vp,Point3d &z1,Point3d &z2)
{
	float d = -std::numeric_limits<float>::max();
	Point3d c;
	// Project the bbox center
	gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
	c[2] = 0;

	Point3d out1,out2;
	Point3f in1,in2;

	Point3d m;

	for (int i=0;i<4;++i)
	{
		// find the furthest axis
		in1 = box.P(i);
		in2 = box.P(i+4);


		gluProject((double)in1[0],(double)in1[1],(double)in1[2],mm,mp,vp,&out1[0],&out1[1],&out1[2]);
		gluProject((double)in2[0],(double)in2[1],(double)in2[2],mm,mp,vp,&out2[0],&out2[1],&out2[2]);
		out1[2] = out2[2] = 0;

		float currDist = Distance(c,(out1+out2)*.5f);

		if(currDist > d)
		{
			d = currDist;
			z1.Import(in1);
			z2.Import(in2);
		}
	}
}
/**
  Draw a line with labeled ticks.
  \param a,b the two endpoints of the line (in 3D)
  \param aVal,bVal the two values at the line endpoints.
  \param tickScalarDistance the distance between labeled ticks ( 1/10 small ticks are added between them).

  for example if you want the a quoted line between 0.363 and 1.567 tickScalarDistance == 0.1 it means that
  you will have a line with labeled ticks at 0.4 0.5 etc.
 */



void ExtraMeshDecoratePlugin::drawQuotedLine(const Point3d &a,const Point3d &b, float aVal, float bVal, float tickScalarDistance, QPainter *painter, QFont qf,float angle,bool rightAlign)
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
  glLabel::Mode md(qf,Color4b::White,angle,rightAlign);
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
            glLabel::render(painter,Point3f::Construct(Zero+v*i),tr("%1 ").arg(i,4+neededZeros,'f',neededZeros),md);
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
  glLabel::render(painter,Point3f::Construct(a), tr("%1").arg(aVal,6+neededZeros,'f',neededZeros+2) ,md);
  glLabel::render(painter,Point3f::Construct(b), tr("%1").arg(bVal,6+neededZeros,'f',neededZeros+2) ,md);

  glPopAttrib();
}


float ExtraMeshDecoratePlugin::niceRound2(float Val,float base)	{return powf(base,ceil(log10(Val)/log10(base)));}
float ExtraMeshDecoratePlugin::niceRound(float val)	{return powf(10.f,ceil(log10(val)));}

void ExtraMeshDecoratePlugin::DrawBBoxCorner(MeshModel &m, bool absBBoxFlag)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0);
	glColor(Color4b::Cyan);
	Box3f b;
	if(absBBoxFlag) {
			b=m.cm.bbox;
			glColor(Color4b::Cyan);
	} else {
			b=m.cm.trBB();
			glColor(Color4b::Green);
	}
	Point3f mi=b.min;
	Point3f ma=b.max;
	Point3f d3=(b.max-b.min)/4.0;
	Point3f zz(0,0,0);
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
	glPopAttrib();
}


bool ExtraMeshDecoratePlugin::isDecorationApplicable(QAction *action, const MeshModel& m, QString &ErrorMessage) const
{
  if( ID(action) == DP_SHOW_VERT_LABEL || ID(action) == DP_SHOW_FACE_LABEL)
        {
          if(m.cm.vn <1000 && m.cm.fn<2000) return true;
          else {
            ErrorMessage=QString("Warning: the mesh contains many faces and vertices.<br>Printing on the screen thousand of numbers is useless and VERY SLOW <br> Do you REALLY want this? ");
            return false;
          }
        }
  if(ID(action) == DP_SHOW_FACE_QUALITY_HISTOGRAM ) return m.hasDataMask(MeshModel::MM_FACEQUALITY);
  if(ID(action) == DP_SHOW_VERT_QUALITY_HISTOGRAM ) return m.hasDataMask(MeshModel::MM_VERTQUALITY);

  if( ID(action) == DP_SHOW_VERT_PRINC_CURV_DIR )
  {
    if(!m.hasDataMask(MeshModel::MM_VERTCURVDIR)) return false;
  }
    if( ID(action) == DP_SHOW_TEXPARAM || ID(action) == DP_SHOW_BOUNDARY_TEX)
    {
        if(!m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)) return false;
    }

    if( ID(action) == DP_SHOW_NON_MANIF_EDGE ) if(m.cm.fn==0) return false;
    if( ID(action) == DP_SHOW_BOUNDARY ) if(m.cm.fn==0) return false;

  return true;
}


bool ExtraMeshDecoratePlugin::startDecorate(QAction * action, MeshDocument &md, RichParameterSet *rm, GLArea *gla)
{	
  switch(ID(action))
  {
  case DP_SHOW_NON_FAUX_EDGE :
  {
    MeshModel *m=md.mm();
    CMeshO::PerMeshAttributeHandle< vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<PointPC> >(m->cm,"ExtraordinaryVertexVector");
    if(!vcg::tri::Allocator<CMeshO>::IsValidHandle(m->cm,bvH))
      bvH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute< vector<PointPC> >(m->cm,std::string("ExtraordinaryVertexVector"));
    vector<PointPC> *BVp = &bvH();
    BVp->clear();

    SimpleTempData<CMeshO::VertContainer, int > ValencyCounter(m->cm.vert,0);

    for(CMeshO::FaceIterator fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
    {
      for(int i=0;i<3;++i)
        if(!(*fi).IsF(i))
        {
          ++ ValencyCounter[(*fi).V0(i)];
          ++ ValencyCounter[(*fi).V1(i)];
        }
    }
    Color4b bCol2=Color4b(255,0,0,0);
    Color4b vCol2=Color4b(255,0,0,192);
    Color4b bCol3=Color4b(255,0,0,0);
    Color4b vCol3=Color4b(255,0,0,128);
    Color4b bCol5=Color4b(0,0,255,0);
    Color4b vCol5=Color4b(0,0,255,128);
    Color4b bCol6=Color4b(0,0,255,0);
    Color4b vCol6=Color4b(0,0,255,192);

    for(CMeshO::FaceIterator fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
    {
      for(int i=0;i<3;++i)
      {
        if(ValencyCounter[(*fi).V(i)]<6) {
          BVp->push_back(make_pair(((*fi).V2(i)->P()+(*fi).V(i)->P())/2.0f,bCol2));
          BVp->push_back(make_pair( (*fi).V(i)->P()                       ,vCol2));
          BVp->push_back(make_pair(((*fi).V1(i)->P()+(*fi).V(i)->P())/2.0f,bCol2));
        }
        if(ValencyCounter[(*fi).V(i)]==6) {
          BVp->push_back(make_pair(((*fi).V2(i)->P()+(*fi).V(i)->P())/2.0f,bCol3));
          BVp->push_back(make_pair( (*fi).V(i)->P()                       ,vCol3));
          BVp->push_back(make_pair(((*fi).V1(i)->P()+(*fi).V(i)->P())/2.0f,bCol3));
        }
        if(ValencyCounter[(*fi).V(i)]==10) {
          BVp->push_back(make_pair(((*fi).V2(i)->P()+(*fi).V(i)->P())/2.0f,bCol5));
          BVp->push_back(make_pair( (*fi).V(i)->P()                       ,vCol5));
          BVp->push_back(make_pair(((*fi).V1(i)->P()+(*fi).V(i)->P())/2.0f,bCol5));
        }
        if(ValencyCounter[(*fi).V(i)]>10) {
          BVp->push_back(make_pair(((*fi).V2(i)->P()+(*fi).V(i)->P())/2.0f,bCol6));
          BVp->push_back(make_pair( (*fi).V(i)->P()                       ,vCol6));
          BVp->push_back(make_pair(((*fi).V1(i)->P()+(*fi).V(i)->P())/2.0f,bCol6));
        }
      }
    }
    int val3cnt=0,val5cnt=0;
    for(CMeshO::VertexIterator vi = m->cm.vert.begin(); vi!= m->cm.vert.end();++vi) if(!(*vi).IsD())
    {
      if(ValencyCounter[(*vi)]==6) ++val3cnt;
      if(ValencyCounter[(*vi)]==10) ++val5cnt;
    }
    qDebug("Found %i vertices with valence 3",val3cnt);
    qDebug("Found %i vertices with valence 5",val5cnt);

  } break;


  case DP_SHOW_BOUNDARY :
  {
    MeshModel *m=md.mm();
    CMeshO::PerMeshAttributeHandle< vector<Point3f> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<Point3f> >(m->cm,"BoundaryVertVector");
    CMeshO::PerMeshAttributeHandle< vector<Point3f> > bfH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<Point3f> >(m->cm,"BoundaryFaceVector");
    if(!vcg::tri::Allocator<CMeshO>::IsValidHandle(m->cm,bvH)){
      bvH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute< vector<Point3f> >(m->cm,std::string("BoundaryVertVector"));
      bfH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute< vector<Point3f> >(m->cm,std::string("BoundaryFaceVector"));
    }
    vector<Point3f> *BVp = &bvH();
    vector<Point3f> *BFp = &bfH();
    BVp->clear();
    BFp->clear();
    tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m->cm);
    for(CMeshO::FaceIterator fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
    {
      bool isB=false;
      for(int i=0;i<3;++i)
        if((*fi).IsB(i))
        {
          isB=true;
          BVp->push_back((*fi).V0(i)->P());
          BVp->push_back((*fi).V1(i)->P());
        }
      if(isB)
      {
        BFp->push_back((*fi).V(0)->P());
        BFp->push_back((*fi).V(1)->P());
        BFp->push_back((*fi).V(2)->P());
      }
    }

  } break;
  case DP_SHOW_BOUNDARY_TEX :
  {
    MeshModel *m=md.mm();
    m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    CMeshO::PerMeshAttributeHandle< vector<Point3f> > btvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<Point3f> >(m->cm,"BoundaryTexVector");
    if(!vcg::tri::Allocator<CMeshO>::IsValidHandle(m->cm,btvH))
      btvH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute< vector<Point3f> >(m->cm,std::string("BoundaryTexVector"));
    vector<Point3f> *BTVp = &btvH();
    BTVp->clear();
    vector<std::pair<CMeshO::FacePointer,int> > SaveTopoVec;
    CMeshO::FaceIterator fi;
    for(fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
    {
        for(int i=0;i<3;++i)
            SaveTopoVec.push_back(std::make_pair((*fi).FFp(i),(*fi).FFi(i)));
    }
    tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(m->cm);
    for(fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
    {
      for(int i=0;i<3;++i)
        if(face::IsBorder(*fi,i))
        {
          BTVp->push_back((*fi).V0(i)->P());
          BTVp->push_back((*fi).V1(i)->P());
        }
    }
    vector<std::pair<CMeshO::FacePointer,int> >::iterator iii;
    for(fi = m->cm.face.begin(), iii=SaveTopoVec.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
    {
        for(int i=0;i<3;++i)
        {
            (*fi).FFp(i)= iii->first;
            (*fi).FFi(i)= iii->second;
        }
    }

  } break;
  case DP_SHOW_NON_MANIF_VERT :
  {
    MeshModel *m=md.mm();
    CMeshO::PerMeshAttributeHandle< vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<PointPC> >(m->cm,"NonManifVertVertVector");
    CMeshO::PerMeshAttributeHandle< vector<PointPC> > fvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<PointPC> >(m->cm,"NonManifVertTriVector");
    if(!vcg::tri::Allocator<CMeshO>::IsValidHandle(m->cm,bvH))
    {
      bvH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute< vector<PointPC> >(m->cm,std::string("NonManifVertVertVector"));
      fvH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute< vector<PointPC> >(m->cm,std::string("NonManifVertTriVector"));
    }
    vector<PointPC> *BVp = &bvH();
    vector<PointPC> *FVp = &fvH();
    BVp->clear();
    FVp->clear();
    if(!m->hasDataMask(MeshModel::MM_FACEFACETOPO)) m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    else tri::UpdateTopology<CMeshO>::FaceFace(m->cm);
    tri::SelectionStack<CMeshO> ss(m->cm);
    ss.push();
    tri::UpdateSelection<CMeshO>::VertexClear(m->cm);
    tri::Clean<CMeshO>::CountNonManifoldVertexFF(m->cm,true);
    Color4b bCol=Color4b(255,0,255,0);
    Color4b vCol=Color4b(255,0,255,64);
    tri::UpdateFlags<CMeshO>::VertexClearV(m->cm);
    for(CMeshO::FaceIterator fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
    {
      for(int i=0;i<3;++i)
        {
          if((*fi).V(i)->IsS())
          {
            if(!(*fi).V0(i)->IsV())
            {
               BVp->push_back(make_pair((*fi).V0(i)->P(),Color4b::Magenta));
               (*fi).V0(i)->SetV();
            }

            Point3f P1=((*fi).V0(i)->P()+(*fi).V1(i)->P())/2.0f;
            Point3f P2=((*fi).V0(i)->P()+(*fi).V2(i)->P())/2.0f;
            FVp->push_back(make_pair((*fi).V0(i)->P(),vCol));
            FVp->push_back(make_pair(P1,bCol));
            FVp->push_back(make_pair(P2,bCol));
          }
        }
    }
    ss.pop();
  }break;
  case DP_SHOW_NON_MANIF_EDGE :
  {
    MeshModel *m=md.mm();
    CMeshO::PerMeshAttributeHandle< vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<PointPC> >(m->cm,"NonManifEdgeVector");
    CMeshO::PerMeshAttributeHandle< vector<PointPC> > fvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< vector<PointPC> >(m->cm,"NonManifFaceVector");
    if(!vcg::tri::Allocator<CMeshO>::IsValidHandle(m->cm,bvH))
    {
      bvH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute< vector<PointPC> >(m->cm,std::string("NonManifEdgeVector"));
      fvH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute< vector<PointPC> >(m->cm,std::string("NonManifFaceVector"));
    }
    vector<PointPC> *BVp = &bvH();
    vector<PointPC> *FVp = &fvH();
    BVp->clear();
    FVp->clear();

    if(!m->hasDataMask(MeshModel::MM_FACEFACETOPO)) m->updateDataMask(MeshModel::MM_FACEFACETOPO);
    else tri::UpdateTopology<CMeshO>::FaceFace(m->cm);

    Color4b edgeCol[5]={Color4b::Black, Color4b::Green, Color4b::Black, Color4b::Red,Color4b::Magenta};
    Color4b faceCol[5]={Color4b::Black, Color4b::Green, Color4b::Black, Color4b::Red,Color4b::Magenta};
    Color4b faceVer[5];
    for(int i=0;i<5;++i) faceCol[i]=Color4b(faceCol[i][0],faceCol[i][1],faceCol[i][2],96);
    for(int i=0;i<5;++i) faceVer[i]=Color4b(faceCol[i][0],faceCol[i][1],faceCol[i][2],0);
    std::set<std::pair<CVertexO*,CVertexO*> > edgeSet; // this set is used to unique count the number of non manifold edges
    for(CMeshO::FaceIterator fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
    {
      for(int i=0;i<3;++i)
        {
          face::Pos<CFaceO> pos(&*fi,i);
          const int faceOnEdgeNum =  min(pos.NumberOfFacesOnEdge(),4);

          if(faceOnEdgeNum == 2 || faceOnEdgeNum == 1) continue;

          bool edgeNotPresent; // true if the edge was not present in the set
          if ( (*fi).V0(i)<(*fi).V1(i)) edgeNotPresent = edgeSet.insert(make_pair((*fi).V0(i),(*fi).V1(i))).second;
                                   else edgeNotPresent = edgeSet.insert(make_pair((*fi).V1(i),(*fi).V0(i))).second;

          if(edgeNotPresent){
            BVp->push_back(make_pair((*fi).V0(i)->P(),edgeCol[faceOnEdgeNum]));
            BVp->push_back(make_pair((*fi).V1(i)->P(),edgeCol[faceOnEdgeNum]));
          }
          FVp->push_back(make_pair((*fi).V0(i)->P(),faceCol[faceOnEdgeNum]));
          FVp->push_back(make_pair((*fi).V1(i)->P(),faceCol[faceOnEdgeNum]));
          FVp->push_back(make_pair((*fi).V2(i)->P(),faceVer[faceOnEdgeNum]));
        }
    }

  } break;
    case DP_SHOW_VERT_QUALITY_HISTOGRAM :
    {
      MeshModel *m=md.mm();

      CMeshO::PerMeshAttributeHandle<CHist > qH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<CHist>(m->cm,"VertQualityHist");

      if(!vcg::tri::Allocator<CMeshO>::IsValidHandle(m->cm,qH))
        qH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute<CHist>  (m->cm,std::string("VertQualityHist"));
      CHist *H = &qH();
      std::pair<float,float> minmax = tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m->cm);
      if(rm->getBool(UseFixedHistParam())) {
        minmax.first=rm->getFloat(FixedHistMinParam());
        minmax.second=rm->getFloat(FixedHistMaxParam());
      }

      H->SetRange( minmax.first, minmax.second, rm->getInt(HistBinNumParam()));
      if(rm->getBool(AreaHistParam()))
      {
        for(CMeshO::FaceIterator fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
        {
          float area6=DoubleArea(*fi)/6.0f;
          for(int i=0;i<3;++i)
            H->Add((*fi).V(i)->Q(),(*fi).V(i)->C(),area6);
        }
      } else {
        for(CMeshO::VertexIterator vi = m->cm.vert.begin(); vi!= m->cm.vert.end();++vi) if(!(*vi).IsD())
        {
          H->Add((*vi).Q(),(*vi).C(),1.0f);
        }
      }
    } break;
    case DP_SHOW_FACE_QUALITY_HISTOGRAM :
      {
        MeshModel *m=md.mm();

        CMeshO::PerMeshAttributeHandle<CHist > qH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<CHist>(m->cm,"FaceQualityHist");

            if(!vcg::tri::Allocator<CMeshO>::IsValidHandle(m->cm,qH))
              qH=vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute<CHist>  (m->cm,std::string("FaceQualityHist"));
            CHist *H = &qH();
            std::pair<float,float> minmax = tri::Stat<CMeshO>::ComputePerFaceQualityMinMax(m->cm);

            if(rm->getBool(UseFixedHistParam())) {
              minmax.first=rm->getFloat(FixedHistMinParam());
              minmax.second=rm->getFloat(FixedHistMaxParam());
            }

            H->SetRange( minmax.first,minmax.second, rm->getInt(HistBinNumParam()));
            if(rm->getBool(AreaHistParam())) {
              for(CMeshO::FaceIterator fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
                  H->Add((*fi).Q(),(*fi).C(),DoubleArea(*fi)*0.5f);
            } else {
              for(CMeshO::FaceIterator fi = m->cm.face.begin(); fi!= m->cm.face.end();++fi) if(!(*fi).IsD())
                  H->Add((*fi).Q(),(*fi).C(),1.0f);
          }
      } break;
  case DP_SHOW_CAMERA :
    {
      connect(gla,SIGNAL(transmitShot(QString,vcg::Shotf)),this,SLOT(setValue(QString,vcg::Shotf)));
      connect(this,SIGNAL(askViewerShot(QString)),gla,SLOT(sendViewerShot(QString)));
    }
  }
 return true;
}

void ExtraMeshDecoratePlugin::DrawFaceLabel(MeshModel &m, QPainter *painter)
{
	glPushAttrib(GL_LIGHTING_BIT  | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_LIGHTING);
	glColor3f(.4f,.4f,.4f);
				for(size_t i=0;i<m.cm.face.size();++i)
					if(!m.cm.face[i].IsD())
							{
								Point3f bar=Barycenter(m.cm.face[i]);
                glLabel::render(painter, bar,tr("%1").arg(i));
							}
	glPopAttrib();
}

void ExtraMeshDecoratePlugin::DrawEdgeLabel(MeshModel &m,QPainter *painter)
{
  glPushAttrib(GL_LIGHTING_BIT  | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
  glDepthFunc(GL_ALWAYS);
  glDisable(GL_LIGHTING);
  glColor3f(.4f,.4f,.4f);
  for(size_t i=0;i<m.cm.edge.size();++i)
    if(!m.cm.edge[i].IsD())
    {
      Point3f bar=(m.cm.edge[i].V(0)->P()+m.cm.edge[i].V(0)->P())/2.0f;
      glLabel::render(painter, bar,tr("%1").arg(i));
    }
  glPopAttrib();
}


void ExtraMeshDecoratePlugin::DrawVertLabel(MeshModel &m,QPainter *painter)
{
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_LIGHTING);
	glColor3f(.4f,.4f,.4f);
    for(size_t i=0;i<m.cm.vert.size();++i){
          if(!m.cm.vert[i].IsD())
                glLabel::render(painter, m.cm.vert[i].P(),tr("%1").arg(i));
    }
	glPopAttrib();			
}

void ExtraMeshDecoratePlugin::setValue(QString /*name*/,Shotf newVal)
{
    curShot=newVal;
}


void ExtraMeshDecoratePlugin::DisplayCamera(MeshModel *, Shotf &ls, int cameraSourceId, QPainter *painter, QFont /*qf*/)
{
  if(!ls.IsValid())
  {
    if(cameraSourceId == 1 ) glLabel::render2D(painter,glLabel::TOP_LEFT,"Current Mesh Has an invalid Camera");
    else if(cameraSourceId == 2 ) glLabel::render2D(painter,glLabel::TOP_LEFT,"Current Raster Has an invalid Camera");
    else glLabel::render2D(painter,glLabel::TOP_LEFT,"Current TrackBall Has an invalid Camera");
    return;
  }

  int ln=0;
  if(ls.Intrinsics.cameraType == Camera<float>::PERSPECTIVE) glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "Camera Type: Perspective");
  if(ls.Intrinsics.cameraType == Camera<float>::ORTHO)       glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "Camera Type: Orthographic");

  Point3f vp = ls.GetViewPoint();
  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("ViewPoint %1 %2 %3").arg(vp[0]).arg(vp[1]).arg(vp[2]));
  Point3f ax0 = ls.Axis(0);
  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("axis 0 - %1 %2 %3").arg(ax0[0]).arg(ax0[1]).arg(ax0[2]));
  Point3f ax1 = ls.Axis(1);
  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("axis 1 - %1 %2 %3").arg(ax1[0]).arg(ax1[1]).arg(ax1[2]));
  Point3f ax2 = ls.Axis(2);
  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("axis 2 - %1 %2 %3").arg(ax2[0]).arg(ax2[1]).arg(ax2[2]));
  float fov = ls.GetFovFromFocal();
  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("Fov %1 ( %2 x %3) ").arg(fov).arg(ls.Intrinsics.ViewportPx[0]).arg(ls.Intrinsics.ViewportPx[1]));
  float focal = ls.Intrinsics.FocalMm;
  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("Focal Lenght %1 (pxsize %2 x %3) ").arg(focal).arg(ls.Intrinsics.PixelSizeMm[0]).arg(ls.Intrinsics.PixelSizeMm[1]));
}

void ExtraMeshDecoratePlugin::DrawCamera(MeshModel *m, Shotf &ls, vcg::Color4b camcolor, vcg::Matrix44f &currtr, RichParameterSet *rm, QPainter */*painter*/, QFont /*qf*/)
{
  if(!ls.IsValid())  // no drawing if camera not valid
    return;

  if((m!=NULL) && (!m->visible))  // no drawing if mesh invisible
    return;

  Point3f vp = ls.GetViewPoint();
  Point3f ax0 = ls.Axis(0);
  Point3f ax1 = ls.Axis(1);
  Point3f ax2 = ls.Axis(2);
//  float fov = ls.GetFovFromFocal();
//  float focal = ls.Intrinsics.FocalMm;

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
    if(rm->getEnum(CameraScaleParam()) == 2)  // adaptive
    {}  //[TODO]

    // arbitrary size to draw axis
    float len;
    //if(m!=NULL)
	  //  len = m->cm.bbox.Diag()/20.0;
    //else
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
    Point3f viewportCenter = vp - (ax2*ls.Intrinsics.FocalMm * drawscale);
    Point3f viewportHorizontal = ax0* float(ls.Intrinsics.ViewportPx[0]*ls.Intrinsics.PixelSizeMm[0]/2.0f * drawscale);
    Point3f viewportVertical   = ax1* float(ls.Intrinsics.ViewportPx[1]*ls.Intrinsics.PixelSizeMm[1]/2.0f * drawscale);


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

void ExtraMeshDecoratePlugin::DrawColorHistogram(CHist &ch, GLArea *gla, QPainter *painter, RichParameterSet *par, QFont qf)
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
  if(par->getBool(this->UseFixedHistParam()))
  {
    histWide = par->getFloat(this->FixedHistWidthParam());
    if(histWide ==0 ) histWide = maxWide;
  }
  float bn = ch.BinNum();

  float border = 0.1;
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

  glColor(Color4b::White);
  drawQuotedLine(Point3d(border*4/5.0,border,0),Point3d(border*4/5.0,1.0-border,0),ch.MinV(),ch.MaxV(),len/20.0,painter,qf,0,true);
  glLabel::render(painter,Point3f(border,1-border*0.5,0),QString("MinV %1 MaxV %2 MaxC %3").arg(ch.MinElem()).arg(ch.MaxElem()).arg(maxWide));
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
void ExtraMeshDecoratePlugin::PlaceTexParam(int /*TexInd*/, int /*TexNum*/)
{

}


void ExtraMeshDecoratePlugin::DrawTexParam(MeshModel &m, GLArea *gla, QPainter *painter,  RichParameterSet *rm, QFont qf)
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
    if(!m.glw.TMId.empty())
      textureName = qPrintable(QString(m.cm.textures[0].c_str()))+QString("  ");
    //, QPainter *qDebug(qPrintable(textureName));
    glLabel::render(painter,Point3f(0.0,-0.10,0.0),textureName);
    checkGLError::qDebug("DrawTexParam");
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

    if(!m.glw.TMId.empty())
    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture( GL_TEXTURE_2D, m.glw.TMId.back() );
    }

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

void ExtraMeshDecoratePlugin::initGlobalParameterSet(QAction *action, RichParameterSet &parset)
{
    switch(ID(action)){
    case DP_SHOW_TEXPARAM : {
            assert(!parset.hasParameter(TextureStyleParam()));
            parset.addParam(new RichBool(TextureStyleParam(), true,"Texture Param Wire","if true the parametrization is drawn in a textured wireframe style"));
            parset.addParam(new RichBool(TextureFaceColorParam(), false,"Face Color","if true the parametrization is drawn with a per face color (useful if you want display per face parametrization distortion)"));
        } break;
    case DP_SHOW_VERT : {
            assert(!parset.hasParameter(VertDotSizeParam()));
            parset.addParam(new RichDynamicFloat(VertDotSizeParam(), 4,2,8,"Dot Size","if true the parametrization is drawn in a textured wireframe style"));
        } break;
    case DP_SHOW_FACE_NORMALS :
    case DP_SHOW_VERT_NORMALS :{
        if(!parset.hasParameter(NormalLength()))
        {
          parset.addParam(new RichFloat(NormalLength(),0.05,"Normal Length","The length of the normal expressed as a percentage of the bbox of the mesh"));
        }
      } break;
    case DP_SHOW_BOUNDARY :{
       assert(!parset.hasParameter(ShowBorderFlag()));
       parset.addParam(new RichBool(ShowBorderFlag(), true,"Show Border Faces","If true also the faces involved in border edges are shown in green"));
    } break;
    case DP_SHOW_FACE_QUALITY_HISTOGRAM :
    case DP_SHOW_VERT_QUALITY_HISTOGRAM :{
        if(!parset.hasParameter(HistBinNumParam()))
        {
          parset.addParam(new RichInt(HistBinNumParam(), 256,"Histogram Bins","If true the parametrization is drawn in a textured wireframe style"));
          parset.addParam(new RichBool(AreaHistParam(), false,"Area Weighted","If true the histogram is computed according to the surface of the involved elements.<br>"
                                       "e.g. each face contribute to the histogram proportionally to its area and each vertex with 1/3 of sum of the areas of the incident triangles."));
          parset.addParam(new RichBool(UseFixedHistParam(), false,"Fixed Histogram width","if true the parametrization is drawn in a textured wireframe style"));
          parset.addParam(new RichFloat(FixedHistMinParam(), 0,"Min Hist Value","Used only if the Fixed Histogram Width Parameter is checked"));
          parset.addParam(new RichFloat(FixedHistMaxParam(), 0,"Max Hist Value","Used only if the Fixed Histogram Width Parameter is checked"));
          parset.addParam(new RichFloat(FixedHistWidthParam(), 0,"Hist Width","If not zero, this value is used to scale histogram width  so that it is the indicated value.<br>"
                                                                              "Useful only if you have to compare multiple histograms.<br>"
                                                                              "Warning, with wrong values the histogram can become excessively flat or it can overflow"));
        }
      } break;

case DP_SHOW_CAMERA :{
    QStringList methods; methods << "Trackball" << "Mesh Camera" << "Raster Camera";
    QStringList scale; scale << "No Scale" << "Fixed Factor" << "Adaptive";
          parset.addParam(new RichEnum(this->CameraScaleParam(), 1, scale,"Camera Scale Method","Change rendering scale for better visibility in the scene"));
          parset.addParam(new RichFloat(this->FixedScaleParam(), 5.0,"Scale Factor","Draw scale. Used only if the Fixed Factor scaling is chosen"));
          parset.addParam(new RichBool(this->ShowMeshCameras(), false, "Show Mesh Cameras","if true, valid cameras are shown for all visible mesh layers"));
          parset.addParam(new RichBool(this->ShowRasterCameras(), true, "Show Raster Cameras","if true, valid cameras are shown for all visible raster layers"));
          parset.addParam(new RichBool(this->ShowCameraDetails(), false, "Show Current Camera Details","if true, prints on screen all intrinsics and extrinsics parameters for current camera"));
        } break;
    case DP_SHOW_NON_FAUX_EDGE :{
              parset.addParam(new RichBool(this->ShowSeparatrix(), false, "Show Quad mesh Separatrices","if true the lines connecting extraordinary vertices of a quad mesh are shown"));
              parset.addParam(new RichBool(this->ShowNonRegular(), false, "Show Non Regular Vertices","if true, vertices with valence not equal to four are shown with red/blue fans"));
            } break;
        }

}
Q_EXPORT_PLUGIN(ExtraMeshDecoratePlugin)
