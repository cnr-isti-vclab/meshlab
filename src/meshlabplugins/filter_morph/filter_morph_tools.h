/*
 * This class will will hold useful things for doing morphing of meshes
 * 
 * @author Oscar Barney
 * 
 */

#ifndef MORPH_TOOLS_H
#define MORPH_TOOLS_H

#include <QObject>

#include <meshlab/meshmodel.h>
#include <vcg/complex/trimesh/update/normal.h>
class MorphTools : public QObject
{
	Q_OBJECT
	
public:
	//calculate the morph
	//float percentage - how much the source should look like the target
	void calculateMorph(float percentage, MeshModel *sourceMeshModel, MeshModel *destMeshModel);
	
private:
	//given a start and end
	inline float calcNewPoint(float start, float end, float distanceRatio){
		return start + (end-start)*distanceRatio;
	}
	
};

#endif
