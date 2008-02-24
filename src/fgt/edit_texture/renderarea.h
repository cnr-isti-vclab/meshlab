#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>

#include "container.h"

#include <stdio.h>

#define AREADIM 300
#define TEXTX 125
#define TEXTY 150
#define TRANSLATE +0.5
#define RADIUS 10

#define NO_CHANGE -1
#define RESET -2
#define IGNORECMD -3
#define ENABLECMD -4
#define DISABLECMD -5

using namespace std;

class RenderArea : public QWidget
{
    Q_OBJECT

public:
	enum Mode { Point, Face, Smooth };

    RenderArea(QWidget *parent = 0, QString textureName = QString(), 
		vector<Container> map = vector<Container>(), 
		bool outOfRange = false);
	~RenderArea();

	vector<Container> map;	// Vector of UV Vertexes
	vector<int> connected;	// Vector of indexes of face connected selected by user

	bool isDragging;
	int highlightedPoint,	// Vertex highlighted when the mouse move over it
		highComp,			// Index of the vertex element of connected component
		highClick;			// Clicked vertex

	bool out;				// Indicates if exists uv coord < 0 or > 1
	Mode mode;				// Action type

	QString fileName;		// Name of the texture

public:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
	void setTexture(QString path);
	void SetUVMap(vector<Container> uv);
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

private:
    QPen pen;
    QBrush brush;
	bool antialiased;	// Antialiasing 
    QImage image;		// Background image
	bool moved;			// Indicates if user has moved a component -> update of position

	void VisitConnected();
	QRect GetRepeatVertex(float u, float v, int index);
	QRect GetClampVertex(float u, float v, int index);

signals:
	void UpdateStat(float u, float v, int faceN, int vertN, int countFace);

};

#endif