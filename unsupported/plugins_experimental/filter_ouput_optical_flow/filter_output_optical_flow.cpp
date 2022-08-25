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

#include "filter_output_optical_flow.h"
#include <QtGui>
#include <wrap/gl/shot.h>
#include <common/pluginmanager.h>
#include <fstream>
#include <wrap/qt/shot_qt.h>
#include "utils.h"




FilterOutputOpticalFlowPlugin::FilterOutputOpticalFlowPlugin()
{
    typeList << FP_OUTPUT_OPTICAL_FLOW;

	foreach( FilterIDType tt , types() )
		actionList << new QAction(filterName(tt), this);
}


QString FilterOutputOpticalFlowPlugin::filterName( FilterIDType id ) const
{
	switch( id )
    {
		case FP_OUTPUT_OPTICAL_FLOW:  return QString( "Export optical flow project" );
		default: assert(0); return QString();
	}
}


QString FilterOutputOpticalFlowPlugin::filterInfo( FilterIDType id ) const
{
	switch( id )
    {
		case FP_OUTPUT_OPTICAL_FLOW:  return QString( "Export the XML project file and the overlapping retro-projected images required for the optical flow algorithm.");
        default: assert(0); return QString();
	}
}


int FilterOutputOpticalFlowPlugin::getRequirements( QAction *act )
{
    switch( ID(act) )
    {
        case FP_OUTPUT_OPTICAL_FLOW:  return MeshModel::MM_NONE;
        default:  assert(0); return 0;
    }
}


MeshFilterInterface::FilterClass FilterOutputOpticalFlowPlugin::getClass( QAction *act )
{
    switch( ID(act) )
    {
        case FP_OUTPUT_OPTICAL_FLOW:  return Texture;
        default:  assert(0); return MeshFilterInterface::Generic;
    }
}


void FilterOutputOpticalFlowPlugin::initParameterSet( QAction *act,
                                                      MeshDocument &/*md*/,
                                                      RichParameterSet &par )
{
	switch( ID(act) )
    {
		case FP_OUTPUT_OPTICAL_FLOW:
        {
            par.addParam( new RichString("xmlFileName",
                                         "flow.xml",
                                         "XML file",
                                         "Name of the XML project file required by the optical flow algorithm") );
            par.addParam( new RichBool("useDistanceWeight",
                                       true,
                                       "Use distance weight",
                                       "Includes a weight accounting for the distance to the camera during the computation of reference images") );
            par.addParam( new RichBool("useImgBorderWeight",
                                       true,
                                       "Use image border weight",
                                       "Includes a weight accounting for the distance to the image border during the computation of reference images") );
            par.addParam( new RichBool("useSilhouetteWeight",
                                       true,
                                       "Use silhouette weight",
                                       "Includes a weight accounting for the distance to silhouettes and step discontinuities during the computation of reference images") );
            par.addParam( new RichBool("useOrientationWeight",
                                       true,
                                       "Use orientation weight",
                                       "Includes a weight accounting for the orientation of the surface wrt. the camera during the computation of reference images") );
            //par.addParam( new RichInt("dominantAreaExpansion",
            //                          4,
            //                          "Area expansion",
            //                          "Width (in triangles) of the border to add to each dominant area.") );
            par.addParam( new RichFloat("minCoverage",
                                        2.0f,
                                        "Min coverage (%)",
                                        "Min coverage (%)") );
            par.addParam( new RichBool("colorFromDominancy",
                                       false,
                                       "Vertex color from dominant image",
                                       "Color vertices with respect to the image they have been associated to") );
            break;
		}
	}
}


bool FilterOutputOpticalFlowPlugin::applyFilter( QAction *act,
                                                 MeshDocument &md,
                                                 RichParameterSet &par,
                                                 vcg::CallBackPos * /*cb*/ )
{
    if( glewInit() != GLEW_OK )
        return false;


    bool retValue = true;
    m_Mesh = &md.mm()->cm;


    QList<OOCRaster> rasters;
    if( md.rasterList.isEmpty() )
    {
        QString filename = QFileDialog::getOpenFileName( NULL, "Select a MeshLab project file", QString(), "MeshLab project (*.mlp)" );
        if( filename.isNull() || !loadRasterList(filename,rasters) )
            return false;
    }
    else
    {
        foreach( RasterModel *rm, md.rasterList )
            rasters.push_back( OOCRaster(rm) );
    }


    std::list<vcg::Shotf> initialShots;
    for( QList<OOCRaster>::iterator r=rasters.begin(); r!=rasters.end(); ++r )
    {
        initialShots.push_back( r->shot );
        r->shot.ApplyRigidTransformation( vcg::Inverse(m_Mesh->Tr) );
    }


    switch( ID(act) )
    {
		case FP_OUTPUT_OPTICAL_FLOW:
		{
            for( CMeshO::FaceIterator f=m_Mesh->face.begin(); f!=m_Mesh->face.end(); ++f )
                f->ClearV();

            int weightMask = 0;
            if( par.getBool("useDistanceWeight") )
                weightMask |= DominancyClassifier::W_DISTANCE;
            if( par.getBool("useImgBorderWeight") )
                weightMask |= DominancyClassifier::W_IMG_BORDER;
            if( par.getBool("useSilhouetteWeight") )
                weightMask |= DominancyClassifier::W_SILHOUETTE;
            if( par.getBool("useOrientationWeight") )
                weightMask |= DominancyClassifier::W_ORIENTATION;

            DominancyClassifier *set = new DominancyClassifier( *m_Mesh, rasters, weightMask );

            if( par.getBool("colorFromDominancy") )
            {
                md.mm()->updateDataMask( MeshModel::MM_VERTCOLOR );

                for( CMeshO::VertexIterator v=m_Mesh->vert.begin(); v!=m_Mesh->vert.end(); ++v )
                    if( (*set)[v].isOnBoundary() )
                    {
                        unsigned char c = (unsigned char)( 255.0f*(*set)[v].borderWeight() );
                        v->C() = vcg::Color4b( 255-c, 255-c, 255-c, 255 );
                    }
                    else
                        v->C() = vcg::Color4b( 0, 0, 255, 255 );

                delete set;
            }
            else
            {
                RasterFaceMap facesByDomImg;
                set->dominancyCoverage( facesByDomImg );
                delete set;

                //if( int n = par.getInt("dominantAreaExpansion") )
                //{
                //    md.mm()->updateDataMask( MeshModel::MM_FACEFACETOPO );
                //    md.mm()->updateDataMask( MeshModel::MM_VERTFACETOPO );

                //    for( RasterFaceMap::iterator rm=facesByDomImg.begin(); rm!=facesByDomImg.end(); ++rm )
                //        expands( rm.value(), n );
                //}

                QMap<int,QVector<int>> validPairs;
                retroProjection( facesByDomImg, 0.01f*par.getFloat("minCoverage"), validPairs );
                saveXMLProject( par.getString("xmlFileName"), md.mm(), facesByDomImg, validPairs );
            }

            break;
		}
	}


    return retValue;
}


bool FilterOutputOpticalFlowPlugin::loadRasterList( QString &mlpFilename,
                                                    QList<OOCRaster> &rasters )
{
    QFile qf(mlpFilename);
	QFileInfo qfInfo(mlpFilename);
	QDir tmpDir = QDir::current();		
	QDir::setCurrent(qfInfo.absoluteDir().absolutePath());
	if( !qf.open(QIODevice::ReadOnly ) )
		return false;

	QString project_path = qfInfo.absoluteFilePath();

	QDomDocument doc("MeshLabDocument");    //It represents the XML document
	if(!doc.setContent( &qf ))	
		return false;

	QDomElement root = doc.documentElement();

	QDomNode node;

	node = root.firstChild();

	//Devices
	while( !node.isNull() )
    {
		if(QString::compare(node.nodeName(),"RasterGroup")==0)
		{
			QDomNode raster; QString filen, label;
			raster = node.firstChild();
			while(!raster.isNull())
			{
                QString imgFile;
				QDomElement el = raster.firstChildElement("Plane");
				while(!el.isNull())
				{
					QString sem = el.attribute("semantic");
                    if( sem == "RGB" )
                    {
					    QString filen = el.attribute("fileName");
					    QFileInfo fi(filen);
                        imgFile = fi.absoluteFilePath();
                    }
					el = node.nextSiblingElement("Plane");
				}

                if( !imgFile.isNull() )
                {
				    QDomNode sh=raster.firstChild();
                    vcg::Shotf shot;
                    ReadShotFromQDomNode( shot, sh );
                    rasters.push_back( OOCRaster(imgFile,shot,rasters.size()) );
                }

				raster=raster.nextSibling();
			}
		}
		node = node.nextSibling();
	}

	QDir::setCurrent(tmpDir.absolutePath());
	qf.close();
	return true;
}


void FilterOutputOpticalFlowPlugin::saveXMLProject( const QString &filename,
                                                    MeshModel *mm,
                                                    RasterFaceMap &rpatches,
                                                    QMap<int,QVector<int>> &validPairs )
{
    std::ofstream xmlFile( filename.toAscii() );

    xmlFile << "<!DOCTYPE RegProjectML>" << std::endl;
    xmlFile << "<project>" << std::endl;

    xmlFile << " <Device path=\""
            << mm->shortName().toStdString()
            << "\" aligned=\"0\" type=\"GlModelWidget\" numPoints=\"0\" id=\"0\"/>"
            << std::endl;

    for( RasterFaceMap::iterator r=rpatches.begin(); r!=rpatches.end(); ++r )
    {
        vcg::Shotf &shot = r.key()->shot;
        
        xmlFile << " <Device path=\""
                << r.key()->shortName().toStdString()
                << "\" aligned=\"1\" type=\"GlImageWidget\" numPoints=\"0\" id=\""
                << r.key()->id + 1
                << "\">"
                << std::endl;

        vcg::Matrix44f rot = shot.Extrinsics.Rot();
        vcg::Point3f tra = -shot.Extrinsics.Tra();

        xmlFile << "  <CamParam SimRot=\"";
        for( int i=0; i<4; ++i )
            for( int j=0; j<4; ++j )
                xmlFile << rot[i][j] << " ";
        xmlFile << "\" Focal=\""
                << shot.Intrinsics.FocalMm
                << "\" Center=\""
                << shot.Intrinsics.CenterPx.X() << " " << shot.Intrinsics.CenterPx.Y()
                << "\" LensDist=\""
                << shot.Intrinsics.k[0] << " " << shot.Intrinsics.k[1]
                << "\" ScaleCorr=\"1\" Viewport=\""
                << shot.Intrinsics.ViewportPx.X() << " " << shot.Intrinsics.ViewportPx.Y()
                << "\" ScaleF=\""
                << shot.Intrinsics.PixelSizeMm.X() << " " << shot.Intrinsics.PixelSizeMm.Y()
                << "\" SimTra=\""
                << tra.X() << " " << tra.Y() << " " << tra.Z() << " 1"
                << "\"/>"
                << std::endl;

        for( QVector<int>::iterator id2=validPairs[r.key()->id+1].begin(); id2!=validPairs[r.key()->id+1].end(); ++id2 )
        {
            xmlFile << "  <Projection path=\""
                    << QString().sprintf("rectified_%03i_%03i.png", *id2, r.key()->id+1 ).toStdString()
                    << "\" id=\""
                    << *id2
                    << "\"/>"
                    << std::endl;
        }

        xmlFile << " </Device>" << std::endl;
    }

    xmlFile << "</project>" << std::endl;
    xmlFile.close();
}


void FilterOutputOpticalFlowPlugin::getNeighbors( CVertexO *v,
                                                  NeighbSet &neighb ) const
{
    vcg::face::Pos<CFaceO> p( v->VFp(), v ), ori = p;
    do
    {
        neighb.insert( p.F() );
        p.FlipF();
        p.FlipE();
    } while( ori != p );
}


void FilterOutputOpticalFlowPlugin::getNeighbors( CFaceO *f,
                                                  NeighbSet &neighb ) const
{
    getNeighbors( f->V(0), neighb );
    getNeighbors( f->V(1), neighb );
    getNeighbors( f->V(2), neighb );
}


void FilterOutputOpticalFlowPlugin::expands( Patch &patch,
                                             int nbGrows )
{
    patch.boundary.clear();
    patch.bWeight.clear();


    // Mark all faces that belong to the current patch as "VISITED".
    for( FaceVec::iterator f=patch.faces.begin(); f!=patch.faces.end(); ++f )
        (*f)->SetV();


    // Get vertices that belong to the boundary of the patch by checking all triangle edges.
    std::set<CVertexO*> candidates;

    for( FaceVec::iterator f=patch.faces.begin(); f!=patch.faces.end(); ++f )
    {
        vcg::face::Pos<CFaceO> pos( *f, (*f)->V(0) );
        for( int i=0; i<3; ++i )
        {
            if( pos.FFlip() && !pos.FFlip()->IsV() )
            {
                candidates.insert( pos.V() );
                candidates.insert( pos.VFlip() );
            }
            pos.FlipV();
            pos.FlipE();
        }
    }


    // For each round of region growing...
    for( int n=0; n<nbGrows; ++n )
    {
        int k = patch.boundary.size();

        // For each vertex marked as belonging to the patch boundary, its 1-ring neighborhood is added
        // to the patch neighborhood and marked as visited.
        for( std::set<CVertexO*>::iterator v=candidates.begin(); v!=candidates.end(); ++v )
        {
            NeighbSet neighb;
            getNeighbors( *v, neighb );
            for( NeighbSet::iterator nn=neighb.begin(); nn!=neighb.end(); ++nn )
                if( !(*nn)->IsV() )
                {
                    (*nn)->SetV();
                    patch.boundary.push_back( *nn );
                }
        }

        // Vertices of the new boundary are recovered from the triangles that have juste been extracted
        // in order to prepare the next region growing round.
        for( candidates.clear(); k<patch.boundary.size(); ++k )
        {
            vcg::face::Pos<CFaceO> pos( patch.boundary[k], patch.boundary[k]->V(0) );
            for( int i=0; i<3; ++i )
            {
                if( pos.FFlip() && !pos.FFlip()->IsV() )
                {
                    candidates.insert( pos.V() );
                    candidates.insert( pos.VFlip() );
                }
                pos.FlipV();
                pos.FlipE();
            }
        }
    }


    // Unmark all marked faces, for other futur processing.
    for( FaceVec::iterator f=patch.faces.begin(); f!=patch.faces.end(); ++f )
        (*f)->ClearV();

    for( FaceVec::iterator f=patch.boundary.begin(); f!=patch.boundary.end(); ++f )
        (*f)->ClearV();
}


void FilterOutputOpticalFlowPlugin::setupMeshVBO()
{
    m_MeshVBO.Create();

    vcg::Point3f *vertexData = new vcg::Point3f [ m_Mesh->vn ];
    for( int v=0; v<m_Mesh->vn; ++v )
        vertexData[v] = m_Mesh->vert[v].P();
    m_MeshVBO.Vertex.LoadData( GL_STATIC_DRAW, vertexData, m_Mesh->vn );
    delete [] vertexData;

    GLuint *indexData = new GLuint [ 3*m_Mesh->fn ];
    for( int f=0, n=0; f<m_Mesh->fn; ++f )
        for( int v=0; v<3; ++v, ++n )
            indexData[n] = m_Mesh->face[f].V(v) - &m_Mesh->vert[0];
    m_MeshVBO.LoadIndices( GL_STATIC_DRAW, indexData, 3*m_Mesh->fn );
    delete [] indexData;
}


void FilterOutputOpticalFlowPlugin::setupShader( GPU::Shader &shader )
{
    std::string logs;
    if( !loadShader(shader,"reproj",&logs) )
        qWarning( (__FUNCTION__": "+logs).c_str() );
}


void FilterOutputOpticalFlowPlugin::shadowTextureMatrices( OOCRaster *rr,
                                                           vcg::Matrix44f &proj,
                                                           vcg::Matrix44f &pose,
                                                           vcg::Matrix44f &shadowProj )
{
    // Recover the view frustum of the current raster.
    float zNear, zFar;
    GlShot< vcg::Shot<float> >::GetNearFarPlanes( rr->shot, m_Mesh->bbox, zNear, zFar );
    if( zNear < 0.0001f )
        zNear = 0.1f;
    if( zFar < zNear )
        zFar = zNear + 1000.0f;

    float l, r, b, t, focal;
    rr->shot.Intrinsics.GetFrustum( l, r, b, t, focal );


    // Compute from the frustum values the camera projection matrix.
    proj.SetZero();
    proj[0][0] = 2.0f*focal / (r-l);
    proj[2][0] = (r+l) / (r-l);
    proj[1][1] = 2.0f*focal / (t-b);
    proj[2][1] = (t+b) / (t-b);
    proj[2][2] = (zNear+zFar) / (zNear-zFar);
    proj[3][2] = 2.0f*zNear*zFar / (zNear-zFar);
    proj[2][3] = -1.0f;


    // Extract the pose matrix from the current raster.
    pose = vcg::Transpose( rr->shot.GetWorldToExtrinsicsMatrix() );


    // Define the bias matrix that will enable to go from clipping space to texture space.
    const float biasMatData[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 0.5f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 0.5f, 0.0f,
                                    0.5f, 0.5f, 0.5f, 1.0f };

    shadowProj = pose * proj * vcg::Matrix44f(biasMatData);
}


void FilterOutputOpticalFlowPlugin::setupShadowAndColorTextures( GPU::Texture2D &shadowMap,
                                                                 GPU::Texture2D &colorMap,
                                                                 OOCRaster *rr )
{
    const vcg::Point2i vp = rr->shot.Intrinsics.ViewportPx;
    glPushAttrib( GL_TEXTURE_BIT );


    // Create and initialize the OpenGL texture object used to store the shadow map.
    if( !shadowMap.IsInstantiated() ||
        shadowMap.Width() !=vp.X()  ||
        shadowMap.Height()!=vp.Y()  )
    {
        shadowMap.Create( GL_DEPTH_COMPONENT,
                          vp.X(),
                          vp.Y(),
                          GL_DEPTH_COMPONENT,
                          GL_INT,
                          NULL );

        shadowMap.SetFiltering( GL_NEAREST );
        shadowMap.SetParam( GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE );
        shadowMap.SetParam( GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL );
        shadowMap.SetParam( GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY );
    }


    // Loads the raster to the GPU as a texture image.
    rr->bind();
    QImage &img = rr->plane->image;
    GLubyte *rasterData = new GLubyte [ 3*img.width()*img.height() ];
    for( int y=img.height()-1, n=0; y>=0; --y )
        for( int x=0; x<img.width(); ++x, n+=3 )
        {
            QRgb p = img.pixel(x,y);
            rasterData[n+0] = qRed  (p);
            rasterData[n+1] = qGreen(p);
            rasterData[n+2] = qBlue (p);
        }

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    colorMap.Create( GL_RGB, img.width(), img.height(), 1, GL_RGB, GL_UNSIGNED_BYTE, rasterData );
    colorMap.SetFiltering( GL_LINEAR );

    delete [] rasterData;
    rr->unbind();


    glPopAttrib();
}


void FilterOutputOpticalFlowPlugin::paintShadowTexture( GPU::Texture2D &shadowMap,
                                                        vcg::Matrix44f &proj,
                                                        vcg::Matrix44f &pose )
{
    glPushAttrib( GL_CURRENT_BIT      |
                  GL_DEPTH_BUFFER_BIT |
                  GL_VIEWPORT_BIT     |
                  GL_POLYGON_BIT      |
                  GL_TRANSFORM_BIT    );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_POLYGON_OFFSET_FILL );
    glPolygonOffset( 2.0f, 2.0f );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadMatrixf( proj.V() );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadMatrixf( pose.V() );

    GPU::FrameBuffer fbuffer( shadowMap.Width(), shadowMap.Height() );
    fbuffer.Attach( GL_DEPTH_ATTACHMENT, shadowMap );
    fbuffer.Bind();

    glClear( GL_DEPTH_BUFFER_BIT );
    m_MeshVBO.Bind();
    m_MeshVBO.DrawElements( GL_TRIANGLES, 0, 3*m_Mesh->fn );
    m_MeshVBO.Unbind();

    fbuffer.Unbind();

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();

    glPopAttrib();
}


void FilterOutputOpticalFlowPlugin::retroProjection( RasterFaceMap &rpatches,
                                                     float coverageThreshold,
                                                     QMap<int,QVector<int>> &validPairs )
{
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    GPU::Texture2D shadowMap;
    GPU::Texture2D colorMap;
    GPU::Shader reprojShader;

    setupMeshVBO();
    setupShader( reprojShader );
    validPairs.clear();

    for( RasterFaceMap::iterator rproj=rpatches.begin(); rproj!=rpatches.end(); ++rproj )
    {
        vcg::Matrix44f proj, pose, shadowProj;
        shadowTextureMatrices( rproj.key(), proj, pose, shadowProj );
        setupShadowAndColorTextures( shadowMap, colorMap, rproj.key() );
        paintShadowTexture( shadowMap, proj, pose );

        for( RasterFaceMap::iterator rref=rpatches.begin(); rref!=rpatches.end(); ++rref )
            if( rref != rproj )
            {
                const vcg::Point2i vp = rref.key()->shot.Intrinsics.ViewportPx;

                // Create the framebuffer for the offscreen rendering.
                GPU::FrameBuffer fbuffer( vp.X(), vp.Y() );
                fbuffer.Attach( GL_COLOR_ATTACHMENT0, GL_RGBA );
                fbuffer.Attach( GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT );
                fbuffer.Bind();

                // Set up the OpenGL state and viewpoint matrices.
                glEnable( GL_DEPTH_TEST );
                glDisable( GL_POLYGON_OFFSET_FILL );

                float zNear, zFar;
                GlShot< vcg::Shot<float> >::GetNearFarPlanes( rref.key()->shot, m_Mesh->bbox, zNear, zFar );
                if( zNear < 0.0001f )
                    zNear = 0.1f;
                if( zFar < zNear )
                    zFar = zNear + 1000.0f;

                GlShot< vcg::Shot<float> >::SetView( rref.key()->shot, zNear, zFar );

                // Perform the rendering of the surface patch associated to the current reference view.
                // The other pictures are then reprojected onto it.
                glClearColor( 1.0f, 0.0f, 1.0f, 0.0f );
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                shadowMap.Bind( 0 );
                colorMap.Bind( 1 );
                reprojShader.Bind();
                reprojShader.SetSampler( "u_ShadowMap" , 0 );
                reprojShader.SetSampler( "u_ColorMap"  , 1 );
                reprojShader.SetUniform( "u_ShadowProj", shadowProj.V() );
                m_MeshVBO.Bind();
                for( FaceVec::iterator f=rref->begin(); f!=rref->end(); ++f )
                    m_MeshVBO.DrawElements( GL_TRIANGLES, 3*(*f - &m_Mesh->face[0]), 3 );
                m_MeshVBO.Unbind();
                reprojShader.Unbind();
                colorMap.Unbind();
                shadowMap.Unbind();

                GlShot< vcg::Shot<float> >::UnsetView();
                fbuffer.Unbind();

                // Read back the content of the framebuffer.
                GLubyte *buffer = new GLubyte [ 4*vp.X()*vp.Y() ];
                glPixelStorei( GL_PACK_ALIGNMENT, 1 );
                fbuffer.DumpTo( GL_COLOR_ATTACHMENT0, buffer, GL_RGBA, GL_UNSIGNED_BYTE );

                // Counts the the number of pixels covered by the whole surface patch, and the number of pixels
                // covered by the reprojection of the other image onto this patch.
                unsigned int coverage = 0, reprojected = 0;
                for( int n=0; n<4*vp.X()*vp.Y(); n+=4 )
                    if( buffer[n+3] == 255 )
                    {
                        coverage ++;
                        if( buffer[n]!=255 && buffer[n+1]!=0 && buffer[n+2]!=255 )
                            reprojected ++;
                    }

                // If the ratio of reprojected pixels is below a given threshold, the current picture pair is ignored.
                if( (float)reprojected/coverage > coverageThreshold )
                {
                    QString filename = QString().sprintf("rectified_%03i_%03i.png", rproj.key()->id+1, rref.key()->id+1 );
                    QImage img( vp.X(), vp.Y(), QImage::Format_RGB888 );
                    for( int y=vp.Y()-1, n=0; y>=0; --y )
                        for( int x=0; x<vp.X(); ++x, n+=4 )
                            img.setPixel( x, y, qRgb(buffer[n],buffer[n+1],buffer[n+2]) );
                    img.save( filename );
                    validPairs[rref.key()->id+1].push_back( rproj.key()->id+1 );
                }

                delete [] buffer;
            }
    }

    m_MeshVBO.Release();
    glPopAttrib();
}




Q_EXPORT_PLUGIN(FilterOutputOpticalFlowPlugin)
