#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <meshlab/meshmodel.h>
#include <wrap/gui/trackball.h>
#include <stdio.h>
#include <QGLWidget>

#define AREADIM 400
#define TEXTX 175
#define TEXTY 200
#define TRANSLATE 5
#define RADIUS 10

#define NO_CHANGE -1
#define RESET -2
#define IGNORECMD -3
#define ENABLECMD -4
#define DISABLECMD -5

using namespace std;

class RenderArea : public QGLWidget
{
    Q_OBJECT

public:
	enum Mode { View, Edit, Select };

    RenderArea(QWidget *parent = 0, QString path = QString(), MeshModel *m = 0, unsigned textNum = 0);
	~RenderArea();

	vector<int> connected;	// Vector of indexes of face connected selected by user

	bool isDragging;
	int highlightedPoint,	// Vertex highlighted when the mouse move over it
		highComp,			// Index of the vertex element of connected component
		highClick;			// Clicked vertex
	bool out;

public:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
	void setTexture(QString path);
	void ChangeMode(int index);
	void RemapRepeat();
	void RemapClamp();
	void RemapMod();
	void UpdateUV();
	void UpdateVertex(float u, float v);
	void UpdateSingleUV(int index, float u, float v);
	void UpdateComponentPos(int x, int y);
	void RotateComponent(float theta);
	void ScaleComponent(int percent);

protected:
    void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent*e);

private:
	bool antialiased;	// Antialiasing 
    QImage image;		// Background texture
	unsigned textNum;	// Number of tab (--> index in 'textures' vector)
	QString fileName;	// Name of the texture
	MeshModel *model;	// Ref to the model (for upate)

	Mode mode;			// Action type

	// Trackball data
	vcg::Trackball *tb;
	Point2f viewport;
	float oldX, oldY;
	int tmpX, tmpY;

	QPen pen;			// For 2D painting
    QBrush brush;

	int panX, panY, tpanX, tpanY, oldPX, oldPY;	// Temp for axis
	int maxX, maxY, minX, minY;	// For texCoord out of border

	void ResetTrack();
	void VisitConnected();
	QRect GetRepeatVertex(float u, float v, int index);
	QRect GetClampVertex(float u, float v, int index);

signals:
	void UpdateStat(float u, float v, int faceN, int vertN, int countFace);

};

#endif