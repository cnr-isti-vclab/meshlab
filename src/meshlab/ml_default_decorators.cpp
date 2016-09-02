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
#include <wrap/gui/coordinateframe.h>
#include <wrap/qt/gl_label.h>

#include "mainwindow.h"
#include "ml_selection_buffers.h"

MLDefaultMeshDecorators::MLDefaultMeshDecorators(MainWindow* mw)
	:_mw(mw)
{

}

bool MLDefaultMeshDecorators::updateMeshDecorationData( MeshModel& mesh,const MLRenderingData& previousdata,const MLRenderingData& currentdata )
{
    MLPerViewGLOptions oldopts;
    MLPerViewGLOptions currentopts;
    bool oldvalid = previousdata.get(oldopts);
    bool currentvalid = currentdata.get(currentopts);
    if ((!oldvalid) || (!currentvalid))
        return false;

	/*bool extraswitchon = currentopts._peredge_extra_enabled && !oldopts._peredge_extra_enabled;
	bool extraswitchoff = !currentopts._peredge_extra_enabled && oldopts._peredge_extra_enabled;

    //the boolean conditions should make the following code lines mutually exclusive.....hopefully
    initBoundaryDecoratorData(mesh,(extraswitchon || (currentopts._peredge_edgeboundary_enabled && !oldopts._peredge_edgeboundary_enabled)),
								   (extraswitchon || (currentopts._peredge_faceboundary_enabled && !oldopts._peredge_faceboundary_enabled)));
    cleanBoundaryDecoratorData(mesh,(extraswitchoff || (!currentopts._peredge_edgeboundary_enabled && oldopts._peredge_edgeboundary_enabled)),
									(extraswitchoff || (!currentopts._peredge_faceboundary_enabled && oldopts._peredge_faceboundary_enabled)));


    if (extraswitchon || (currentopts._peredge_edgemanifold_enabled && !oldopts._peredge_edgemanifold_enabled))
        initNonManifEdgeDecoratorData(mesh);
    else
        if (extraswitchoff || (!currentopts._peredge_edgemanifold_enabled && oldopts._peredge_edgemanifold_enabled))
            cleanNonManifEdgeDecoratorData(mesh);

    if (extraswitchon || (currentopts._peredge_text_boundary_enabled && !oldopts._peredge_text_boundary_enabled))
        initBoundaryTextDecoratorData(mesh);
    else
        if (extraswitchoff || (!currentopts._peredge_text_boundary_enabled && oldopts._peredge_text_boundary_enabled))
            cleanBoundaryTextDecoratorData(mesh);*/

	initBoundaryDecoratorData(mesh,currentopts._peredge_edgeboundary_enabled && !oldopts._peredge_edgeboundary_enabled,
								   currentopts._peredge_faceboundary_enabled && !oldopts._peredge_faceboundary_enabled);
	cleanBoundaryDecoratorData(mesh,!currentopts._peredge_edgeboundary_enabled && oldopts._peredge_edgeboundary_enabled,
									!currentopts._peredge_faceboundary_enabled && oldopts._peredge_faceboundary_enabled);


	if (currentopts._peredge_edgemanifold_enabled && !oldopts._peredge_edgemanifold_enabled)
		initNonManifEdgeDecoratorData(mesh);
	else
		if (!currentopts._peredge_edgemanifold_enabled && oldopts._peredge_edgemanifold_enabled)
			cleanNonManifEdgeDecoratorData(mesh);

	if (currentopts._peredge_vertmanifold_enabled && !oldopts._peredge_vertmanifold_enabled)
		initNonManifVertDecoratorData(mesh);
	else
		if (!currentopts._peredge_vertmanifold_enabled && oldopts._peredge_vertmanifold_enabled)
			cleanNonManifVertDecoratorData(mesh);

	if (currentopts._peredge_text_boundary_enabled && !oldopts._peredge_text_boundary_enabled)
		initBoundaryTextDecoratorData(mesh);
	else
		if (!currentopts._peredge_text_boundary_enabled && oldopts._peredge_text_boundary_enabled)
			cleanBoundaryTextDecoratorData(mesh); 
	
	initSelectionDecoratorData(mesh, currentopts._vertex_sel && !oldopts._vertex_sel, currentopts._face_sel && !oldopts._face_sel);
	cleanSelectionDecoratorData(mesh,!currentopts._vertex_sel && oldopts._vertex_sel, !currentopts._face_sel && oldopts._face_sel);

	return true;
}

bool MLDefaultMeshDecorators::initMeshDecorationData( MeshModel& m,const MLRenderingData& dt )
{
    MLPerViewGLOptions opts;
    bool valid = dt.get(opts);
    if (!valid)
        return false;

	//if (opts._peredge_extra_enabled)
	//{
	initBoundaryDecoratorData(m, opts._peredge_edgeboundary_enabled, opts._peredge_faceboundary_enabled);

	if (opts._peredge_edgemanifold_enabled)
		initNonManifEdgeDecoratorData(m);

	if (opts._peredge_text_boundary_enabled)
		initBoundaryTextDecoratorData(m);
	//}
	
	initSelectionDecoratorData(m, opts._vertex_sel, opts._face_sel);
	

    return true;
}

void MLDefaultMeshDecorators::decorateMesh( MeshModel & m,const MLRenderingData& dt, QPainter* painter, GLLogStream& log )
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
			if (opts._peredge_edgeboundary_enabled)
				drawLineVector(m.cm.Tr, beH());
            if(opts._peredge_faceboundary_enabled) 
                drawTriVector(m.cm.Tr, bfH());
            drawDotVector(m.cm.Tr, bvH(),5);
        
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
              CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > vvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,nonManifVertAttName());
              CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > tvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,nonManifVertFaceAttName());
              drawDotVector(m.cm.Tr, vvH());
              drawTriVector(m.cm.Tr, tvH());

              QString inf;
              inf += "<b>" + QString::number(vvH().size()) + " </b> non manifold vertices<br><b>" + QString::number(tvH().size() / 3) + "</b> faces over non manifold edges";
              log.RealTimeLog("Non Manifold Vertices",m.shortName(),inf);
        }

        if (opts._peredge_edgemanifold_enabled)
        {
            //Note the standard way for adding extra per-mesh data using the per-mesh attributes.
            CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,nonManifEdgeAttName());
            CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > fvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<PointPC> >(m.cm,nonManifEdgeFaceAttName());
            drawLineVector(m.cm.Tr, bvH());
            drawTriVector(m.cm.Tr, fvH());
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
			CMeshO::PerMeshAttributeHandle< MLSelectionBuffers* > selbufhand = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<MLSelectionBuffers* >(m.cm, selectionAttName());
			if (selbufhand() != NULL)
				selbufhand()->drawSelection(MLSelectionBuffers::ML_PERFACE_SEL);
        }

		if (opts._vertex_sel)
		{
			CMeshO::PerMeshAttributeHandle< MLSelectionBuffers* > selbufhand = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<MLSelectionBuffers* >(m.cm, selectionAttName());
			if (selbufhand() != NULL)
				selbufhand()->drawSelection(MLSelectionBuffers::ML_PERVERT_SEL);
		}
    }

    if (opts._perbbox_enabled)
    {
        if (opts._perbbox_quoted_info_enabled)
        {
            QFont qf;
            drawQuotedBox(m,painter,qf);
        }
    }
	glFinish();
}

void MLDefaultMeshDecorators::drawQuotedBox(MeshModel &m,QPainter *gla,QFont& qf)
{
    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT );
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
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrix(m.cm.Tr);
    // Mesh boundingBox
    Box3m b(m.cm.bbox);

    glLineWidth(1.f);
    glPointSize(3.f);

    vcg::Point3d p1,p2;

    Point3m c = b.Center();

    float s = 1.15f;
    const float LabelSpacing = 30;
    chooseX(b,mm,mp,vp,p1,p2);					// Selects x axis candidate
    glPushMatrix();
    glScalef(1,s,s);
    glTranslatef(0,c[1]/s-c[1],c[2]/s-c[2]);
    drawQuotedLine(p1,p2,b.min[0],b.max[0],vcg::CoordinateFrame::calcSlope(p1,p2,b.DimX(),LabelSpacing,mm,mp,vp),gla,qf);	// Draws x axis
    glPopMatrix();

    chooseY(b,mm,mp,vp,p1,p2);					// Selects y axis candidate
    glPushMatrix();
    glScalef(s,1,s);
    glTranslatef(c[0]/s-c[0],0,c[2]/s-c[2]);
    drawQuotedLine(p1,p2,b.min[1],b.max[1],vcg::CoordinateFrame::calcSlope(p1,p2,b.DimY(),LabelSpacing,mm,mp,vp),gla,qf);	// Draws y axis
    glPopMatrix();

    chooseZ(b,mm,mp,vp,p1,p2);					// Selects z axis candidate
    glPushMatrix();
    glScalef(s,s,1);
    glTranslatef(c[0]/s-c[0],c[1]/s-c[1],0);
    drawQuotedLine(p1,p2,b.min[2],b.max[2],vcg::CoordinateFrame::calcSlope(p1,p2,b.DimZ(),LabelSpacing,mm,mp,vp),gla,qf);	// Draws z axis
    glPopMatrix();
	glPopMatrix();
    glPopAttrib();

}

void MLDefaultMeshDecorators::drawQuotedLine(const vcg::Point3d &a,const vcg::Point3d &b, float aVal, float bVal, float tickScalarDistance, QPainter *painter, QFont& qf,float angle,bool rightAlign)
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

    vcg::Point3d Zero = a-((b-a)/(bVal-aVal))*aVal; // 3D Position of Zero.
    vcg::Point3d v(b-a);
    //v.Normalize();
    v = v*(1.0/(bVal-aVal));
    vcg::glLabel::Mode md(qf,vcg::Color4b(vcg::Color4b::White),angle,rightAlign);
    if(tickScalarDistance > 0)   // Draw lines only if the two endpoint are not coincident
    {
        neededZeros = std::ceil(std::max(0.0,-std::log10(double(tickScalarDistance))));
        glPointSize(3);
        float i;
        glBegin(GL_POINTS);
        for(i=firstTick;i<bVal;i+=tickScalarDistance)
            glVertex(Zero+v*i);
        glEnd();
        for(i=firstTick; (i+labelMargin)<bVal;i+=tickScalarDistance)
            vcg::glLabel::render(painter,Point3m::Construct(Zero+v*i),QString("%1   ").arg(i,4+neededZeros,'f',neededZeros),md);
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
    vcg::glLabel::render(painter,vcg::Point3f::Construct(a), QString("%1   ").arg(aVal,4+neededZeros,'f',neededZeros) ,md);
    vcg::glLabel::render(painter,vcg::Point3f::Construct(b), QString("%1   ").arg(bVal,4+neededZeros,'f',neededZeros) ,md);

    glPopAttrib();
}

void MLDefaultMeshDecorators::chooseX(Box3m &box,double *mm,double *mp,GLint *vp,vcg::Point3d &x1,vcg::Point3d &x2)
{
    float d = -std::numeric_limits<float>::max();
    vcg::Point3d c;
    // Project the bbox center
    gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
    c[2] = 0;

    vcg::Point3d out1,out2;
    Point3m in1,in2;

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


void MLDefaultMeshDecorators::chooseY(Box3m &box,double *mm,double *mp,GLint *vp,vcg::Point3d &y1,vcg::Point3d &y2)
{
    float d = -std::numeric_limits<float>::max();
    vcg::Point3d c;
    // Project the bbox center
    gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
    c[2] = 0;

    vcg::Point3d out1,out2;
    Point3m in1,in2;

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

void MLDefaultMeshDecorators::chooseZ(Box3m &box,double *mm,double *mp,GLint *vp,vcg::Point3d &z1,vcg::Point3d &z2)
{
    float d = -std::numeric_limits<float>::max();
    vcg::Point3d c;
    // Project the bbox center
    gluProject(box.Center()[0],box.Center()[1],box.Center()[2],mm,mp,vp,&c[0],&c[1],&c[2]);
    c[2] = 0;

    vcg::Point3d out1,out2;
    Point3m in1,in2;

    vcg::Point3d m;

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

	if (opts._vertex_sel || opts._face_sel)
		cleanSelectionDecoratorData(mesh, !opts._vertex_sel, !opts._face_sel);
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
	{
		beH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, boundaryEdgeAttName());
	}

    CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bfH;
	if (faceboundary)
	{
		bfH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, boundaryFaceAttName());
	}

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
	if (_mw == NULL)
		return;

	MLRenderingEdgeBoundaryAction eact(m.id(), NULL);
	unsigned int edgerequview = _mw->viewsRequiringRenderingActions(m.id(), &eact);
	if (edgeboundary)
	{		
		if (edgerequview == 0)
		{
			CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > beH;
			beH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, boundaryEdgeAttName());
			if (beH._handle != NULL)
				beH().clear();
			vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::vector<PointPC>>(m.cm, beH);
		}
	}

	MLRenderingFaceBoundaryAction fact(m.id(), NULL);
	unsigned int facerequview = _mw->viewsRequiringRenderingActions(m.id(), &fact);
    if (faceboundary)
	{
		if (facerequview == 0)
		{
			CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > beH;
			beH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, boundaryFaceAttName());
			if (beH._handle != NULL)
				beH().clear();
			vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::vector<PointPC>>(m.cm, beH);
		}
	}
        
	if ((facerequview == 0) && (edgerequview == 0))
	{
		CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > beH;
		beH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, boundaryVertAttName());
		if (beH._handle != NULL)
			beH().clear();
		vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::vector<PointPC>>(m.cm, beH);
	}
}

void MLDefaultMeshDecorators::initSelectionDecoratorData(MeshModel & mm, bool vertsel, bool facesel)
{
	CMeshO::PerMeshAttributeHandle< MLSelectionBuffers* > selbufhand;
	selbufhand = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< MLSelectionBuffers* >(mm.cm, selectionAttName());
	if (selbufhand() == NULL)
		selbufhand() = new MLSelectionBuffers(mm, 100000);
	if (vertsel)
		selbufhand()->updateBuffer(MLSelectionBuffers::ML_PERVERT_SEL);
	if (facesel)
		selbufhand()->updateBuffer(MLSelectionBuffers::ML_PERFACE_SEL);
}

void MLDefaultMeshDecorators::cleanSelectionDecoratorData(MeshModel& mm, bool vertsel, bool facesel)
{
	if (_mw == NULL)
		return;

	CMeshO::PerMeshAttributeHandle< MLSelectionBuffers* > selbufhand = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<MLSelectionBuffers*>(mm.cm, selectionAttName());
	MLSelectionBuffers* tmp = selbufhand();

	MLRenderingVertSelectionAction vact(mm.id(), NULL);
	unsigned int vertselreqview = _mw->viewsRequiringRenderingActions(mm.id(), &vact);
	if (vertsel && (tmp != NULL) && (vertselreqview == 0))
		tmp->deallocateBuffer(MLSelectionBuffers::ML_PERVERT_SEL);
	
	MLRenderingFaceSelectionAction fact(mm.id(), NULL);
	unsigned int faceselreqview = _mw->viewsRequiringRenderingActions(mm.id(), &fact);
	if (facesel && (tmp != NULL) && (faceselreqview == 0))
		tmp->deallocateBuffer(MLSelectionBuffers::ML_PERFACE_SEL);

	if ((faceselreqview == 0) && (vertselreqview == 0))
	{
		delete tmp;
		vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<MLSelectionBuffers*>(mm.cm, selbufhand);
	}
}

void MLDefaultMeshDecorators::initNonManifEdgeDecoratorData(MeshModel& m)
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
	if (_mw == NULL)
		return;

	MLRenderingEdgeManifoldAction eact(m.id(), NULL);
	unsigned int manifreqviews = _mw->viewsRequiringRenderingActions(m.id(), &eact);

	if (manifreqviews == 0)
	{
		CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > beH;
		beH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, nonManifEdgeAttName());
		if (beH._handle != NULL)
			beH().clear();
		vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::vector<PointPC>>(m.cm, beH);

		CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bef;
		bef = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, nonManifEdgeFaceAttName());
		if (bef._handle != NULL)
			bef().clear();
		vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::vector<PointPC>>(m.cm, bef);

	}
}

void MLDefaultMeshDecorators::initNonManifVertDecoratorData(MeshModel& mm)
{
	CMeshO::PerMeshAttributeHandle<std::vector<PointPC> > bvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(mm.cm, nonManifVertAttName());
	CMeshO::PerMeshAttributeHandle<std::vector<PointPC> > fvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(mm.cm, nonManifVertFaceAttName());
	std::vector<PointPC> *BVp = &bvH();
	std::vector<PointPC> *FVp = &fvH();
	BVp->clear();
	FVp->clear();
	mm.updateDataMask(MeshModel::MM_FACEFACETOPO);
	vcg::tri::SelectionStack<CMeshO> ss(mm.cm);
	ss.push();
	vcg::tri::UpdateSelection<CMeshO>::VertexClear(mm.cm);
	int res = vcg::tri::Clean<CMeshO>::CountNonManifoldVertexFF(mm.cm, true);
	vcg::Color4b bCol(255, 0, 255, 0);
	vcg::Color4b vCol(255, 0, 255, 64);
	vcg::tri::UpdateFlags<CMeshO>::VertexClearV(mm.cm);
	for (CMeshO::FaceIterator fi = mm.cm.face.begin(); fi != mm.cm.face.end(); ++fi)
	{
		if (!(*fi).IsD())
		{
			for (int i = 0; i < 3; ++i)
			{
				if ((*fi).V(i)->IsS())
				{
					if (!(*fi).V0(i)->IsV())
					{
						BVp->push_back(std::make_pair((*fi).V0(i)->P(), vcg::Color4b(vcg::Color4b::Magenta)));
						(*fi).V0(i)->SetV();
					}

					Point3m P1 = ((*fi).V0(i)->P() + (*fi).V1(i)->P()) / 2.0f;
					Point3m P2 = ((*fi).V0(i)->P() + (*fi).V2(i)->P()) / 2.0f;
					FVp->push_back(std::make_pair((*fi).V0(i)->P(), vCol));
					FVp->push_back(std::make_pair(P1, bCol));
					FVp->push_back(std::make_pair(P2, bCol));
				}
			}
		}
	}
	ss.pop();
}

void MLDefaultMeshDecorators::cleanNonManifVertDecoratorData(MeshModel& m)
{
	if (_mw == NULL)
		return;

	MLRenderingVertManifoldAction eact(m.id(), NULL);
	unsigned int manifreqviews = _mw->viewsRequiringRenderingActions(m.id(), &eact);

	if (manifreqviews == 0)
	{
		CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > beH;
		beH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, nonManifVertAttName());
		if (beH._handle != NULL)
			beH().clear();
		vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::vector<PointPC>>(m.cm, beH);

		CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > bef;
		bef = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, nonManifVertFaceAttName());
		if (bef._handle != NULL)
			bef().clear();
		vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::vector<PointPC>>(m.cm, bef);

	}
}

void MLDefaultMeshDecorators::initBoundaryTextDecoratorData( MeshModel& m)
{
	if (!m.hasDataMask(MeshModel::MM_WEDGTEXCOORD) && !m.hasDataMask(MeshModel::MM_VERTTEXCOORD))
		return;
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
	MLRenderingTexBorderAction eact(m.id(), NULL);
	unsigned int manifreqviews = _mw->viewsRequiringRenderingActions(m.id(), &eact);

	if (manifreqviews == 0)
	{
		CMeshO::PerMeshAttributeHandle< std::vector<PointPC> > beH;
		beH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<PointPC> >(m.cm, boundaryTextVertAttName());
		if (beH._handle != NULL)
			beH().clear();
		vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::vector<PointPC>>(m.cm, beH);
	}
}

void MLDefaultMeshDecorators::drawLineVector(const vcg::Matrix44f& tr, std::vector<PointPC> &EV)
{
    glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT| GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.f);
    glDepthRange (0.0, 0.999);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrix(tr);
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
	glPopMatrix();
    glPopAttrib();
}

void MLDefaultMeshDecorators::drawTriVector(const vcg::Matrix44f& tr,std::vector<PointPC> &TV)
{
    glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT| GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.f);
    glDepthRange (0.0, 0.999);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrix(tr);
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
	glPopMatrix();
    glPopAttrib();
}

void MLDefaultMeshDecorators::drawDotVector(const vcg::Matrix44f& tr,std::vector<PointPC> &TV, float baseSize)
{
    glPushAttrib(GL_ENABLE_BIT|GL_VIEWPORT_BIT| GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.f);
    glDepthRange (0.0, 0.999);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrix(tr);
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
	glPopMatrix();
    glPopAttrib();
}







