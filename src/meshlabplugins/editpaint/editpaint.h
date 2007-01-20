#ifndef EDITPAINT_H
#define EDITPAINT_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QDockWidget>
#include <QHash>
#include <QDialog>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/math/matrix44.h>
#include "ui_painttoolbox.h"
#include "colorwid.h"

class EditPaintPlugin;
class PaintToolbox;
class Penn;

typedef enum {PEN, FILL, PICK} PaintThing;

class Penn {
public:
	QPoint width;
	QPoint pos;
};

class EditPaintPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
	QList <QAction *> actionList;

public:
	EditPaintPlugin();
	
	virtual ~EditPaintPlugin();
	
	virtual const QString Info(QAction *);
	virtual const PluginInfo &Info();
	
	virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void mousePressEvent    (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void mouseMoveEvent     (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void mouseReleaseEvent  (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	//	  virtual void wheelEvent         (QAction *QWheelEvent*e, MeshModel &/*m*/, GLArea * );
	virtual QList<QAction *> actions() const ;
	QPoint start;
	QPoint cur;
	QPoint prev;
	bool isDragging;
	vector<CMeshO::FacePointer> LastSel;

private:

	bool pressed; // to check in decorate if it is the first call after a mouse click
	typedef enum {SMAdd, SMClear,SMSub} SelMode;
	SelMode selMode;


	double mvmatrix[16];
	double projmatrix[16];
	int viewport[4];

	vector<CMeshO::FacePointer> tempSel; //to use when needed
	vector<CMeshO::FacePointer> curSel; //the faces i am painting on
	QHash<CVertexO *,Color4b> temporaneo;
	Penn pen;
	//QHash <long,face::Color4bOcf> dipendendo;
	QHash <long,CFaceO *> painting_at;
	
	PaintToolbox* paintbox;
	Qt::MouseButton curr_mouse;

	GLfloat *pixels;
	bool first;
	QPoint old_size;

	int inverse_y; // gla->curSiz.height()
	int paintType();
	void DrawXORRect(GLArea * gla, bool doubleDraw);
	void getInternFaces(vector<CMeshO::FacePointer> *actual,vector<CMeshO::VertexPointer> * risult, GLArea * gla);
	int isIn(QPointF p0,QPointF p1,float dx,float dy,float raduis);
	bool getFaceAtMouse(MeshModel &,CMeshO::FacePointer &);
	bool getFacesAtMouse(MeshModel &,vector<CMeshO::FacePointer> &);
	bool getVertexAtMouse(MeshModel &,CMeshO::VertexPointer &);
	bool getVertexesAtMouse();

	void fillFrom(MeshModel &,CFaceO *);

	inline void updateMatrixes() {		
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
	}

	inline int getNearest(QPointF center, QPointF punti[],int num) {
		int near=0;
		float dist=fabsf(center.x()-punti[0].x())*fabsf(center.x()-punti[0].x())+fabsf(center.y()-punti[0].y())*fabsf(center.y()-punti[0].y());
		for (int lauf=1; lauf<num; lauf++) {
			float temp=fabsf(center.x()-punti[lauf].x())*fabsf(center.x()-punti[lauf].x())+
				fabsf(center.y()-punti[lauf].y())*fabsf(center.y()-punti[lauf].y());
			if (temp<dist) {
				near=lauf;
				dist=temp;
			}
		}
		return near;
	}
	
	inline void colorize(CVertexO * vertice,const Color4b& newcol,int opac) {
		Color4b orig=vertice->C();
		orig[0]=min(255,(newcol[0]*opac+orig[0]*(100-opac))/100);
		orig[1]=min(255,(newcol[1]*opac+orig[1]*(100-opac))/100);
		orig[2]=min(255,(newcol[2]*opac+orig[2]*(100-opac))/100);
		orig[3]=min(255,(newcol[3]*opac+orig[3]*(100-opac))/100);
		vertice->C()=orig;
	}

};

class PaintToolbox : public QWidget {
Q_OBJECT
public:
	float diag;
	PaintToolbox ( /*const QString & title,*/ QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	Color4b getColor(Qt::MouseButton);
	void setColor(Color4b,Qt::MouseButton);
	inline double getRadius() { return ui.pen_radius->value(); }
	inline int paintType() { if (ui.pen_type->currentText()=="pixel") return 1; return 2; }
	inline int getOpacity() { return ui.deck_slider->value(); }
	inline int paintUtensil() { return paint_utensil; }
	inline bool getPaintBackface() { return ui.backface_culling->checkState()!=Qt::Unchecked; }
	inline bool getPaintInvisible() { return ui.invisible_painting->checkState()!=Qt::Unchecked; }
private:
	int paint_utensil;
	Ui::PaintToolbox ui;
private slots:
	void on_pen_type_currentIndexChanged(QString value);
	void on_pen_radius_valueChanged(double value);
	void on_switch_me_clicked();
	void on_set_bw_clicked();
	void on_deck_slider_valueChanged(int value);
	void on_deck_box_valueChanged(int value);
	void on_pen_button_clicked();
	void on_fill_button_clicked();
	void on_pick_button_clicked();
	void on_backface_culling_stateChanged(int value);
	void on_invisible_painting_stateChanged(int value);
};

#endif
