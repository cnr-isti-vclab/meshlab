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

#include "ml_default_decorators.h"
#include <vector>
#include <wrap/gl/gl_type_name.h>


bool MLDefaultMeshDecorators::updateMeshDecorationData( MeshModel& mesh,const MLRenderingData& previousdata,const MLRenderingData& currentdata )
{
    MLPerViewGLOptions oldopts;
    MLPerViewGLOptions currentopts;
    bool oldvalid = previousdata.get(oldopts);
    bool currentvalid = currentdata.get(currentopts);
    if ((!oldvalid) || (!currentvalid))
        return false;

    /*the boolean conditions should make the following code lines mutually exclusive.....hopefully*/ 
    initBoundaryDecoratorData(mesh,currentopts._peredge_edgeboundary_enabled && !oldopts._peredge_edgeboundary_enabled,
                                   currentopts._peredge_faceboundary_enabled && !oldopts._peredge_faceboundary_enabled);
    cleanBoundaryDecoratorData(mesh,!currentopts._peredge_edgeboundary_enabled && oldopts._peredge_edgeboundary_enabled,
                                    !currentopts._peredge_faceboundary_enabled && oldopts._peredge_faceboundary_enabled);


    if (currentopts._peredge_edgemanifold_enabled && !oldopts._peredge_edgemanifold_enabled)
        initNonManifEdgeDecoratorData(mesh);
    else
        if (!currentopts._peredge_edgemanifold_enabled && oldopts._peredge_edgemanifold_enabled)
            cleanNonManifEdgeDecoratorData(mesh);

    if (currentopts._peredge_text_boundary_enabled && !oldopts._peredge_text_boundary_enabled)
        initBoundaryTextDecoratorData(mesh);
    else
        if (!currentopts._peredge_text_boundary_enabled && oldopts._peredge_text_boundary_enabled)
            cleanBoundaryTextDecoratorData(mesh);
}

bool MLDefaultMeshDecorators::initMeshDecorationData( MeshModel& m,const MLRenderingData& dt )
{
    MLPerViewGLOptions opts;
    bool valid = dt.get(opts);
    if (!valid)
        return false;

    initBoundaryDecoratorData(m,opts._peredge_edgeboundary_enabled,opts._peredge_faceboundary_enabled);

    if (opts._peredge_edgemanifold_enabled)
        initNonManifEdgeDecoratorData(m);

    if (opts._peredge_text_boundary_enabled)
        initBoundaryTextDecoratorData(m);

    return true;
}

void MLDefaultMeshDecorators::decorateMesh( MeshModel & m,const MLRenderingData& dt, QPainter* /*painter*/, GLLogStream& log )
{
    MLPerViewGLOptions opts;
    bool valid = dt.get(opts);
    if (!valid)
        return;

    if (opts._peredge_extra_enabled)
    {
        if (opts._peredge_edgeboundary_enabled || opts._peredge_faceboundary_enabled)
        {
            CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,boundaryVertAttName());
            CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > beH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,boundaryEdgeAttName());
            CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bfH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,boundaryFaceAttName());
            drawLineVector(beH());
            if(opts._peredge_faceboundary_enabled) 
                drawTriVector(bfH());
            drawDotVector(bvH(),5);
        
            if (opts._peredge_edgeboundary_enabled)
            {
                QString inf;
                if(m.cm.fn==0) 
                    inf += "<b>" + QString::number(bvH().size()) + " </b> vertex";
                else
                    inf += "<b>" + QString::number(beH().size()/2) + " </b> edges";
                log.RealTimeLog("Boundary",m.shortName(),inf);
            }

            if (opts._peredge_faceboundary_enabled)
            {
                QString inf;
                if(m.cm.fn==0) 
                    inf += "<b>" + QString::number(bvH().size()) + " </b> vertex";
                else
                    inf += "<b>" + QString::number(bfH().size()/3) + " </b> faces";
                log.RealTimeLog("Boundary Faces",m.shortName(),inf);
            }      
        }

        if (opts._peredge_vertmanifold_enabled)
        {
              // Note the standard way for adding extra per-mesh data using the per-mesh attributes.
              CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > vvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,"NonManifVertVertVector");
              CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > tvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,"NonManifVertTriVector");
              drawDotVector(vvH());
              drawTriVector(tvH());

              QString inf;
              inf += "<b>" + QString::number(vvH().size()) + " </b> non manifold vertices<br><b>" + QString::number(tvH().size()) + "</b> faces over non manifold edges";
              log.RealTimeLog("Non Manifold Vertices",m.shortName(),inf);
        }

        if (opts._peredge_edgemanifold_enabled)
        {
            //Note the standard way for adding extra per-mesh data using the per-mesh attributes.
            CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,nonManifEdgeAttName());
            CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > fvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,nonManifEdgeFaceAttName());
            drawLineVector(bvH());
            drawTriVector(fvH());
            QString inf;
            inf += "<b>" + QString::number(bvH().size()/2) + " </b> non manifold edges<br><b>" + QString::number(fvH().size()/3) + "</b> faces over non manifold edges";
            log.RealTimeLog("Non Manifold Edges",m.shortName(),inf);
        }

        if (opts._peredge_text_boundary_enabled)
        {
            CMeshO::PerMeshAttributeHandle< std::vector<Point3m> > btvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<Point3m> >(m.cm,boundaryTextVertAttName());
            std::vector<Point3m> *BTVp = &btvH();
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
                glColor(vcg::Color4b(vcg::Color4b::Magenta));
                glDepthRange (0.0, 0.999);
                glEnableClientState (GL_VERTEX_ARRAY);
                glVertexPointer(3,vcg::GL_TYPE_NM<Scalarm>::SCALAR(),sizeof(Point3m),&(BTVp->begin()[0]));
                glDrawArrays(GL_LINES,0,BTVp->size());
                glDisableClientState (GL_VERTEX_ARRAY);
                glPopAttrib();
            }
        }
    }

    if (opts._sel_enabled)
    {
        if (opts._face_sel)
        {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
            glColor4f(1.0f,0.0,0.0,.3f);
            glPolygonOffset(-1.0, -1);
            CMeshO::FaceIterator fi;
            glPushMatrix();
            glMultMatrix(m.cm.Tr);
            glBegin(GL_TRIANGLES);
            m.cm.sfn=0;
            //for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi)
            //{
            //    if(!(*fi).IsD() && (*fi).IsS())
            //    {
            //        /*glVertex((*fi).cP(0));
            //        glVertex((*fi).cP(1));
            //        glVertex((*fi).cP(2));*/
            //        ++m.cm.sfn;
            //    }
            //}
            glEnd();
            glPopMatrix();
            glPopAttrib();
        }
    }
}

bool MLDefaultMeshDecorators::cleanMeshDecorationData( MeshModel& mesh,const MLRenderingData& dt )
{
    MLPerViewGLOptions opts;
    bool valid = dt.get(opts);
    if (!valid)
        return false;

    if (opts._peredge_edgeboundary_enabled || opts._peredge_faceboundary_enabled)
        cleanBoundaryDecoratorData(mesh,!opts._peredge_edgeboundary_enabled,!opts._peredge_faceboundary_enabled);

    if (opts._peredge_edgemanifold_enabled)
        cleanNonManifEdgeDecoratorData(mesh);

    if (opts._peredge_text_boundary_enabled)
        cleanBoundaryTextDecoratorData(mesh);
    return true;
}

void MLDefaultMeshDecorators::initBoundaryDecoratorData( MeshModel& m,bool edgeboundary,bool faceboundary)
{
    if (!edgeboundary && !faceboundary)
        return;

    CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bvH;
    bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm,boundaryVertAttName());
    CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > beH;
    if (edgeboundary)
        beH= vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm,boundaryEdgeAttName());
    CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bfH;
    if (faceboundary)
        bfH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm,boundaryFaceAttName());

    std::vector<PointPC> *BVp = &bvH();
    std::vector<PointPC> *BEp = NULL;
    if (edgeboundary)
        BEp = &beH();

    std::vector<PointPC> *BFp = NULL;
    if (faceboundary)
        BFp = &bfH();

    if (BVp != NULL)
        BVp->clear();
    
    if ((BEp != NULL) && (edgeboundary))
        BEp->clear();
    
    if ((BFp != NULL) && (faceboundary))
        BFp->clear();

    vcg::Color4b bCol=vcg::Color4b(0,255,0,32);

    if(!m.cm.edge.empty())
    {
        std::vector<int> cntVec(m.cm.vert.size(),0);
        for(CMeshO::EdgeIterator ei=m.cm.edge.begin(); ei!= m.cm.edge.end();++ei) if(!(*ei).IsD())
        {
            cntVec[vcg::tri::Index(m.cm,ei->V(0))]++;
            cntVec[vcg::tri::Index(m.cm,ei->V(1))]++;
        }

        if (BVp != NULL)
        {
            for(size_t i=0;i<cntVec.size();++i)
                if (cntVec[i]==1)
                    BVp->push_back(std::make_pair(m.cm.vert[i].P(),vcg::Color4b(vcg::Color4b::Green)));
        }
    }
    vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(m.cm);
    for(CMeshO::FaceIterator fi = m.cm.face.begin(); fi!= m.cm.face.end();++fi) 
    {
        if(!(*fi).IsD())
        {
            bool isB=false;
            for(int i=0;i<3;++i)
            {
                if((*fi).IsB(i))
                {
                    isB=true;
                    if ((BEp != NULL) && (edgeboundary))
                    {
                        BEp->push_back(std::make_pair((*fi).V0(i)->P(),vcg::Color4b(vcg::Color4b::Green)));
                        BEp->push_back(std::make_pair((*fi).V1(i)->P(),vcg::Color4b(vcg::Color4b::Green)));
                    }
                }
            }
            if ((isB) && (BFp != NULL) && (faceboundary))
            {
                BFp->push_back(std::make_pair((*fi).V(0)->P(),bCol));
                BFp->push_back(std::make_pair((*fi).V(1)->P(),bCol));
                BFp->push_back(std::make_pair((*fi).V(2)->P(),bCol));
            }           
        }
    }
}


void MLDefaultMeshDecorators::cleanBoundaryDecoratorData( MeshModel& m,bool edgeboundary,bool faceboundary)
{
    if (edgeboundary && faceboundary)
        vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute(m.cm,boundaryVertAttName());
    if (edgeboundary)
        vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute(m.cm,boundaryEdgeAttName());
    if (faceboundary)
        vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute(m.cm,boundaryFaceAttName());
}

void MLDefaultMeshDecorators::initNonManifEdgeDecoratorData( MeshModel& m )
{
    CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm,nonManifEdgeAttName());
    CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > fvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm,nonManifEdgeFaceAttName());
    std::vector<PointPC> *BVp = &bvH();
    std::vector<PointPC> *FVp = &fvH();
    BVp->clear();
    FVp->clear();

    m.updateDataMask(MeshModel::MM_FACEFACETOPO);

    vcg::Color4b edgeCol[5]={vcg::Color4b(vcg::Color4b::Black), vcg::Color4b(vcg::Color4b::Green), vcg::Color4b(vcg::Color4b::Black), vcg::Color4b(vcg::Color4b::Red),vcg::Color4b(vcg::Color4b::Magenta)};
    vcg::Color4b faceCol[5]={vcg::Color4b(vcg::Color4b::Black), vcg::Color4b(vcg::Color4b::Green), vcg::Color4b(vcg::Color4b::Black), vcg::Color4b(vcg::Color4b::Red),vcg::Color4b(vcg::Color4b::Magenta)};
    vcg::Color4b faceVer[5];
    for(int i=0;i<5;++i) 
        faceCol[i]=vcg::Color4b(faceCol[i][0],faceCol[i][1],faceCol[i][2],96);
    for(int i=0;i<5;++i) 
        faceVer[i]=vcg::Color4b(faceCol[i][0],faceCol[i][1],faceCol[i][2],0);
    std::set<std::pair<CVertexO*,CVertexO*> > edgeSet; // this set is used to unique count the number of non manifold edges
    for(CMeshO::FaceIterator fi = m.cm.face.begin(); fi!= m.cm.face.end();++fi) if(!(*fi).IsD())
    {
        for(int i=0;i<3;++i)
        {
            vcg::face::Pos<CFaceO> pos(&*fi,i);
            const int faceOnEdgeNum =  std::min(pos.NumberOfFacesOnEdge(),4);

            if(faceOnEdgeNum == 2 || faceOnEdgeNum == 1) continue;

            bool edgeNotPresent; // true if the edge was not present in the set
            if ( (*fi).V0(i)<(*fi).V1(i)) edgeNotPresent = edgeSet.insert(std::make_pair((*fi).V0(i),(*fi).V1(i))).second;
            else edgeNotPresent = edgeSet.insert(std::make_pair((*fi).V1(i),(*fi).V0(i))).second;

            if(edgeNotPresent){
                BVp->push_back(std::make_pair((*fi).V0(i)->P(),edgeCol[faceOnEdgeNum]));
                BVp->push_back(std::make_pair((*fi).V1(i)->P(),edgeCol[faceOnEdgeNum]));
            }
            FVp->push_back(std::make_pair((*fi).V0(i)->P(),faceCol[faceOnEdgeNum]));
            FVp->push_back(std::make_pair((*fi).V1(i)->P(),faceCol[faceOnEdgeNum]));
            FVp->push_back(std::make_pair((*fi).V2(i)->P(),faceVer[faceOnEdgeNum]));
        }
    }
}

void MLDefaultMeshDecorators::cleanNonManifEdgeDecoratorData( MeshModel& m )
{
    vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute(m.cm,nonManifEdgeAttName());
    vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute(m.cm,nonManifEdgeFaceAttName());
}

void MLDefaultMeshDecorators::initBoundaryTextDecoratorData( MeshModel& m)
{
    m.updateDataMask(MeshModel::MM_FACEFACETOPO);
    CMeshO::PerMeshAttributeHandle< std::vector<Point3m> > btvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<Point3m> >(m.cm,boundaryTextVertAttName());
    std::vector<Point3m> *BTVp = &btvH();
    BTVp->clear();
    std::vector<std::pair<CMeshO::FacePointer,int> > SaveTopoVec;
    CMeshO::FaceIterator fi;
    for(fi = m.cm.face.begin(); fi!= m.cm.face.end();++fi) if(!(*fi).IsD())
    {
        for(int i=0;i<3;++i)
            SaveTopoVec.push_back(std::make_pair((*fi).FFp(i),(*fi).FFi(i)));
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(m.cm);
    for(fi = m.cm.face.begin(); fi!= m.cm.face.end();++fi) if(!(*fi).IsD())
    {
        for(int i=0;i<3;++i)
            if(vcg::face::IsBorder(*fi,i))
            {
                BTVp->push_back((*fi).V0(i)->P());
                BTVp->push_back((*fi).V1(i)->P());
            }
    }
    std::vector<std::pair<CMeshO::FacePointer,int> >::iterator iii;
    for(fi = m.cm.face.begin(), iii=SaveTopoVec.begin(); fi!= m.cm.face.end();++fi) if(!(*fi).IsD())
    {
        for(int i=0;i<3;++i)
        {
            (*fi).FFp(i)= iii->first;
            (*fi).FFi(i)= iii->second;
        }
    }
}

void MLDefaultMeshDecorators::cleanBoundaryTextDecoratorData( MeshModel& m)
{
    vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute(m.cm,boundaryTextVertAttName());
}

void MLDefaultMeshDecorators::drawLineVector(std::vector<PointPC> &EV)
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

void MLDefaultMeshDecorators::drawTriVector(std::vector<PointPC> &TV)
{
    glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT| GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.f);
    glDepthRange (0.0, 0.999);
    if (TV.size() > 0)
    {
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_COLOR_ARRAY);
        glVertexPointer(3,vcg::GL_TYPE_NM<Scalarm>::SCALAR(),sizeof(PointPC),&(TV.begin()[0].first));
        glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(TV.begin()[0].second));
        glDrawArrays(GL_TRIANGLES,0,TV.size());
        glDisableClientState (GL_COLOR_ARRAY);
        glDisableClientState (GL_VERTEX_ARRAY);
    }
    glPopAttrib();
}

void MLDefaultMeshDecorators::drawDotVector(std::vector<PointPC> &TV, float baseSize)
{
    glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT| GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.f);
    glDepthRange (0.0, 0.999);
    if (TV.size() > 0)
    {
        glEnableClientState (GL_VERTEX_ARRAY);
        glEnableClientState (GL_COLOR_ARRAY);
        glPointSize(baseSize+0.5);
        glVertexPointer(3,vcg::GL_TYPE_NM<Scalarm>::SCALAR(),sizeof(PointPC),&(TV.begin()[0].first));
        glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(PointPC),&(TV.begin()[0].second));
        glDisableClientState (GL_COLOR_ARRAY);
        glColor(vcg::Color4b(vcg::Color4b::DarkGray));
        glDrawArrays(GL_POINTS,0,TV.size());
        glPointSize(baseSize-1);
        glEnableClientState (GL_COLOR_ARRAY);
        glDrawArrays(GL_POINTS,0,TV.size());
        glDisableClientState (GL_VERTEX_ARRAY);
    }
    glPopAttrib();
}







