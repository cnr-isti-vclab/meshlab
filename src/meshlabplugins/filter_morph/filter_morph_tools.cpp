/*
 * This class will will hold useful things for doing morphing of meshes
 * 
 * @author Oscar Barney
 * 
 */

#include "filter_morph_tools.h"

#include <QtGui>

using namespace vcg;


MorphTools::MorphTools(QWidget *parent)
{
	parentWidget = parent;
}

void MorphTools::calculateMorph(float percentage, MeshModel *sourceMeshModel, MeshModel *destMeshModel)
{
	//if two models are not selected return
	if(NULL == sourceMeshModel || NULL == destMeshModel){
		qDebug() << "trying to calculate with a null model";
		return;
	}
	
	float morphRatio = percentage/100.0;
	//qDebug() << "Ratio is: " << morphRatio;
	
	//make sure this will be a valid morph and that we didnt loose any vertices
	//since these meshes were picked
	if(sourceMeshModel->cm.vn == destMeshModel->cm.vn)
	{
		std::vector<CVertexO> &destVertexVector = destMeshModel->cm.vert;
		
		std::vector<CVertexO> *sourceVertexVector = new std::vector<CVertexO>(sourceMeshModel->cm.vert);
		std::vector<CVertexO> &vertexToChangeVector = sourceMeshModel->cm.vert;
		
		//TODO figure out what to do with the source vertex vector!!!
		
		for(int i=0; i < sourceVertexVector->size(); i++)
		{
		/*
			qDebug() << "before changing x: " << vertexToChangeVector.at(i).P()[0]
			         << " y: " << vertexToChangeVector.at(i).P()[1]
			         << " z: " << vertexToChangeVector.at(i).P()[2];

		*/
			
			//get new x
			vertexToChangeVector.at(i).P()[0] = calcNewPoint(sourceVertexVector->at(i).P()[0],
								destVertexVector.at(i).P()[0], morphRatio);
			//get new y
			vertexToChangeVector.at(i).P()[1] = calcNewPoint(sourceVertexVector->at(i).P()[1],
								destVertexVector.at(i).P()[1], morphRatio);
			//get new z
			vertexToChangeVector.at(i).P()[2] = calcNewPoint(sourceVertexVector->at(i).P()[2],
								destVertexVector.at(i).P()[2], morphRatio);
		/*
			qDebug() << "after changing x: " << vertexToChangeVector.at(i).P()[0]
					 << " y: " << vertexToChangeVector.at(i).P()[1]
					 << " z: " << vertexToChangeVector.at(i).P()[2];
		*/
			
			//now adjust the normals
			//get new x for normal
			vertexToChangeVector.at(i).N()[0] = calcNewPoint(sourceVertexVector->at(i).N()[0],
								destVertexVector.at(i).N()[0], morphRatio);
			//get new y for normal
			vertexToChangeVector.at(i).N()[1] = calcNewPoint(sourceVertexVector->at(i).N()[1],
								destVertexVector.at(i).N()[1], morphRatio);
			//get new z for normal
			vertexToChangeVector.at(i).N()[2] = calcNewPoint(sourceVertexVector->at(i).N()[2],
								destVertexVector.at(i).N()[2], morphRatio);
			
			//normalize the normal
			vertexToChangeVector.at(i).N().Normalize();
		}
		
		//qDebug() << "NOW, fix face normals for the mesh so that the lighting is correct";
		
		assert(sourceMeshModel->cm.face.size()==size_t(sourceMeshModel->cm.fn));
		
		for(int i=0; i<sourceMeshModel->cm.face.size(); ++i)
		{
			//NOTE: this if/else is coppied from vcg library's import_obj.h line 559
			if ( sourceMeshModel->ioMask & vcg::tri::io::Mask::IOM_WEDGNORMAL )
			{
				// face normal is computed as an average of wedge normals
				sourceMeshModel->cm.face[i].N().Import( sourceMeshModel->cm.face[i].WN(0) +
						sourceMeshModel->cm.face[i].WN(1) + sourceMeshModel->cm.face[i].WN(2) );
			} else
			{
				// computing face normal from position of face vertices
				vcg::face::ComputeNormalizedNormal(sourceMeshModel->cm.face[i]);
			}
		}		
	}
}
