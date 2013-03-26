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

#include "filter_img_patch_param.h"
#include <QtGui>
#include <wrap/gl/shot.h>
#include <vcg/space/rect_packer.h>
#include "VisibleSet.h"
#include "VisibilityCheck.h"
#include "TexturePainter.h"
#include <cmath>




FilterImgPatchParamPlugin::FilterImgPatchParamPlugin() : m_Context(NULL)
{
    typeList << FP_PATCH_PARAM_ONLY
             << FP_PATCH_PARAM_AND_TEXTURING
             << FP_RASTER_VERT_COVERAGE
             << FP_RASTER_FACE_COVERAGE;

	foreach( FilterIDType tt , types() )
		actionList << new QAction(filterName(tt), this);
}


FilterImgPatchParamPlugin::~FilterImgPatchParamPlugin()
{
    delete m_Context;
    m_Context = NULL;
}


QString FilterImgPatchParamPlugin::filterName( FilterIDType id ) const
{
    switch( id )
    {
        case FP_PATCH_PARAM_ONLY:  return QString( "Parameterization from registered rasters" );
        case FP_PATCH_PARAM_AND_TEXTURING:  return QString( "Parameterization + texturing from registered rasters" );
        case FP_RASTER_VERT_COVERAGE:  return QString( "Quality from raster coverage (Vertex)" );
        case FP_RASTER_FACE_COVERAGE:  return QString( "Quality from raster coverage (Face)" );
        default: assert(0); return QString();
    }
}


QString FilterImgPatchParamPlugin::filterInfo( FilterIDType id ) const
{
    switch( id )
    {
        case FP_PATCH_PARAM_ONLY:  return QString( "The mesh is parameterized by creating some patches that correspond to projection of portions of surfaces onto the set of registered rasters.");
        case FP_PATCH_PARAM_AND_TEXTURING:	return QString("The mesh is parameterized and textured by creating some patches that correspond to projection of portions of surfaces onto the set of registered rasters.");
        case FP_RASTER_VERT_COVERAGE:  return QString( "Compute a quality value representing the number of images into which each vertex of the active mesh is visible." );
        case FP_RASTER_FACE_COVERAGE:  return QString( "Compute a quality value representing the number of images into which each face of the active mesh is visible." );
        default: assert(0); return QString();
    }
}


int FilterImgPatchParamPlugin::getRequirements( QAction *act )
{
    switch( ID(act) )
    {
        case FP_PATCH_PARAM_ONLY:
        case FP_PATCH_PARAM_AND_TEXTURING:  return MeshModel::MM_WEDGTEXCOORD | MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTFACETOPO;
        case FP_RASTER_VERT_COVERAGE:  return MeshModel::MM_VERTQUALITY;
        case FP_RASTER_FACE_COVERAGE:  return MeshModel::MM_FACEQUALITY;
        default:  assert(0); return 0;
    }
}


MeshFilterInterface::FilterClass FilterImgPatchParamPlugin::getClass( QAction *act )
{
    switch( ID(act) )
    {
        case FP_PATCH_PARAM_ONLY:
        case FP_PATCH_PARAM_AND_TEXTURING:  return Texture;
        case FP_RASTER_VERT_COVERAGE:
        case FP_RASTER_FACE_COVERAGE:  return Quality;
        default:  assert(0); return MeshFilterInterface::Generic;
    }
}


//int FilterImgPatchParamPlugin::postCondition( QAction *act ) const
//{
//    switch( ID(act) )
//    {
//        case FP_PATCH_PARAM_ONLY:
//        case FP_PATCH_PARAM_AND_TEXTURING:  return MeshModel::MM_WEDGTEXCOORD;
//        case FP_RASTER_COVERAGE:
//        {
//            return QString(  );
//        }
//        default:  assert(0); return 0;
//    }
//}


void FilterImgPatchParamPlugin::initParameterSet( QAction *act,
                                                  MeshDocument &/*md*/,
                                                  RichParameterSet &par )
{
    switch( ID(act) )
    {
        case FP_PATCH_PARAM_AND_TEXTURING:
        {
            par.addParam( new RichInt( "textureSize",
                                       1024,
                                       "Texture size",
                                       "Specifies the dimension of the generated texture" ) );
            par.addParam( new RichString( "textureName",
                                          "texture.png",
                                          "Texture name",
                                          "Specifies the name of the file into which the texture image will be saved" ) );
            par.addParam( new RichBool( "colorCorrection",
                                        true,
                                        "Color correction",
                                        "If true, the final texture is corrected so as to ensure seamless transitions" ) );
            par.addParam( new RichInt( "colorCorrectionFilterSize",
                                       1,
                                       "Color correction filter",
                                       "Highest values increase the robustness of the color correction process in the case of strong image-to-geometry misalignments" ) );
        }
        case FP_PATCH_PARAM_ONLY:
        {
            par.addParam( new RichBool( "useDistanceWeight",
                                        false,
                                        "Use distance weight",
                                        "Includes a weight accounting for the distance to the camera during the computation of reference images" ) );
            par.addParam( new RichBool( "useImgBorderWeight",
                                        false,
                                        "Use image border weight",
                                        "Includes a weight accounting for the distance to the image border during the computation of reference images" ) );
            par.addParam( new RichBool( "useAlphaWeight",
                                        false,
                                        "Use image alpha weight",
                                        "If true, alpha channel of the image is used as additional weight. In this way it is possible to mask-out parts of the images that should not be projected on the mesh. Please note this is not a transparency effect, but just influences the weigthing between different images" ) );
            par.addParam( new RichBool( "cleanIsolatedTriangles",
                                        true,
                                        "Clean isolated triangles",
                                        "Remove all patches compound of a single triangle by aggregating them to adjacent patches" ) );
            par.addParam( new RichBool( "stretchingAllowed",
                                        false,
                                        "UV stretching",
                                        "If true, texture coordinates are stretched so as to cover the full interval [0,1] for both directions" ) );
            par.addParam( new RichInt( "textureGutter",
                                       4,
                                       "Texture gutter",
                                       "Extra boundary to add to each patch before packing in texture space (in pixels)" ) );
            break;
        }
        case FP_RASTER_VERT_COVERAGE:
        case FP_RASTER_FACE_COVERAGE:
        {
            par.addParam( new RichBool( "normalizeQuality",
                                        false,
                                        "Normalize",
                                        "Rescale quality values to the range [0,1]" ) );
            break;
        }
    }
}


bool FilterImgPatchParamPlugin::applyFilter( QAction *act,
                                             MeshDocument &md,
                                             RichParameterSet &par,
                                             vcg::CallBackPos * /*cb*/ )
{
	if (vcg::tri::Clean<CMeshO>::CountNonManifoldEdgeFF(md.mm()->cm)>0)
	{
		errorMessage = "Mesh has some not 2-manifold faces, this filter requires manifoldness"; // text
		return false; // can't continue, mesh can't be processed
	}
	vcg::tri::Allocator<CMeshO>::CompactFaceVector(md.mm()->cm);
	vcg::tri::Allocator<CMeshO>::CompactVertexVector(md.mm()->cm);
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(md.mm()->cm);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(md.mm()->cm);
	glContext->makeCurrent();
	if( glewInit() != GLEW_OK )
		return false;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

    delete m_Context;
    m_Context = new glw::Context();
    m_Context->acquire();

    if( !VisibilityCheck::GetInstance(*m_Context) )
        return false;
    VisibilityCheck::ReleaseInstance();



    bool retValue = true;

    CMeshO &mesh = md.mm()->cm;

    std::list<vcg::Shotf> initialShots;
    QList<RasterModel*> activeRasters;
    foreach( RasterModel *rm, md.rasterList )
    {
        initialShots.push_back( rm->shot );
        rm->shot.ApplyRigidTransformation( vcg::Inverse(mesh.Tr) );
        if( rm->visible )
          activeRasters.push_back( rm );
    }

    if( activeRasters.empty() )
      return false;


    switch( ID(act) )
    {
        case FP_PATCH_PARAM_ONLY:
        {
            RasterPatchMap patches;
            PatchVec nullPatches;
            patchBasedTextureParameterization( patches,
                                               nullPatches,
                                               mesh,
                                               activeRasters,
                                               par );

			break;
		}
		case FP_PATCH_PARAM_AND_TEXTURING:
		{
			QString texName = par.getString( "textureName" ).simplified();
			int pathEnd = std::max( texName.lastIndexOf('/'), texName.lastIndexOf('\\') );
			if( pathEnd != -1 )
				texName = texName.right( texName.size()-pathEnd-1 );

            if( (retValue = texName.size()!=0) )
            {
                RasterPatchMap patches;
                PatchVec nullPatches;
                patchBasedTextureParameterization( patches,
                                                   nullPatches,
                                                   mesh,
                                                   activeRasters,
                                                   par );

                TexturePainter painter( *m_Context, par.getInt("textureSize") );
                if( (retValue = painter.isInitialized()) )
                {
                    QTime t; t.start();
                    painter.paint( patches );
                    if( par.getBool("colorCorrection") )
                        painter.rectifyColor( patches, par.getInt("colorCorrectionFilterSize") );
                    Log( "TEXTURE PAINTING: %.3f sec.", 0.001f*t.elapsed() );

                    QImage tex = painter.getTexture();
                    if( tex.save(texName) )
                    {
                        mesh.textures.clear();
                        mesh.textures.push_back( texName.toStdString() );
                    }
                }
            }

			break;
		}
		case FP_RASTER_VERT_COVERAGE:
		{
			VisibilityCheck &visibility = *VisibilityCheck::GetInstance( *m_Context );
			visibility.setMesh( &mesh );

            for( CMeshO::VertexIterator vi=mesh.vert.begin(); vi!=mesh.vert.end(); ++vi )
                vi->Q() = 0.0f;

            foreach( RasterModel *rm, activeRasters )
            {
                visibility.setRaster( rm );
                visibility.checkVisibility();
                for( CMeshO::VertexIterator vi=mesh.vert.begin(); vi!=mesh.vert.end(); ++vi )
                    if( visibility.isVertVisible(vi) )
                        vi->Q() += 1.0f;
            }

            if( par.getBool("normalizeQuality") )
            {
                const float normFactor = 1.0f / md.rasterList.size();
                for( CMeshO::VertexIterator vi=mesh.vert.begin(); vi!=mesh.vert.end(); ++vi )
                    vi->Q() *= normFactor;
            }

            break;
        }
        case FP_RASTER_FACE_COVERAGE:
        {
            VisibilityCheck &visibility = *VisibilityCheck::GetInstance( *m_Context );
            visibility.setMesh( &mesh );

            for( CMeshO::FaceIterator fi=mesh.face.begin(); fi!=mesh.face.end(); ++fi )
                fi->Q() = 0.0f;

            foreach( RasterModel *rm, activeRasters )
            {
                visibility.setRaster( rm );
                visibility.checkVisibility();
                for( CMeshO::FaceIterator fi=mesh.face.begin(); fi!=mesh.face.end(); ++fi )
                    if( visibility.isFaceVisible(fi) )
                        fi->Q() += 1.0f;
            }

            if( par.getBool("normalizeQuality") )
            {
                const float normFactor = 1.0f / md.rasterList.size();
                for( CMeshO::FaceIterator fi=mesh.face.begin(); fi!=mesh.face.end(); ++fi )
                    fi->Q() *= normFactor;
            }

            break;
        }
    }


    foreach( RasterModel *rm, md.rasterList )
    {
        rm->shot = *initialShots.begin();
        initialShots.erase( initialShots.begin() );
    }

    VisibilityCheck::ReleaseInstance();


    delete m_Context;
    m_Context = NULL;

    glPopAttrib();
    glContext->doneCurrent();


    return retValue;
}


void FilterImgPatchParamPlugin::getNeighbors( CVertexO *v,
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


void FilterImgPatchParamPlugin::getNeighbors( CFaceO *f,
                                              NeighbSet &neighb ) const
{
    getNeighbors( f->V(0), neighb );
    getNeighbors( f->V(1), neighb );
    getNeighbors( f->V(2), neighb );
}


void FilterImgPatchParamPlugin::boundaryOptimization( CMeshO &mesh,
                                                      VisibleSet &faceVis,
                                                      bool mostFrontFacing )
{
    std::set<CFaceO*> toOptim;


    // Collects the faces belonging to boundaries (namely faces for which at least one adjacent
    // face has a different reference image), so as to initialize the optimization step.
    for( CMeshO::FaceIterator f=mesh.face.begin(); f!=mesh.face.end(); ++f )
        f->ClearV();

    for( CMeshO::FaceIterator f=mesh.face.begin(); f!=mesh.face.end(); ++f )
    {
        // Checks each of the three edges of the current face. If the opposite face has a different
        // reference image, the 1-ring neighborhood of this edge is added to the processing queue.
        vcg::face::Pos<CFaceO> p( &*f, f->V(0) );
        for( int i=0; i<3; ++i )
        {
            const CFaceO *f2 = p.FFlip();
            if( f2 && !f2->IsV() )
                if( faceVis[f2].ref() != faceVis[f].ref() )
                {
                    NeighbSet neighb;
                    getNeighbors( p.V(), neighb );
                    getNeighbors( p.VFlip(), neighb );
                    for( NeighbSet::iterator n=neighb.begin(); n!=neighb.end(); ++n )
                        toOptim.insert( *n );
                }
            p.FlipV();
            p.FlipE();
        }
        f->SetV();
    }


    // The optimization is a greedy approach that changes the reference image of a face in order to reduce
    // the number of different images adjacent to that face.
    while( !toOptim.empty() )
    {
        // Extract a face from the queue.
        CFaceO *f = *toOptim.begin();
        toOptim.erase( toOptim.begin() );


        // Counts how many times appears each reference image in the 1-ring neighborhood of this face.
        NeighbSet neighb;
        getNeighbors( f, neighb );

        QMap<RasterModel*,int> neighbRefCount;

        for( NeighbSet::iterator n=neighb.begin(); n!=neighb.end(); ++n )
            if( *n && *n!=f )
            {
                RasterModel *neighbRef = faceVis[*n].ref();
                QMap<RasterModel*,int>::iterator nFound = neighbRefCount.find( neighbRef );
                if( nFound == neighbRefCount.end() )
                    neighbRefCount[neighbRef] = 1;
                else
                    (*nFound) ++;
            }


        if( mostFrontFacing )
        {
            // Look for the one that appears the most, and that belongs to the list of visible rasters
            // of the considered face.
            std::vector<RasterModel*> appearsMost;
            int nbMaxAppear = 0;

            for( QMap<RasterModel*,int>::iterator n=neighbRefCount.begin(); n!=neighbRefCount.end(); ++n )
                if( n.value()>=nbMaxAppear && faceVis[f].contains(n.key()) )
                {
                    if( n.value() > nbMaxAppear )
                        appearsMost.clear();

                    nbMaxAppear = n.value();
                    appearsMost.push_back( n.key() );
                }


            // If multiple neighboring reference images have the same number of occurences, the one with the highest
            // weight with respect to the current face is chosen.
            RasterModel *candidate = faceVis[f].ref();

            if( appearsMost.size() > 1 )
            {
                float maxWeight = -std::numeric_limits<float>::max();
                for( std::vector<RasterModel*>::iterator r=appearsMost.begin(); r!=appearsMost.end(); ++r )
                {
                    float weight = faceVis.getWeight( *r, *f );
                    if( weight > maxWeight )
                    {
                        maxWeight = weight;
                        candidate = *r;
                    }
                }
            }
            else if( appearsMost.size() == 1 )
                candidate = appearsMost.front();


            // If the reference image of the current face is different from the candidate image, change it accordingly.
            // Triangles of its neighborhood are reintroduced in the queue only if their reference images is different
            // from the new one.
            if( candidate != faceVis[f].ref() )
            {
                faceVis[f].setRef( candidate );
                for( NeighbSet::iterator n=neighb.begin(); n!=neighb.end(); ++n )
                    if( *n && *n!=f && faceVis[*n].ref()!=candidate )
                        toOptim.insert( *n );
            }
        }
        else
        {
            // Look for the one that appears the most, and that belongs to the list of visible rasters
            // of the considered face.
            RasterModel *appearsMost = faceVis[f].ref();
            int nbMaxAppear = 0;

            for( QMap<RasterModel*,int>::iterator n=neighbRefCount.begin(); n!=neighbRefCount.end(); ++n )
                if( n.value()>nbMaxAppear && faceVis[f].contains(n.key()) )
                {
                    nbMaxAppear = n.value();
                    appearsMost = n.key();
                }


            // If the reference image of the current face is different from the candidate image, change it accordingly.
            // Triangles of its neighborhood are reintroduced in the queue only if their reference images is different
            // from the new one.
            if( appearsMost != faceVis[f].ref() )
            {
                faceVis[f].setRef( appearsMost );
                for( NeighbSet::iterator n=neighb.begin(); n!=neighb.end(); ++n )
                    if( *n && *n!=f && faceVis[*n].ref()!=appearsMost )
                        toOptim.insert( *n );
            }
        }
    }
}


int FilterImgPatchParamPlugin::cleanIsolatedTriangles( CMeshO &mesh,
                                                       VisibleSet &faceVis )
{
    int nbTrianglesChanged = 0;


    // For each triangle T...
    for( CMeshO::FaceIterator f=mesh.face.begin(); f!=mesh.face.end(); ++f )
    {
        // Each reference image in the immediate edge neighborhood of T is gathered and counted.
        QMap<RasterModel*,int> neighb;
        for( int i=0; i<3; ++i )
            if( f->FFp(i) )
            {
                RasterModel *r = faceVis[ f->FFp(i) ].ref();
                if( neighb.contains(r) )
                    neighb[r] ++;
                else
                    neighb[r] = 1;
            }

        // If the reference image of T doesn't appear in its neighborhood, it seems that T is isolated.
        // In that case, the reference image that appears the most in its neighborhood is chosen as
        // the new reference image of T.
        if( !neighb.contains(faceVis[f].ref()) )
        {
            RasterModel *appearsMost = NULL;
            int nAppearanceMax = 0;

            for( QMap<RasterModel*,int>::iterator n=neighb.begin(); n!=neighb.end(); ++n )
                if( n.value() > nAppearanceMax )
                {
                    appearsMost = n.key();
                    nAppearanceMax = n.value();
                }

            if( appearsMost )
            {
                faceVis[f].setRef( appearsMost );
                nbTrianglesChanged ++;
            }
        }
    }


    return nbTrianglesChanged;
}


int FilterImgPatchParamPlugin::extractPatches( RasterPatchMap &patches,
                                               PatchVec &nullPatches,
                                               CMeshO &mesh,
                                               VisibleSet &faceVis,
                                               QList<RasterModel*> &rasterList )
{
    int nbPatches = 0;

    foreach( RasterModel *rm, rasterList )
        patches[rm] = PatchVec();

    for( CMeshO::FaceIterator fSeed=mesh.face.begin(); fSeed!=mesh.face.end(); ++fSeed )
        if( fSeed->IsV() )
        {
            std::queue<CFaceO*> seedFillQueue;
            seedFillQueue.push( &*fSeed );
            fSeed->ClearV();

            Patch patch;
            patch.ref = faceVis[fSeed].ref();

            do
            {
                CFaceO *f = seedFillQueue.front();
                seedFillQueue.pop();

                patch.faces.push_back( f );

                for( int i=0; i<3; ++i )
                {
                    CFaceO *fAdj = f->FFp(i);
                    if( fAdj && fAdj->IsV() && faceVis[fAdj].ref()==patch.ref )
                    {
                        fAdj->ClearV();
                        seedFillQueue.push( fAdj );
                    }
                }
            } while( !seedFillQueue.empty() );

            if( patch.ref )
            {
                patches[patch.ref].push_back( patch );
                ++ nbPatches;
            }
            else
                nullPatches.push_back( patch );
        }

    return nbPatches;
}


void FilterImgPatchParamPlugin::constructPatchBoundary( Patch &p,
                                                        VisibleSet &faceVis )
{
    for( std::vector<CFaceO*>::iterator f=p.faces.begin(); f!=p.faces.end(); ++f )
    {
        RasterModel *fRef = faceVis[*f].ref();
        vcg::face::Pos<CFaceO> pos( *f, (*f)->V(0) );

        for( int i=0; i<3; ++i )
        {
            const CFaceO *f2 = pos.FFlip();
            if( f2 && faceVis[f2].ref() && faceVis[f2].ref()!=fRef )
            {
                NeighbSet neighb;
                getNeighbors( pos.V(), neighb );
                getNeighbors( pos.VFlip(), neighb );
                for( NeighbSet::iterator n=neighb.begin(); n!=neighb.end(); ++n )
                    if( !(*n)->IsV() && faceVis[*n].ref()!=fRef )
                    {
                        p.boundary.push_back( *n );
                        (*n)->SetV();
                    }
            }
            pos.FlipV();
            pos.FlipE();
        }
    }

    for( std::vector<CFaceO*>::iterator f=p.boundary.begin(); f!=p.boundary.end(); ++f )
        (*f)->ClearV();
}


void FilterImgPatchParamPlugin::computePatchUV( CMeshO &mesh,
                                                RasterModel *rm,
                                                PatchVec &patches )
{
    // Recovers the view frustum of the current raster.
    float zNear, zFar;
    GlShot< vcg::Shot<float> >::GetNearFarPlanes( rm->shot, mesh.bbox, zNear, zFar );
    if( zNear < 0.0001f )
        zNear = 0.1f;
    if( zFar < zNear )
        zFar = zNear + 1000.0f;

    float l, r, b, t, focal;
    rm->shot.Intrinsics.GetFrustum( l, r, b, t, focal );

    // Computes the camera perspective projection matrix from the frustum values.
    vcg::Matrix44f camProj;
    camProj.SetZero();
    camProj[0][0] = 2.0f*focal / (r-l);
    camProj[0][2] = (r+l) / (r-l);
    camProj[1][1] = 2.0f*focal / (t-b);
    camProj[1][2] = (t+b) / (t-b);
    camProj[2][2] = (zNear+zFar) / (zNear-zFar);
    camProj[2][3] = 2.0f*zNear*zFar / (zNear-zFar);
    camProj[3][2] = -1.0f;

    vcg::Matrix44f cam2clip;
    cam2clip.SetZero();
    cam2clip[0][0] = cam2clip[0][3] = 0.5f * rm->shot.Intrinsics.ViewportPx.X();
    cam2clip[1][1] = cam2clip[1][3] = 0.5f * rm->shot.Intrinsics.ViewportPx.Y();
    cam2clip[2][2] = cam2clip[3][3] = 1.0f;

    // Computes the full transform that goes from the mesh local space to the camera clipping space.
    vcg::Matrix44f mesh2clip = cam2clip * camProj * rm->shot.GetWorldToExtrinsicsMatrix();

    for( PatchVec::iterator p=patches.begin(); p!=patches.end(); ++p )
    {
        // Resets the UV bounding box of the patch, and allocate the array containing the UV coordinates
        // for the boundary faces.
        p->bbox.SetNull();
        p->boundaryUV.clear();
        p->boundaryUV.reserve( p->boundary.size() );

        // Computes UV coordinates for internal patch faces, and update the bounding box accordingly.
        for( std::vector<CFaceO*>::iterator f=p->faces.begin(); f!=p->faces.end(); ++f )
            for( int i=0; i<3; ++i )
            {
                vcg::Point3f &vp = (*f)->V(i)->P();

                (*f)->WT(i).U() = mesh2clip.GetRow3(0)*vp + mesh2clip[0][3];
                (*f)->WT(i).V() = mesh2clip.GetRow3(1)*vp + mesh2clip[1][3];
                (*f)->WT(i).P() *= 1.0f / (mesh2clip.GetRow3(3)*vp + mesh2clip[3][3]);

                p->bbox.Add( (*f)->WT(i).P() );
            }

        // Computes UV coordinates for boundary patch faces, and update the bounding box accordingly.
        for( std::vector<CFaceO*>::iterator f=p->boundary.begin(); f!=p->boundary.end(); ++f )
        {
            TriangleUV fuv;
            for( int i=0; i<3; ++i )
            {
                vcg::Point3f &vp = (*f)->V(i)->P();

                fuv.v[i].U() = mesh2clip.GetRow3(0)*vp + mesh2clip[0][3];
                fuv.v[i].V() = mesh2clip.GetRow3(1)*vp + mesh2clip[1][3];
                fuv.v[i].P() *= 1.0f / (mesh2clip.GetRow3(3)*vp + mesh2clip[3][3]);

                p->bbox.Add( fuv.v[i].P() );
            }
            p->boundaryUV.push_back( fuv );
        }
    }
}


void FilterImgPatchParamPlugin::mergeOverlappingPatches( PatchVec &patches )
{
    if( patches.size() <= 1 )
        return;


    for( PatchVec::iterator p=patches.begin(); p!=patches.end(); ++p )
        p->valid = true;


    float globalGain = 0.0f;
    for( PatchVec::iterator p1=patches.begin(); p1!=patches.end(); ++p1 )
        if( p1->valid )
        {
            float maxOccupancyGain = -globalGain;
            PatchVec::iterator candidate = patches.end();

            for( PatchVec::iterator p2=patches.begin(); p2!=patches.end(); ++p2 )
                if( p2!=p1 && p2->valid && p2->bbox.Collide(p1->bbox) )
                {
                    vcg::Box2f boxMerge = p1->bbox;
                    boxMerge.Add( p2->bbox );
                    float occupancyGain = p1->bbox.Area() + p2->bbox.Area() - boxMerge.Area();

                    if( occupancyGain > maxOccupancyGain )
                    {
                        maxOccupancyGain = occupancyGain;
                        candidate = p2;
                    }
                }

            if( candidate != patches.end() )
            {
                p1->faces.insert( p1->faces.end(), candidate->faces.begin(), candidate->faces.end() );
                p1->boundary.insert( p1->boundary.end(), candidate->boundary.begin(), candidate->boundary.end() );
                p1->boundaryUV.insert( p1->boundaryUV.end(), candidate->boundaryUV.begin(), candidate->boundaryUV.end() );
                p1->bbox.Add( candidate->bbox );
                candidate->valid = false;
                globalGain += maxOccupancyGain;
            }
        }


    for( PatchVec::iterator p=patches.begin(); p!=patches.end(); )
        if( p->valid )
            ++ p;
        else
        {
            *p = patches.back();
            patches.pop_back();
        }
}


void FilterImgPatchParamPlugin::patchPacking( RasterPatchMap &patches,
                                              int textureGutter,
                                              bool allowUVStretching )
{
    std::vector<vcg::Box2f> patchRect;
    std::vector<vcg::Similarity2f> patchPackingTr;


    // Computes the foreseen texture edge length based on the total area covered by patches' boxes.
    float totalArea = 0;

    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
        for( PatchVec::iterator p=rp->begin(); p!=rp->end(); ++p )
        {
            p->bbox.Offset( vcg::Point2f(textureGutter,textureGutter) );
            patchRect.push_back( p->bbox );
            totalArea += p->bbox.Area();
        }

    if( patchRect.empty() )
        return;

    float edgeLen = std::sqrt( totalArea );


    // Performs the packing.
    vcg::Point2f coveredArea;
    vcg::RectPacker<float>::Pack( patchRect, vcg::Point2i(edgeLen,edgeLen), patchPackingTr, coveredArea );


    // Applies to the UV coordinates the transformations computed by the packing algorithm, as well as a scaling
    // so as to make them ranging the interval [0,1]x[0,1].
    float scaleU, scaleV;

    if( allowUVStretching )
    {
        scaleU = 1.0f / coveredArea.X();
        scaleV = 1.0f / coveredArea.Y();
    }
    else
        scaleU = scaleV = 1.0f / std::max( coveredArea.X(), coveredArea.Y() );

    int n = 0;
    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
        for( PatchVec::iterator p=rp->begin(); p!=rp->end(); ++p, ++n )
        {
            vcg::Similarity2f &tr = patchPackingTr[n];
            float c = std::cos( tr.rotRad );
            float s = std::sin( tr.rotRad );

            p->img2tex.SetIdentity();
            p->img2tex[0][0] =  c * tr.sca * scaleU;
            p->img2tex[0][1] = -s * tr.sca * scaleU;
            p->img2tex[0][3] =  tr.tra.X() * scaleU;
            p->img2tex[1][0] =  s * tr.sca * scaleV;
            p->img2tex[1][1] =  c * tr.sca * scaleV;
            p->img2tex[1][3] =  tr.tra.Y() * scaleV;

            for( std::vector<CFaceO*>::iterator f=p->faces.begin(); f!=p->faces.end(); ++f )
                for( int i=0; i<3; ++i )
                {
                    (*f)->WT(i).P() = tr * (*f)->WT(i).P();
                    (*f)->WT(i).U() *= scaleU;
                    (*f)->WT(i).V() *= scaleV;
                }

            for( std::vector<TriangleUV>::iterator f=p->boundaryUV.begin(); f!=p->boundaryUV.end(); ++f )
                for( int i=0; i<3; ++i )
                {
                    f->v[i].P() = tr * f->v[i].P();
                    f->v[i].U() *= scaleU;
                    f->v[i].V() *= scaleV;
                }
        }
}


void FilterImgPatchParamPlugin::patchBasedTextureParameterization( RasterPatchMap &patches,
                                                                   PatchVec &nullPatches,
                                                                   CMeshO &mesh,
                                                                   QList<RasterModel*> &rasterList,
                                                                   RichParameterSet &par )
{
    // Computes the visibility set for all mesh faces. It contains the set of all images
    // into which the face is visible, as well as a reference image, namely the one whith
    // the most orthogonal viewing angle.
    QTime t; t.start();
    int weightMask = VisibleSet::W_ORIENTATION;
    if( par.getBool("useDistanceWeight") )
        weightMask |= VisibleSet::W_DISTANCE;
    if( par.getBool("useImgBorderWeight") )
        weightMask |= VisibleSet::W_IMG_BORDER;
    if( par.getBool("useAlphaWeight") )
        weightMask |= VisibleSet::W_IMG_ALPHA;
    VisibleSet *faceVis = new VisibleSet( *m_Context, mesh, rasterList, weightMask );
    Log( "VISIBILITY CHECK: %.3f sec.", 0.001f*t.elapsed() );


    // Boundary optimization: the goal is to produce more regular boundaries between surface regions
    // associated to different reference images.
    t.start();
    boundaryOptimization( mesh, *faceVis, true );
    Log( "BOUNDARY OPTIMIZATION: %.3f sec.", 0.001f*t.elapsed() );


    // Incorporates patches compounds of only one triangles to one of their neighbours.
    if( par.getBool("cleanIsolatedTriangles") )
    {
        t.start();
        int triCleaned = cleanIsolatedTriangles( mesh, *faceVis );
        Log( "CLEANING ISOLATED TRIANGLES: %.3f sec.", 0.001f*t.elapsed() );
        Log( "  * %i triangles cleaned.", triCleaned );
    }


    // Recovers patches by extracting connected components of faces having the same reference image.
    t.start();
    float oldArea = computeTotalPatchArea( patches );
    int nbPatches = extractPatches( patches, nullPatches, mesh, *faceVis, rasterList );
    Log( "PATCH EXTRACTION: %.3f sec.", 0.001f*t.elapsed() );
    Log( "  * %i patches extracted, %i null patches.", nbPatches, nullPatches.size() );


    // Extends each patch so as to include faces that belong to the other side of its boundary.
    t.start();
    oldArea = computeTotalPatchArea( patches );
    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
        for( PatchVec::iterator p=rp->begin(); p!=rp->end(); ++p )
            constructPatchBoundary( *p, *faceVis );
    delete faceVis;
    Log( "PATCH EXTENSION: %.3f sec.", 0.001f*t.elapsed() );


    // Compute the UV coordinates of all patches by projecting them onto their reference images.
    // UV are then defined in image space, ranging from [0,0] to [w,h].
    t.start();
    oldArea = computeTotalPatchArea( patches );
    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
        computePatchUV( mesh, rp.key(), rp.value() );
    Log( "PATCHES UV COMPUTATION: %.3f sec.", 0.001f*t.elapsed() );


    // Merge patches so as to reduce the occupied texture area when their bounding boxes overlap.
    t.start();
    oldArea = computeTotalPatchArea( patches );
    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
        mergeOverlappingPatches( *rp );
    Log( "PATCH MERGING: %.3f sec.", 0.001f*t.elapsed() );
    Log( "  * Area reduction: %.1f%%.", 100.0f*computeTotalPatchArea(patches)/oldArea );
    Log( "  * Patches number reduced from %i to %i.", nbPatches, computePatchCount(patches) );


    // Patches' bounding boxes are packed in texture space. After this operation, boxes are still defined
    // in the space of their patches' reference images but UV coordinates are all defined in a common texture
    // space, ranging from [0,0] to [1,1].
    t.start();
        patchPacking( patches, par.getInt("textureGutter"), par.getBool("stretchingAllowed") );
    Log( "PATCH TEXTURE PACKING: %.3f sec.", 0.001f*t.elapsed() );


    // Clear the UV coordinates for patches that are not visible in any image.
    for( PatchVec::iterator p=nullPatches.begin(); p!=nullPatches.end(); ++p )
        for( std::vector<CFaceO*>::iterator f=p->faces.begin(); f!=p->faces.end(); ++f )
            for( int i=0; i<3; ++i )
                (*f)->WT(i).P() = vcg::Point2f(0.0f,0.0f);
}


float FilterImgPatchParamPlugin::computeTotalPatchArea( RasterPatchMap &patches )
{
    float totalArea = 0;

    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
        for( PatchVec::iterator p=rp->begin(); p!=rp->end(); ++p )
            totalArea += p->bbox.Area();

    return totalArea;
}


int FilterImgPatchParamPlugin::computePatchCount( RasterPatchMap &patches )
{
    int nbPatches = 0;

    for( RasterPatchMap::iterator rp=patches.begin(); rp!=patches.end(); ++rp )
        nbPatches += rp->size();

    return nbPatches;
}




MESHLAB_PLUGIN_NAME_EXPORTER(FilterImgPatchParamPlugin)
