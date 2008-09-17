/* A class to represent the ui for the morpher plugin
 *  
 * @author Oscar Barney
 */

#ifndef MORPHER_DIALOG_H
#define MORPHER_DIALOG_H

#include <QtGui>

#include <meshlab/meshmodel.h>
#include <meshlab/glarea.h>

#include "ui_morpherDialog.h"

using namespace std;

class MorpherPlugin;

//a class to represent the special tree widgets for this UI
class MeshModelTreeWidgetItem : public QTreeWidgetItem
{
public:
	void setMeshModel(MeshModel *mm);
	
	MeshModel * getMeshModel();
	
private:
	MeshModel * meshModel;
	
};

class MorpherDialog : public QDockWidget
{
	Q_OBJECT

public:
	MorpherDialog(MorpherPlugin *plugin,
		QWidget *parent = 0);
	
	~MorpherDialog();
	
	//a function to set the current GL Area
	void setCurrentGLArea(GLArea *gla);
	
	//return the gla area we are working with
	GLArea * getCurrentGLArea();
	
	//allows the plugin to reset the morph if the user does not want to keep the changes
	void verifyKeepChanges();
	
private:
	//QT patern - the ui pointer
	Ui::morpherDialog ui;

	//the parent plugin
	MorpherPlugin *parentPlugin;
	
	GLArea *glArea;
	
	//a copy of the source's vertices so that we know where we started out the morph
	std::vector<CVertexO> * sourceVertexVector;
	
	MeshModel * sourceMeshModel;
	
	MeshModel * destMeshModel;
	
private slots:
	//triggered when the spin box changes the start value
	void startValueChanged(int newValue);
	
	//triggered when the spinbox changes the end value
	void endValueChanged(int newValue);
	
	//pick the Source Mesh
	void pickSourceMesh();
	
	//pick the Dest Mesh
	void pickDestMesh();
	
	//called when the slider's value changes (update the label for now)
	void sliderChanged(int newValue);
	
	//the user indicated that they want to recalculate the morph
	void recalculateMorph(int newValue);
	
	inline float Abs(float x ) { return x > 0. ? x : -x; }
	
	//given a start and end
	inline float calcNewPoint(float start, float end, float distanceRatio){
		return start + (end-start)*distanceRatio;
	}
};

#endif
