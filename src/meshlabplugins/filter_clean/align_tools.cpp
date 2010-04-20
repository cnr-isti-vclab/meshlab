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

#include <QtGui>

#include "align_tools.h"

#include <meshlabplugins/edit_pickpoints/pickedPoints.h>

#include <meshlabplugins/editalign/align/align_parameter.h>
#include <meshlabplugins/editalign/meshtree.h>
#include <meshlabplugins/editalign/align/AlignPair.h>

#include <vcg/math/point_matching.h>

#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/allocate.h>

using namespace vcg;

const QString AlignTools::UseMarkers = "UseM";
const QString AlignTools::AllowScaling = "AllowScaling";
const QString AlignTools::UseICP = "UseICP";
const QString AlignTools::StuckMesh = "StuckMesh";
const QString AlignTools::MeshToMove = "MeshToMove";

AlignTools::AlignTools(){}

void AlignTools::buildParameterSet(MeshDocument &md,RichParameterSet & parlst)
{
	vcg::AlignPair::Param ICPParameters;
	AlignParameter::buildRichParameterSet(ICPParameters, parlst);

	parlst.addParam(new RichBool(UseMarkers, true, "Use Markers for Alignment","if true (default), then use the user picked markers to do an alignment (or pre alignment if you also use ICP)."));
	parlst.addParam(new RichBool(AllowScaling, false, "Scale the mesh","if true (false by default), in addition to the alignment, scale the mesh based on the points picked"));
	
	parlst.addParam(new RichBool(UseICP, true, "Use ICP for Alignment","if true (default), then use the ICP to align the two meshes."));
	
	parlst.addParam(new RichMesh (StuckMesh, md.mm(), &md,"Stuck Mesh",
			"The mesh that will not move."));
	parlst.addParam(new RichMesh (MeshToMove, md.mm(), &md, "Mesh to Move",
			"The mesh that will move to fit close to the Stuck Mesh."));
}

bool AlignTools::setupThenAlign(MeshModel &/*mm*/, RichParameterSet & par)
{
	//mesh that wont move
	MeshModel *stuckModel = par.getMesh(StuckMesh);
	PickedPoints *stuckPickedPoints = 0;

	//mesh that will move
	MeshModel *modelToMove = par.getMesh(MeshToMove);
	PickedPoints *modelToMovePickedPoints = 0;

	bool useMarkers = par.getBool(UseMarkers);
	
	if(NULL == stuckModel || NULL == modelToMove)
	{
		qDebug() << "one of the input meshes to filter align was null";
		return false;
	}

	//if we are going to use the markers try to load them
	if(useMarkers){
		//first try to get points from memory
		if(vcg::tri::HasPerMeshAttribute(stuckModel->cm, PickedPoints::Key) )
		{
			CMeshO::PerMeshAttributeHandle<PickedPoints*> ppHandle =
					vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<PickedPoints*>(stuckModel->cm, PickedPoints::Key);
	
			stuckPickedPoints = ppHandle();
	
			if(NULL == stuckPickedPoints){
				qDebug() << "problem casting to picked points";
				return false;
			}
		} else
		{
			//now try to load them from a file
			QString ppFileName = PickedPoints::getSuggestedPickedPointsFileName(*stuckModel);
			QFileInfo file(ppFileName);
			if(file.exists())
			{
				stuckPickedPoints = new PickedPoints();
				bool success = stuckPickedPoints->open(ppFileName);
	
				if(!success){
					qDebug() << "problem loading stuck picked points from a file";
					return false;
				}
			} else
			{
				qDebug() << "stuck points file didnt exist: " << ppFileName;
				//Log(GLLogStream::WARNING, "No points were found for the Stuck mesh.");
				return false;
			}
		}
	
		if(vcg::tri::HasPerMeshAttribute(modelToMove->cm, PickedPoints::Key) )
		{
			CMeshO::PerMeshAttributeHandle<PickedPoints*> ppHandle =
					vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<PickedPoints*>(modelToMove->cm, PickedPoints::Key);
	
			modelToMovePickedPoints = ppHandle();
	
			if(NULL == modelToMovePickedPoints){
				qDebug() << "problem casting to picked poitns";
				return false;
			}
		} else
		{
			QString ppFileName = PickedPoints::getSuggestedPickedPointsFileName(*modelToMove);
			QFileInfo file(ppFileName);
			if(file.exists())
			{
				modelToMovePickedPoints = new PickedPoints();
				bool success = modelToMovePickedPoints->open(ppFileName);
				if(!success){
					qDebug() << "failed to load modelToMove pick points";
					return false;
				}
			} else
			{
				qDebug() << "model to move points file didnt exist: " << ppFileName;
				//Log(GLLogStream::WARNING, "No points were found for the mesh to move.");
				return false;
			}
		}
	}
	
	bool result = AlignTools::align(stuckModel, stuckPickedPoints,
			modelToMove, modelToMovePickedPoints,
			0, par);
	
	return result;
}


bool AlignTools::align(MeshModel *stuckModel, PickedPoints *stuckPickedPoints,
		MeshModel *modelToMove, PickedPoints *modelToMovePickedPoints,
		GLArea *modelToMoveGLArea,
		RichParameterSet &filterParameters,
		QWidget *parentWidget, bool confirm)
{
	vcg::Matrix44f result;

	bool useMarkers = filterParameters.getBool(UseMarkers);
	bool allowScaling = filterParameters.getBool(AllowScaling);
	bool useICP = filterParameters.getBool(UseICP);

	if(useMarkers){
		//get the picked points
		std::vector<vcg::Point3f> *stuckPoints = stuckPickedPoints->getPoint3fVector();
		std::vector<vcg::Point3f> *meshToMovePoints = modelToMovePickedPoints->getPoint3fVector();
		
		//number of points are not the same so return false
		if(stuckPoints->size() != meshToMovePoints->size())	return false;

		//this will calculate the transform for the destination mesh model which we will be moving
		//into alignment with the source
		if(allowScaling)
		{
			qDebug() << "Scaling allowed";
			vcg::PointMatching<float>::ComputeSimilarityMatchMatrix(result, *stuckPoints, *meshToMovePoints);
		} else
			vcg::PointMatching<float>::ComputeRigidMatchMatrix(result, *stuckPoints, *meshToMovePoints);

		//set the transform
		modelToMove->cm.Tr = result;

		if(NULL != modelToMoveGLArea)
			modelToMoveGLArea->update();
	}

	if(useICP)
	{
		qDebug("Now on to ICP");

		//create a meshtree
		MeshTree meshTree;
		//put both meshes in it
		meshTree.nodeList.push_back(new MeshNode(stuckModel, 0));
		meshTree.nodeList.push_back(new MeshNode(modelToMove, 1));

		//set both to glued
		foreach(MeshNode *mn, meshTree.nodeList)
			mn->glued=true;

		vcg::AlignPair::Param ICPParameters;

		//get the parameter values
		AlignParameter::buildAlignParameters(filterParameters, ICPParameters);

		meshTree.Process(ICPParameters);

		qDebug() << "done with process for ICP";

		if(NULL != modelToMoveGLArea)
				modelToMoveGLArea->update();

	}

	if(useMarkers || useICP)
	{

		if(confirm && NULL != modelToMoveGLArea)
		{
			bool removeMeshAddedForQuestion = false;

			vcg::Color4b oldStuckColor;
			vcg::Color4b oldToMoveColor;
			vcg::GLW::ColorMode	oldColorMode;
			//if the stuck model is not displayed next to the model to move,
			//then temporarily display it
			if(!modelToMoveGLArea->meshDoc.meshList.contains(stuckModel))
			{
				removeMeshAddedForQuestion = true;
				modelToMoveGLArea->meshDoc.meshList.push_back(stuckModel);

				//save the old colors
				oldStuckColor = stuckModel->cm.C();
				oldToMoveColor = modelToMove->cm.C();
				//set the color of the objects
				stuckModel->cm.C() = vcg::Color4b::LightBlue;
				modelToMove->cm.C() = vcg::Color4b::LightGray;

				//save the old colorMode
				oldColorMode = modelToMoveGLArea->rm.colorMode;
				//set the new colorMode
				modelToMoveGLArea->rm.colorMode = GLW::CMPerMesh;

				modelToMoveGLArea->update();
			}

			int returnValue = QMessageBox::question(parentWidget,
					"MeshLab", "Do you want accept this alignment?",
					QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

			//if we added the other model in for comparing
			if(removeMeshAddedForQuestion)
			{
				//remove the mesh that was added
				modelToMoveGLArea->meshDoc.meshList.pop_back();

				//set back to how things were before
				stuckModel->cm.C() = oldStuckColor;
				modelToMove->cm.C() = oldToMoveColor;
				modelToMoveGLArea->rm.colorMode = oldColorMode;

				modelToMoveGLArea->update();
			}

			if(returnValue == QMessageBox::No)
			{
				modelToMove->cm.Tr.SetIdentity();

				modelToMoveGLArea->update();

				return false;
			}
		}

		//if there are points (may not be if you just used ICP
		if(NULL != modelToMovePickedPoints)
		{
			//now translate the picked points points
			modelToMovePickedPoints->translatePoints(modelToMove->cm.Tr);

			//update the metadata
			CMeshO::PerMeshAttributeHandle<PickedPoints*> ppHandle =
				(vcg::tri::HasPerMeshAttribute(modelToMove->cm, PickedPoints::Key) ?
					vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<PickedPoints*> (modelToMove->cm, PickedPoints::Key) :
					vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute<PickedPoints*> (modelToMove->cm, PickedPoints::Key) );

			ppHandle() = modelToMovePickedPoints;
		}
		
		//now save the transform as perMeshData so that we can undo it in the future
		CMeshO::PerMeshAttributeHandle<vcg::Matrix44f> transformHandle =
			(vcg::tri::HasPerMeshAttribute(modelToMove->cm, getKey()) ?
				vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<vcg::Matrix44f> (modelToMove->cm, getKey()) :
				vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute<vcg::Matrix44f> (modelToMove->cm, getKey()) );
		transformHandle() = modelToMove->cm.Tr;
		
		//now translate all the points in the mesh
		//TODO probably should call a function to do this so if meshlab changes we dont have to
		//taken from meshlab/src/meshlabplugins/meshfilter/meshfilter.cpp
		//if (ID(filter) == (FP_FREEZE_TRANSFORM) ) {
		vcg::tri::UpdatePosition<CMeshO>::Matrix(modelToMove->cm, modelToMove->cm.Tr);
		vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(modelToMove->cm);
		vcg::tri::UpdateBounding<CMeshO>::Box(modelToMove->cm);
		modelToMove->cm.Tr.SetIdentity();

		return true;
	} else
	{
		qDebug() << "you ran align without choosing a method";
	}

	return false;
}
