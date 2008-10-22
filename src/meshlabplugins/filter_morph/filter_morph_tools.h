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
	
	//constructor...  parent widget is used to open dialogs to the user
	MorphTools(QWidget *parent=0);

	//calculate the morph
	//float percentage - how much the source should look like the target
	void calculateMorph(float percentage, MeshModel *sourceMeshModel, MeshModel *destMeshModel);
	
private:
	//incase we need to dispay any widgets
	QWidget *parentWidget;
	
	inline float Abs(float x ) { return x > 0. ? x : -x; }
	
	//given a start and end
	inline float calcNewPoint(float start, float end, float distanceRatio){
		return start + (end-start)*distanceRatio;
	}
	
};

#endif
