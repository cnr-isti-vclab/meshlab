/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005                                                \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.                                                      *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
 * for more details.                                                         *
 *                                                                           *
 ****************************************************************************/

#ifndef EDITPAINT_H
#define EDITPAINT_H

#include <GL/glew.h>
#include <QObject>
#include <QDockWidget>

#include <wrap/gl/pick.h>
#include <meshlab/glarea.h>
#include <common/interfaces.h>

#include "paintbox.h"

/**
 * These options are chosen by each tool in the method setToolType(TolType t)
 */
enum PaintOptions {
	EPP_NONE 			= 0x00000,
	EPP_PICK_FACES		= 0x00001, //On press, pick faces		
	EPP_PICK_VERTICES 	= 0x00003, //On press, find the vertices (implies face picking too)
	EPP_AVG_NORMAL		= 0x00007, //During vertex picking, find the average normal of all vertices (implies vertex picking)
	EPP_DRAW_CURSOR		= 0x00008  //On mouse move display a shape representing the given cursor
};


/**
 * EditPaint plugin main class (MeshEditing plugin)
 */
class EditPaintPlugin : public QObject, public MeshEditInterface {
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)

public:
	EditPaintPlugin();
	virtual ~EditPaintPlugin();

	static const QString Info();

	virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void tabletEvent(QTabletEvent *, MeshModel & , GLArea *);

public slots:
	void update();
	void setToolType(ToolType t);
	void setBrushSettings(int size, int opacity, int hardness);
	
private:
	struct PickingData { QPoint position; QPointF rel_position; float distance;};
	
	void updateSelection(MeshModel &m, std::vector< std::pair<CVertexO *, PickingData> > * vertex_result = NULL);
	
	double modelview_matrix[16]; //modelview
	double projection_matrix[16]; //projection
	GLint viewport[4];
	
	GLArea * glarea;
	GLfloat* zbuffer; /*< last known zbuffer, always kept up-to-date */

	QDockWidget* dock; 
	Paintbox* paintbox; /*< The current Paintbox*/
	
	std::vector<CMeshO::FacePointer> * selection; //currently selected faces
	std::vector< std::pair<CVertexO *, PickingData> > vertices; //touched vertices during last updateSelection
	vcg::Point3f normal; //average normal of all vertices in "vertices"
	
	std::vector<QPointF> circle;
	std::vector<QPointF> dense_circle;
	std::vector<QPointF> square;
	std::vector<QPointF> dense_square;
	
	//New code under this line---
	
	//Memoization of type and type-settings
	ToolType 		current_type;
	int			 	current_options;
	
	//Input Events Handling:
	struct InputEvent { 
		Qt::MouseButton 		button;
		QEvent::Type 			type;
		QPoint					position;
		QPoint					gl_position;
		Qt::KeyboardModifiers	modifiers;
		qreal					pressure;
		bool					processed;
		bool					valid;
	};

	InputEvent 		latest_event;
	InputEvent		previous_event;
	
	inline void pushInputEvent(QEvent::Type t, QPoint p, 
			Qt::KeyboardModifiers k, qreal pres, Qt::MouseButton b, GLArea * gla)
	{
		if (latest_event.processed) previous_event = latest_event;
				
		latest_event.type = t;
		latest_event.position = p;
		latest_event.gl_position = QPoint(p.x(), gla->curSiz.height() - p.y());
		latest_event.modifiers = k;
		latest_event.button = b;
		latest_event.pressure = pres;
		latest_event.processed = false;
		latest_event.valid = true;
	}
	
	//Brush frequently accessed setting memoization
	struct Brush{
		int 	size;
		int 	opacity;
		int 	hardness;
		float	radius;
		bool	size_map;
		bool	opacity_map;
		bool	hardness_map;
	};
	
	Brush current_brush;
	
	/****** Painting, Cloning and Noise Tools ******/
	void paint(std::vector< std::pair<CVertexO *, PickingData> > * vertices);
	void capture();
	bool accessCloneBuffer(int, int, vcg::Color4b &);
	void computeNoiseColor(CVertexO * , vcg::Color4b &);
	QHash<CVertexO *, std::pair<vcg::Color4b, int> >	painted_vertices; /*<active vertices during painting */
	
	vcg::Color4b	color;
	
	GLubyte* 		color_buffer; /*< buffer used as color source in cloning*/
	GLfloat* 		clone_zbuffer; /*<buffer to determine if the source is legal or not */
	QPoint 			clone_delta;
	QPoint			source_delta;
	QPoint			apply_start;
	int				buffer_width;
	int				buffer_height;
		
	float			noise_scale;
	/****** Pull and Push Tools ******/
	void sculpt(MeshModel &, std::vector< std::pair<CVertexO *, PickingData> > * vertices);
	
	QHash<CVertexO *, std::pair<vcg::Point3f, float> > displaced_vertices; /*<active vertices during sculpting */

	
	/****** Gradient Tool ******/
	void gradient(MeshModel & m,GLArea * gla);
	
	QPoint gradient_start;
	
	
	/****** Color and Position Smoothing ******/
	void smooth(std::vector< std::pair<CVertexO *, PickingData> > * vertices);
	QHash<CVertexO *, CVertexO *> smoothed_vertices;
	
	/****** Color Fill ******/
	void fill(MeshModel & m,CFaceO * face);
};

/**
 * Undo Class to store color changes. Intergrates in the Qt Undo framework.
 * Undoes the change to a single vertex only. Must use the Command compression
 * system by Qt.
 * What if the vertex has been deleted in the meantime?
 */
class SingleColorUndo : public QUndoCommand
{
	
public:
	SingleColorUndo(CVertexO * v, vcg::Color4b c, QUndoCommand * parent = 0) : QUndoCommand(parent){
		vertex = v; original = c; // setText("Single Vertex Color Change");
	}
	
	virtual void undo() {vcg::Color4b temp = vertex->C(); vertex->C() = original; original = temp;}
	virtual void redo() {undo();}
	virtual int id() {return COLOR_PAINT;}
	
private:
	CVertexO* vertex;
	vcg::Color4b original;
};

//TODO Create MultipleColorUndo

class SinglePositionUndo : public QUndoCommand
{
	
public:
	SinglePositionUndo(CVertexO * v, vcg::Point3f p, vcg::Point3f n, QUndoCommand * parent = 0) : QUndoCommand(parent){
		vertex = v; original = p; normal = n;
	}
	
	virtual void undo() {vcg::Point3f temp = vertex->P(); vertex->P() = original; original = temp;
						 temp = vertex->N(); vertex->N() = normal; normal = temp;}
	virtual void redo() {undo();}
	virtual int id() {return MESH_PULL;}
	
private:
	CVertexO* vertex;
	vcg::Point3f original;
	vcg::Point3f normal;
};


/**
 * Undoes changes in selected faces.
 * 
 * Same doubt as above about vertex deletion!
 */
class SelectionUndo : public QUndoCommand
{
public:
	SelectionUndo(CFaceO * f, bool sel, QUndoCommand * parent = 0) : QUndoCommand(parent){
		face = f; original = sel; // setText("Single Vertex Color Change");
	}
	
	virtual void undo() {bool temp = face->IsS(); original? face->SetS() : face->ClearS(); original = temp;}
	virtual void redo() {undo();}
	virtual int id() {return MESH_SELECT;}
private :
	CFaceO * face;
	bool original;
};

/*********Generic Utility Routines************/

/** 
 * Colorizes a vertex with the new color.

 * Has been converted in a for loop. No performance
 * loss since compiler will unroll constant loops
 *  
 * O(1) complexity
 */
inline void applyColor(CVertexO * vertex, const vcg::Color4b& newcol, int opac)
{
	vcg::Color4b orig =  vertex->C();
	
	opac *= ((float)newcol[3]/255.0);
	
	for (int i = 0; i < 3; i ++) 
		orig[i] = vcg::math::Min(255, ( (newcol[i]-orig[i]) * opac + orig[i] * 100 ) / 100);
	
	vertex->C() = orig;
}

/**  
 * Same modification as above
 * 
 * O(1) complexity 
 */
inline void mergeColors(double percent,const vcg::Color4b& c1,const vcg::Color4b& c2, vcg::Color4b* dest)
{
	for (int i = 0; i < 4; i ++)
		(*dest)[i] = (char)vcg::math::Min(255.0,((c1[i]-c2[i])*percent+c2[i]));
}

/** 
 * The weighted average of the two vectors c1 and c2 is
 * stored in dest. c1's weight is given by percent, while
 * c2's weight is given by 1.0 - percent
 * 
 * Can actually merge any 3d array, not just positions  
 */
inline void mergePositions(double percent,const float c1[3],const float c2[3], float dest[3])
{
	for (int i = 0; i < 3; i ++)
		dest[i] = c1[i]*percent+c2[i]*(1.0-percent);
}

/*********Geometric Utility Routines************/

/** 
 * Quickly ransforms a 3 point with a 44 matrix, 
 * avoiding unnecessary computations (the w component is 
 * assumed to be 1)
 *
 * Original Code by Gfrei Andreas 
 */
inline void fastMultiply(float x,float y,float z,double matrix[],double *xr,double *yr,double *zr)
{
	*xr = x * matrix[0] + y * matrix[4] + z * matrix[8] + matrix[12];
	*yr = x * matrix[1] + y * matrix[5] + z * matrix[9] + matrix[13];	
	*zr = x * matrix[2] + y * matrix[6] + z * matrix[10] + matrix[14];
}

/**
 * Finds the nearest point to "center" among "points".
 * 
 * O(num)
 */
inline int getNearest(QPointF center, QPointF *points, int num) 
{
	int index = 0;
	
	float dist = fabsf(center.x()-points[0].x())*fabsf(center.x()-points[0].x())+fabsf(center.y()-points[0].y())*fabsf(center.y()-points[0].y());
	
	float temp = 0;
	
	for (int i=1; i < num; i++) 
	{
		temp = fabsf(center.x()-points[i].x())*fabsf(center.x()-points[i].x())+
			fabsf(center.y()-points[i].y())*fabsf(center.y()-points[i].y());
		
		if (temp < dist) {
			index = i;
			dist = temp;
		}
	}
	return index;
}

/** 
 * Checks if a point (x, y) is contained in either:
 * 
 * a) the rectangle aligned to the segment connecting p0 to p1
 *    whose height is the distance between p0 and p1 and width
 *    is radius * 2
 * b) the circle centered in p0 with given radius
 * c) the circle centered in p1 with same radius 
 *
 * To check condition (a): let v be the vector going from p0 to p1
 * and w the vector from p0 to (x, y). If theta is the angle between
 * v and w, then: 
 * 
 *     |v| |w| cos(theta)     |w|
 * r = ------------------  =  --- cos(theta)
 *            |v|²            |v|
 *
 * is the ratio between the length of vector v and the projection
 * of vector w on vector v.
 * 
 * Since w' = v * r is the projection of w on v, pf = p0 + w' is
 * the orthogonal projection of (x, y) on the segment 
 * connecting p0 to p1. If the distance between (x, y) and pf is less
 * than radius, then (x, y) lays inside the rectangle  
 *  
 */
inline bool isIn(const QPointF &p0,const QPointF &p1, float x,float y,float radius, float *dist, QPointF &pos)
{
	float radius_sq = radius * radius;
	
	if (p0 != p1) //otherwise condition (a) needs not to be tested: rectangle is null
	{ 
		float v_x = (p1.x()-p0.x()); 
		float v_y = (p1.y()-p0.y());
		
		float v_length_squared = v_x * v_x + v_y * v_y;
		
		float w_x = x - p0.x(); 
		float w_y = y - p0.y();
			
		float v_dot_w = w_x * v_x + w_y * v_y; //scalar product of v and w 
		
		float r = v_dot_w / v_length_squared;
		
		float pf_x = p0.x() + r * v_x;
		float pf_y = p0.y() + r * v_y;
	
		float delta_x = x - pf_x;
		float delta_y = y - pf_y; 
	
		if (r >= 0 && r <= 1 && (delta_x * delta_x + delta_y * delta_y < radius_sq)) 
		{ 
			float delta_len = sqrt(delta_x * delta_x + delta_y * delta_y); 
			
			*dist = delta_len / radius;
			pos.setY(delta_y / radius);
			pos.setX(delta_x / radius); 
			return true; 
		}
	}

	// there could be some problem when point is nearer p0 or p1 and viceversa
	// so i have to check both. is only needed with smooth_borders
	bool found = false;
	float dx=(x-p1.x());
	float dy=(y-p1.y());
	float delta_len_sq =dx*dx+dy*dy;
	
	if (delta_len_sq < radius_sq) 
	{ 
		float delta_len = sqrt(delta_len_sq);
		*dist = delta_len; 
		pos.setY(dy / radius);
		pos.setX(dx / radius);
		found = true; 
	}

	if (p0 == p1){
		*dist /= radius;
		return found;
	}

	dx=(x-p0.x());
	dy=(y-p0.y());
	delta_len_sq =dx*dx+dy*dy;
	
	if (delta_len_sq < radius_sq) 
	{	
		float delta_len = sqrt(delta_len_sq);
		if ( (found && delta_len < *dist ) || !found)	
		{
			*dist = delta_len;
		//	pos.setY(x / delta_len );
			pos.setY(dy / radius);
			pos.setX(dx / radius);
		}
		found = true;
	}
	
	*dist /= radius;
	
	return found;
} 

inline bool pointInTriangle(const float p_x,const float p_y,const float a_x,const float a_y,
		const float b_x,const float b_y,const float c_x,const float c_y) 
{
	float fab=(p_y-a_y)*(b_x-a_x) - (p_x-a_x)*(b_y-a_y);
	float fbc=(p_y-c_y)*(a_x-c_x) - (p_x-c_x)*(a_y-c_y);
	float fca=(p_y-b_y)*(c_x-b_x) - (p_x-b_x)*(c_y-b_y);
	if (fab*fbc>0 && fbc*fca>0) return true;
	return false;
}

/** 
 * checks if a point is in a triangle (2D) 
 */
inline bool pointInTriangle(const QPointF &p,const QPointF &a,const QPointF &b,const QPointF &c) 
{
	return pointInTriangle(p.x(), p.y(), a.x(), a.y(), b.x(), b.y(), c.x(), c.y());
}

/** 
 * checks if a triangle is front or backfaced 
 */
inline bool isFront(const float a_x, const float a_y, const float b_x, const float b_y, const float c_x, const float c_y) {
	return (b_x-a_x)*(c_y-a_y)-(b_y-a_y)*(c_x-a_x)>0;
}

/**
 * checks if a triangle is front or backfaced
 */
inline bool isFront(const QPointF &a,const QPointF &b,const QPointF &c) {
	return isFront(a.x(), a.y(), b.x(), b.y(), c.x(), c.y());
}


/**
 * Checks if a line from LineStart to LineEnd intersects the circle defined
 * by CircleCenter and Radius in one or two points.
 * 
 * pOut will either store
 * a) LineStart, if it is inside the circle
 * b) The one intersection between the line and the circle
 * c) One of the two intersections 
 * 
 * Let L(t) = LineStart + (t * Direction) the parametric equation
 * of the line, with 0 <= t <= 1 and Dircetion = LineEnd - LineStart
 * 
 * Let |P - CircleCenter|² = radius² be the equation of the circle. Then, substituting L(t) to P
 * 
 * |LineStart + (t * Direction) - CircleCenter|² = radius²
 * 
 * Let Delta be LineStart - CircleCenter, then the equation above becomes
 * 
 * t² * |Direction|² + 2 * t * (Direction · Delta) + |Delta|² - radius² = 0
 * 
 * whose determinant is
 * 
 * d = (Direction · Delta)² − |Direction|²(|Delta|² − radius²)
 * 
 * if d < 0 there are no intersection, if d = 0 there is one, if d > 0 there are two.
 */ 
inline bool lineHitsCircle(QPointF& LineStart,QPointF& LineEnd,QPointF& CircleCenter, float radius) 
{
	const float radius_sq = radius * radius;
	QPointF Delta = LineStart - CircleCenter;
	
	float delta_length_sq = Delta.x()*Delta.x()+Delta.y()*Delta.y();
	
	if(delta_length_sq <= radius_sq) return true;
	
	QPointF Direction = LineEnd - LineStart;
	
	const float direction_dot_delta = Direction.x()*Delta.x()+Direction.y()*Delta.y(); 
	
	const float direction_length_sq = Direction.x()*Direction.x()+Direction.y()*Direction.y();
	
	const float d = (direction_dot_delta * direction_dot_delta) - direction_length_sq * (delta_length_sq - radius_sq);
	
	if(d < 0.0f) return false; //no intersection
	else if(d < 0.0001f) //one intersection 
	{ 
		const float s = - direction_dot_delta / direction_length_sq;
		if(s < 0.0f || s > 1.0f) return false;
		else return true;
	}else //two intersections 
	{
		const float s = (- direction_dot_delta - sqrtf(d)) / direction_length_sq; //only one intersection is chosen
		if(s < 0.0f || s > 1.0f) return false;
		else return true;
	}
}

/*********CMeshO Utility Routines************/

/**
 * Simple displacement of vertex along its normal. Make sure normals
 * are precomputed!!
 * 
 * O(1)
 */
inline void displaceAlongVector(CVertexO* vp, vcg::Point3f vector, float displacement)
{
	(*vp).P() += vector * displacement;
}

/**
 * Get the vertex currently pointed by the mouse at position cursor on the mesh m
 * This vertex is obtained by first finding the face nearest to the mouse, and then
 * finding the nearest vertex to the cursor among the face vertices.
 * 
 * Same complexity as GLPickTri<CMeshO>::PickNearestFace(..)
 */
inline bool getVertexAtMouse(MeshModel &m,CMeshO::VertexPointer& value, QPoint& cursor,
		double* modelview_matrix, double* projection_matrix, GLint* viewport)
{
	CFaceO * fp = NULL;

	double tx, ty, tz;

	//TODO e se i vertici sono stati cancellati? (IsD())
	
	if (vcg::GLPickTri<CMeshO>::PickNearestFace(cursor.x(), cursor.y(), m.cm, fp, 2, 2)) 
	{
		
		QPointF point[3];
		
		for (int i = 0; i < 3; i++) 
		{
			gluProject(fp->V(i)->P()[0], fp->V(i)->P()[1], fp->V(i)->P()[2], 
					modelview_matrix,
					projection_matrix,
					viewport, 
					&tx, &ty, &tz);
			
			point[i] = QPointF(tx,ty);
		}
		
		value = fp->V(getNearest(cursor, point, 3));
		
		return true;
	}	
	return false;
}


/** 
 * calcs the surrounding faces of a vertex with VF topology
 */
inline void getSurroundingFacesVF(CFaceO * fac,int vert_pos, std::vector<CFaceO *> *surround) {
	CVertexO * vert=fac->V(vert_pos);
	int pos=vert->VFi();
	CFaceO * first_fac=vert->VFp();
	CFaceO * curr_f=first_fac;
	do {
		CFaceO * temp=curr_f->VFp(pos);
		if (curr_f!=0 && !curr_f->IsD()) {
			surround->push_back(curr_f);
			pos=curr_f->VFi(pos);
		}
		curr_f=temp;
	} while (curr_f!=first_fac && curr_f!=0);
}

inline bool hasSelected(MeshModel &m) {
	CMeshO::FaceIterator fi;
	for(fi=m.cm.face.begin();fi!=m.cm.face.end();fi++) {
		if (!(*fi).IsD() && (*fi).IsS()) return true;
	}
	return false;
}

inline void updateNormal(CVertexO * v) 
{
	CFaceO * f = v->VFp();
	CFaceO * one_face = f;
	int pos = v->VFi();
	v->N()[0]=0;v->N()[1]=0;v->N()[2]=0;
	do {
		CFaceO * temp=one_face->VFp(pos);
		if (one_face!=0 && !one_face->IsD()) 
		{
			vcg::face::ComputeNormalizedNormal(*one_face);
	/*		for (int lauf=0; lauf<3; lauf++) 
				if (pos!=lauf) { 
					v->N()+=one_face->V(lauf)->cN();
				} 
			vcg::face::ComputeNormalizedNormal(*one_face); */
			v->N() += one_face->N();
			pos=one_face->VFi(pos);
		}
		one_face=temp;
	} while (one_face!=f && one_face!=0);
	v->N().Normalize();
}

/*********OpenGL Drawing Routines************/


void drawNormalPercentualCircle(GLArea *, QPoint &, MeshModel &, GLfloat* , double* , double* , GLint* , float );
void drawVertex(CVertexO* );
void drawLine(GLArea *, QPoint &, QPoint &);
void drawSimplePolyLine(GLArea * gla, QPoint & gl_cur, float scale, std::vector<QPointF> * points);
void drawPercentualPolyLine(GLArea * , QPoint &, MeshModel &, GLfloat* , double* , double* , GLint* , float , std::vector<QPointF> * );

void generateCircle(std::vector<QPointF> &, int segments = 18);
void generateSquare(std::vector<QPointF> &, int segments = 1);

#endif
