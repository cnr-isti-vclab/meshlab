#ifndef EDITCUTPLUGIN_H
#define EDITCUTPLUGIN_H

#include <QObject>
#include <common/plugins/interfaces/edit_plugin.h>
#include <map>

class EditCutPlugin : public QObject, public EditTool
{
	Q_OBJECT

public:
	EditCutPlugin();
	virtual ~EditCutPlugin() {}

	static const QString info();

	void suggestedRenderingData(MeshModel &m, MLRenderingData &dt);
	bool startEdit(MeshModel &m, GLArea *parent, MLSceneGLSharedDataContext *cont);
	void endEdit(MeshModel &m, GLArea *parent, MLSceneGLSharedDataContext *cont);
	void decorate(MeshModel &m, GLArea *parent);
	void mousePressEvent(QMouseEvent *event, MeshModel &m, GLArea *gla);
	void mouseMoveEvent(QMouseEvent *event, MeshModel &m, GLArea *gla);
	void mouseReleaseEvent(QMouseEvent *event, MeshModel &m, GLArea *gla);
	void keyReleaseEvent(QKeyEvent *e, MeshModel &m, GLArea *gla);
	void keyPressEvent(QKeyEvent *e, MeshModel &m, GLArea *gla);

private:
	// Polyline state
	std::vector<vcg::Point2f> cutPolyLine;

	// Projection data
	GLdouble mvMatrix_f[16];
	GLdouble prMatrix_f[16];
	GLint viewpSize[4];
	Eigen::Matrix<Scalarm, 4, 4> SelMatrix;
	Scalarm SelViewport[4];

	// Drawing helpers
	void DrawXORPolyLine(GLArea *gla);

	// Core algorithm
	void executeCut(MeshModel &m, GLArea *gla);
	void selectInsideFaces(MeshModel &m, GLArea *gla);
};

#endif
