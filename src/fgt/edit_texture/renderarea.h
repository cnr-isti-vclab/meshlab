#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <meshlab/meshmodel.h>
#include <wrap/gui/trackball.h>
#include <stdio.h>
#include <QGLWidget>

#define TEXTX 175
#define TEXTY 200
#define TRANSLATE 5
#define RADIUS 10
#define RECTDIM 30

using namespace std;
using namespace vcg;

class RenderArea : public QGLWidget
{
    Q_OBJECT

public:
	enum Mode { View, Edit, EditVert, Select };
	enum EditMode { Scale, Rotate, NoEdit };
	enum SelectMode { Area, Connected, Vertex };

    RenderArea(QWidget *parent = 0, QString path = QString(), MeshModel *m = 0, unsigned textNum = 0);
	~RenderArea();

public:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
	void setTexture(QString path);
	void ChangeMode(int index);
	void ChangeSelectMode(int index);
	void RemapClamp();
	void RemapMod();

protected:
    void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

private:
	bool antialiased;	// Antialiasing 
    QImage image;		// Background texture
	unsigned textNum;	// Number of tab (--> index in 'textures' vector)
	QString fileName;	// Name of the texture
	MeshModel *model;	// Ref to the model (for upate)

	Mode mode;			// Action type
	EditMode editMode;
	SelectMode selectMode;

	// Trackball data
	vcg::Trackball *tb;
	vcg::Point2f viewport;
	float oldX, oldY;
	int tmpX, tmpY;

	QPen pen;			// For 2D painting
    QBrush brush;

	int panX, panY, tpanX, tpanY, oldPX, oldPY, posVX, posVY;	// Temp for axis
	int maxX, maxY, minX, minY;	// For texCoord out of border

	int selBit, selVertBit, selFaceBit;		// User bit: idicates if a face/vertex is selected for editing
	bool selected, selectedV;	// Indicates if the are some selected faces

	QPointF origin;		// Origin for rotate editing
	QRect originR;
	int orX, orY;

	QPoint start, end;	// Selection area
	QRect area;			// Dragged rectangle

	// Info for interactive editing
	vector<QRect> selRect;	// Vector of buttons area
	QRect vertRect;			// Rectangle for vertex
	QRect selection;		// Selection area
	QPoint selStart, selEnd;
	int posX, posY, rectX, rectY, oldSRX, oldSRY;	// Stored value
	float degree, scaleX, scaleY;					// Value for edit
	int highlighted, pressed, oldDelta;	// Info about mouse
	QPointF oScale;			// Point of origin for scaling
	int initVX, initVY;		// Old values of viewport
	float B2, Rm, Rq;		// Params for line intersection

	QImage rot, scal;		// Images for buttons

	float zoom;				// Actual value of zoom

	void UpdateUV();
	void UpdateVertex();
	void ResetTrack(bool reset);
	void SelectFaces();
	void SelectVertexes();
	void HandleScale(QPoint e);
	void HandleRotate(QPoint e);
	void RotateComponent(float theta);
	void ScaleComponent(float percentX, float percentY);
	void RecalculateSelectionArea();
	void UpdateSelectionArea(int x, int y);

signals:
	void UpdateModel();

};

#endif