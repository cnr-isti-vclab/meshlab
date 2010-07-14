/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <Qt>
#include <QtGui>

#include "filter_virtual_range_scan.h"

using namespace std;
using namespace vcg;

using namespace agl::vrs;

/* initializes agl constants */
const string agl::global::default_shaders_path = "../../fgt/filter_vrs/agl/shaders/glsl/";
const string agl::global::vrs_path = "../../fgt/filter_vrs/agl/vrs/";

// ------- MeshFilterInterface implementation ------------------------
FilterVirtualRangeScan::FilterVirtualRangeScan()
{
    typeList << FP_VRS;
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);

    innerContext = new QGLContext( QGLFormat() );

    innerContext->makeCurrent();
    glewInit();
}

FilterVirtualRangeScan::~FilterVirtualRangeScan( void )
{
    delete innerContext;
}

QString FilterVirtualRangeScan::filterName(FilterIDType filterId) const
{
    switch (filterId) {
    case FP_VRS:
        return QString("Virtual Range Scan");
        break;
    }

    return "";
}

QString FilterVirtualRangeScan::filterInfo(FilterIDType filterId) const
{
    QString description = "Performs a virtual range scan onto the current mesh";
    return description;
}

void FilterVirtualRangeScan::initParameterSet(QAction* filter,MeshDocument &md, RichParameterSet &par)
{
    switch(ID(filter))
    {
    case FP_VRS:
        par.addParam( new RichInt( "povs", 12, "Povs:", "The number of point of views from which the target mesh is observed.") );
        par.addParam( new RichInt( "uniform_side", 64, "Uniform sampling resolution:",
                                   "The mesh will be sampled uniformly from a texture of v x v pixels, where v is the chosen value.") );
        par.addParam( new RichInt( "features_side", 512, "Feature sampling resolution:",
                                   "The filter performs feature detection from a texture of v x v pixels, where v is the chosen value.") );
        par.addParam( new RichDynamicFloat( "frontFacingCone", 40, 0.0f, 180.0f,
                                            "Front facing cone:",
                                            QString("Pixels whose normal is directed towards the viewer are considered front-facing.<br />") +
                                            "To be front-facing, these normals must reside within a given cone of directions, whose angle is set with this parameter."));
        par.addParam( new RichDynamicFloat( "bigJump", 0.1, 0.0, 1.0, "Big depth jump threshold:",
                                            QString("The filter detects mesh borders and big offsets within the mesh by testing the depth of neighbours pixels.<br />") +
                                            "This parameter controls the (normalized) minimum depth offset for a depth jump to be recognized.") );
        par.addParam( new RichDynamicFloat( "smallJump", 0.05f, 0.0f, 0.2f, "Small depth jump threshold:",
                                            QString("To be considered on the same mesh patch, neighbours pixels must be within this depth range.<br />") +
                                            "For example, if the max depth value is 0.6 and the min depth value is 0.4, then a value of 0.01 "+
                                            "means that the depth offset between neighbours pixels must be "+
                                            "less or equal 1/100th of (0.6 - 0.4) = 0.2.") );
        par.addParam( new RichDynamicFloat( "normalsAngle", 60, 0, 180, "Normals angle threshold:",
                                            "The minimum angle between neighbour pixels normals for the center pixel to be considered feature.") );
        par.addParam( new RichBool( "oneMesh", true, "Unify uniform and feature samples",
                                    QString("If checked, generates a unique mesh that contains both uniform and feature samples.<br />") +
                                    "If not checked, generates two output meshes: the first contains uniform samples and the second contains feature samples.") );
        break;
    }
}

bool FilterVirtualRangeScan::applyFilter( QAction* filter,
                             MeshDocument &md,
                             RichParameterSet &par,
                             vcg::CallBackPos* cb )
{
    switch(ID(filter))
    {
    case FP_VRS:

        int povs = par.getInt( "povs" );
        int uniform_sampling_resolution = par.getInt( "uniform_side" );
        int feature_sampling_resolution = par.getInt( "features_side" );
        float frontFacingCone = par.getDynamicFloat( "frontFacingCone" );
        float bigDepthJump = par.getDynamicFloat( "bigJump" );
        float smallDepthJump = par.getDynamicFloat( "smallJump" );
        float normalsAngle = par.getDynamicFloat( "normalsAngle" );
        bool oneMesh = par.getBool( "oneMesh" );

        MeshModel* curMeshModel = md.mm();
        CMeshO* startMesh = &( curMeshModel->cm );

        MeshModel* firstMeshModel = 0, *secondMeshModel = 0;
        CMeshO* firstMesh = 0, *secondMesh = 0;

        if( oneMesh )
        {
            firstMeshModel = md.addNewMesh( "VRS Point Cloud" );
            firstMeshModel->updateDataMask( curMeshModel );
            firstMesh = &( firstMeshModel->cm );
        }
        else
        {
            firstMeshModel = md.addNewMesh( "VRS Uniform Samples" );
            firstMeshModel->updateDataMask( curMeshModel );
            firstMesh = &( firstMeshModel->cm );
            secondMeshModel = md.addNewMesh( "VRS Feature Samples" );
            secondMeshModel->updateDataMask( curMeshModel );
            secondMesh = &( secondMeshModel->cm );
        }

        innerContext->makeCurrent();
        glPushAttrib( GL_ALL_ATTRIB_BITS );

        vrsParams.povs = povs;
        vrsParams.viewportResolution = uniform_sampling_resolution;
        vrsParams.generateUniformSamples = true;
        vrsParams.attributeMask = VRSParameters::POSITION |
                                  VRSParameters::NORMAL;
        vrsParams.frontFacingCone = frontFacingCone;
        vrsParams.bigDepthJump = bigDepthJump;
        vrsParams.maxDepthJump = smallDepthJump;
        vrsParams.angleThreshold = normalsAngle;
        vrsParams.discriminatorThreshold = 0.9;

        Sampler< CMeshO > sampler;
        sampler.addSamplerListener( this );
        this->cbBackup = cb;
        sampler.generateSamples( *startMesh, *firstMesh, vrsParams );

        vrsParams.generateUniformSamples = false;
        vrsParams.viewportResolution = feature_sampling_resolution;
        sampler.generateSamples( *startMesh,
                                 oneMesh? *firstMesh : *secondMesh, vrsParams );

        glPopAttrib();
        //innerContext->doneCurrent(); // <--- app crash!

        return true;
        break;
    }
    return false;
}

MeshFilterInterface::FilterClass FilterVirtualRangeScan::getClass(QAction* filter)
{
    switch(ID(filter)) {
    case FP_VRS:
        return MeshFilterInterface::Sampling;
        break;
    default: assert(0);
        return MeshFilterInterface::Generic;
    }
}

int FilterVirtualRangeScan::getRequirements(QAction *filter)
{
    switch( ID(filter) )
    {
    case FP_VRS:
        return MeshModel::MM_NONE;
        break;
    }

    return MeshModel::MM_NONE;
}

int FilterVirtualRangeScan::postCondition(QAction *filter) const
{
    switch(ID(filter))
    {
    case FP_VRS:
        return MeshModel::MM_NONE;
        break;
    }

    return MeshModel::MM_NONE;
}

// SamplerListener implementation
void FilterVirtualRangeScan::setupComplete( VRSParameters* currentParams )
{
    if( vrsParams.generateUniformSamples )
    {
        cbBackup( 0, "Generating uniform samples..." );
    }
    else
    {
        cbBackup( 0, "Generating feature samples..." );
    }
}

void FilterVirtualRangeScan::povProcessed( int povIndex, int samplesCount )
{
    char buffer[200];
    string s;
    if( vrsParams.generateUniformSamples )
    {
       s = "Generating uniform samples";
    }
    else
    {
        s = "Generating feature samples";
    }

    povIndex++;

    sprintf( buffer, "%s - POV %d of %d - %d samples", s.c_str(), povIndex, vrsParams.povs, samplesCount );
    cbBackup( 100 * povIndex / vrsParams.povs, buffer );

    if( povIndex == vrsParams.povs )
    {
        totSamples = samplesCount;
    }
}

void FilterVirtualRangeScan::texturesDownloaded ( void )
{
    cbBackup( 0, "Filling target mesh..." );
}

void FilterVirtualRangeScan::fillingTargetMesh( int samplesCount )
{
    char buffer[150];
    sprintf( buffer, "Filling target mesh - %d of %d samples", samplesCount, totSamples );
    cbBackup( 100 * samplesCount / totSamples, buffer );
}

// ----------------------------------------------------------------------
Q_EXPORT_PLUGIN(FilterVirtualRangeScan)

