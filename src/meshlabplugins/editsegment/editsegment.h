#ifndef EDITSEGMENT_H
#define EDITSEGMENT_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QDockWidget>
#include <QDialog>
#include <QDebug>
#include <QMap>

#include <meshlab/glarea.h>
#include <vcg/math/matrix44.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshcutdialog.h>
#include <wrap/gui/trackball.h>

#include <cutting.h>

struct Penn {
	float radius;
	bool backface;
	bool invisible;
};


class EditSegment : public QObject, public MeshEditInterface {
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)

private: 
	bool show_trackball;
	bool first;
	bool dragging;
	
	bool selectForeground;
	bool doRefine;
	float curvatureWeight;
	float normalWeight;
	
	QPoint current_point;
	QPoint previous_point;
	Qt::MouseButton mouse_button_pressed;
	bool pressed;
	Penn pen;
	QDockWidget *meshcut_dock;
	MeshCutDialog * meshCutDialog;
	GLArea * glarea;
	vcg::MeshCutting<CMeshO> * meshCut;

	QMap<GLArea *, vcg::MeshCutting<CMeshO> *> glarea_map;

	std::vector<CMeshO::FacePointer> currentSelection;
	GLfloat *pixels;

	double mvmatrix[16]; //modelview
	double projmatrix[16]; //projection
	GLint viewport[4]; //viewport

	inline void updateMatrixes() {		
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
	}

	void DrawXORCircle(MeshModel &m,GLArea * gla, bool doubleDraw); // draws the circle which represents the pen

public:

	EditSegment();
	~EditSegment();

	static const QString Info();

	void StartEdit(MeshModel & /*m*/, GLArea * /*parent*/);
	void EndEdit(MeshModel & /*m*/, GLArea * /*parent*/);
	void Decorate(MeshModel & /*m*/, GLArea * /*parent*/);
	void mousePressEvent(QMouseEvent *event, MeshModel & /*m*/, GLArea * );
	void mouseMoveEvent(QMouseEvent *event, MeshModel & /*m*/, GLArea * );
	void mouseReleaseEvent(QMouseEvent *event, MeshModel & /*m*/, GLArea * );

public slots:
	void MeshCutSlot();
	void UpdateCurvatureSlot();
	void SelectForegroundSlot(bool);
	void doRefineSlot(bool);
	void changeNormalWeight(int);
	void changeCurvatureWeight(int);
	void changePenRadius(int);

	void ColorizeGaussianSlot();
	void ColorizeMeanSlot();
	void ResetSlot();

};

#endif
