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
#include "my_gl_widget.h"

#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/append.h>

using namespace std;
using namespace vcg;
using namespace vs;

// ------- MeshFilterInterface implementation ------------------------
FilterVirtualRangeScan::FilterVirtualRangeScan()
{
    typeList << FP_VRS;
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
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
        par.addParam( new RichDynamicFloat( "xConeAxis", 0.0f, -1.0f, 1.0f, "Cone axis (X):", "The X component of povs looking cone's axis.") );
        par.addParam( new RichDynamicFloat( "yConeAxis", 1.0f, -1.0f, 1.0f, "Cone axis (Y):", "The Y component of povs looking cone's axis.") );
        par.addParam( new RichDynamicFloat( "zConeAxis", 0.0f, -1.0f, 1.0f, "Cone axis (Z):", "The Z component of povs looking cone's axis.") );

        par.addParam( new RichBool( "useCustomPovs", false, "Use povs contained in the following layer, ignoring the above parameters", "" ) );
        par.addParam( new RichMesh( "povsLayer", md.mm(), &md, "Povs layer:", "" ) );

        par.addParam( new RichDynamicFloat( "coneGap", 360.0f, 0.0f, 360.0f, "Cone gap:", "The looking cone gap (in degrees).") );
        par.addParam( new RichInt( "uniform_side", 64, "Uniform sampling resolution:",
                                   "The mesh will be sampled uniformly from a texture of v x v pixels, where v is the chosen value.") );
        par.addParam( new RichInt( "features_side", 512, "Feature sampling resolution:",
                                   "The filter performs feature detection from a texture of v x v pixels, where v is the chosen value.") );
        par.addParam( new RichDynamicFloat( "frontFacingConeU", 40, 0.0f, 180.0f,
                                            "Front facing cone (uniform):",
                                            QString("Pixels whose normal is directed towards the viewer are considered front-facing.<br />") +
                                            "To be front-facing, these normals must reside within a given cone of directions, whose angle is set with this parameter.<br />" +
                                            "Only the front-facing pixels form the uniform samples cloud" ) );
        par.addParam( new RichDynamicFloat( "bigJump", 0.1, 0.0, 1.0, "Big depth jump threshold:",
                                            QString("The filter detects mesh borders and big offsets within the mesh by testing the depth of neighbours pixels.<br />") +
                                            "This parameter controls the (normalized) minimum depth offset for a depth jump to be recognized.") );
        par.addParam( new RichDynamicFloat( "frontFacingConeF", 40, 0.0f, 180.0f,
                                            "Front facing cone (features):",
                                            QString("Look at the <i>Front facing cone (uniform)</i> parameter description to understand when a pixel is") +
                                            "said <i>front-facing</i>. In the feature sensitive sampling step, border pixels are recognized as features if " +
                                            "they are facing the observer within a given cone of direction, whose gap is specified (in degrees) with this parameter." ) );
        par.addParam( new RichDynamicFloat( "smallJump", 0.01f, 0.001f, 0.1f, "Small depth jump threshold:",
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


        bool useLayerPovs = par.getBool( "useCustomPovs" );
        if( useLayerPovs )
        {
            MeshModel* tmpMesh = par.getMesh( "povsLayer" );
            assert( tmpMesh );
            CMeshO::PerMeshAttributeHandle< std::vector<Pov>  > povs_handle;
            povs_handle = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::vector<Pov> > (tmpMesh->cm,"pointofviews");
            if (!vcg::tri::Allocator<CMeshO>::IsValidHandle(tmpMesh->cm,povs_handle) )
            {
                errorMessage = "Can't apply the filter because the selected layer contains no point of views.";
                return false;
            }

            vrsParams.customPovs = povs_handle(); // copies the povs data
        }
        else
        {
            vrsParams.povs = par.getInt( "povs" );
            vrsParams.coneAxis[ 0 ] = par.getDynamicFloat( "xConeAxis" );
            vrsParams.coneAxis[ 1 ] = par.getDynamicFloat( "yConeAxis" );
            vrsParams.coneAxis[ 2 ] = par.getDynamicFloat( "zConeAxis" );
            vrsParams.coneGap = par.getDynamicFloat( "coneGap" );
        }

        vrsParams.uniformResolution = par.getInt( "uniform_side" );
        vrsParams.featureResolution = par.getInt( "features_side" );
        vrsParams.frontFacingConeU = par.getDynamicFloat( "frontFacingConeU" );
        vrsParams.frontFacingConeF = par.getDynamicFloat( "frontFacingConeF" );
        vrsParams.bigDepthJump = par.getDynamicFloat( "bigJump" );
        vrsParams.smallDepthJump = par.getDynamicFloat( "smallJump" );
        vrsParams.angleThreshold = par.getDynamicFloat( "normalsAngle" );
        vrsParams.useCustomPovs = useLayerPovs;

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
            secondMesh = firstMesh;
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

        /*
        innerContext = new QGLContext( QGLFormat() );
        innerContext->makeCurrent();
        glewInit();
        glEnable( GL_DEPTH_TEST );

        Sampler< CMeshO > sampler( &vrsParams, startMesh, firstMesh, secondMesh );
        sampler.listeners.push_back( this );
        this->cb = cb;
        sampler.generateSamples();

        delete innerContext;
        */

        MyGLWidget* tmpWidget = new MyGLWidget( 0 );
        tmpWidget->params = &vrsParams;
        tmpWidget->inputMesh = startMesh;
        tmpWidget->uniformSamplesMesh = firstMesh;
        tmpWidget->featureSamplesMesh = secondMesh;
        tmpWidget->samplerListener = this;
        this->cb = cb;
        tmpWidget->show();

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
void FilterVirtualRangeScan::startingSetup( void )
{
    //cb( 0, "Starting setup..." );
}

void FilterVirtualRangeScan::setupComplete( int povs )
{
    /*
    char buf[ 100 ];
    sprintf( buf, "Processing pov 1 of %d", vrsParams.povs );
    cb( 0, buf );
    samplingFeatures = false;
    */
}

void FilterVirtualRangeScan::povProcessed( int pov, int samples )
{
    /*
    char buf[ 250 ];
    sprintf( buf, "Processing pov %d of %d (%d %s samples)...", pov + 1,
             vrsParams.povs, samples, samplingFeatures? "feature" : "uniform" );
    cb( (pov + 1)/vrsParams.povs * 100, buf );
    */
}

void FilterVirtualRangeScan::startingFeatureSampling( void )
{
    //samplingFeatures = true;
}
// ----------------------------------------------------------------------
Q_EXPORT_PLUGIN(FilterVirtualRangeScan)

