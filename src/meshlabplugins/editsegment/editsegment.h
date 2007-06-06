#ifndef EDITSEGMENT_H
#define EDITSEGMENT_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QDockWidget>
#include <QHash>
#include <QDialog>
#include <QDebug>

#include <meshlab/glarea.h>
#include <vcg/math/matrix44.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshCutDialog.h>

#include <cutting.h>

struct Penn {
	float radius;
	int paintutensil;
	int painttype;
	bool backface;
	bool invisible;
};


class EditSegment : public QObject, public MeshEditInterface {
	Q_OBJECT
		Q_INTERFACES(MeshEditInterface)
		QList <QAction *> actionList;
private: 
	MeshCutting<CMeshO> * meshCut;
	bool show_trackball;
	bool first;
	bool dragging;
	QPoint starting_point;
	QPoint current_point;
	QPoint previous_point;
	Qt::MouseButton mouse_button_pressed;
	bool pressed;
	Penn pen;
	QDockWidget *meshcut_dock;
	MeshCutDialog * meshCutDialog;

	vector<CMeshO::FacePointer> currentSelection;
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

	const QString Info(QAction *);
	const PluginInfo &Info();

	void StartEdit(QAction * /*mode*/, MeshModel & /*m*/, GLArea * /*parent*/);
	void EndEdit	(QAction * /*mode*/, MeshModel & /*m*/, GLArea * /*parent*/);
	void Decorate	(QAction * /*mode*/, MeshModel & /*m*/, GLArea * /*parent*/);
	void mousePressEvent    (QAction *, QMouseEvent *event, MeshModel & /*m*/, GLArea * );
	void mouseMoveEvent     (QAction *, QMouseEvent *event, MeshModel & /*m*/, GLArea * );
	void mouseReleaseEvent  (QAction *, QMouseEvent *event, MeshModel & /*m*/, GLArea * );
	//void wheelEvent         (QAction *QWheelEvent*e, MeshModel &/*m*/, GLArea * );
	QList<QAction *> actions() const ;

public slots:
	void MeshCut();

};

#endif