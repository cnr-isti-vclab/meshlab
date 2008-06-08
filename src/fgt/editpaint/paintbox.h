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

#ifndef PAINTBOX_H_
#define PAINTBOX_H_

#include <vector>

#include <vcg/math/base.h>
#include <GL/glew.h>
#include "ui_paintbox.h"

/**
 * The types of tools MUST be defined in the same order as
 * the buttons inside the tabs. 
 */ 
typedef enum {COLOR_PAINT, COLOR_FILL, COLOR_GRADIENT, COLOR_SMOOTH, COLOR_CLONE, COLOR_PICK, COLOR_NOISE,
		MESH_SELECT, MESH_SMOOTH, MESH_PUSH, MESH_PULL} ToolType;

		
/**
 * Mnemonics to address the tabs numbers. These MUST be declared
 * in the same order as the actual tabs
 */
enum {COLOR_TAB, MESH_TAB, PREFERENCES_TAB};

//TODO add mnemonics for all settings

typedef enum {CIRCLE, SQUARE, PIXMAP} Brush;

/**
 * This class manages the user interface and is concerned
 * with emitting appropriate signals. It should not be 
 * concerned with the application logic in any way.
 */
class Paintbox : public QWidget, private Ui::Paintbox 
{
	Q_OBJECT

private:
	ToolType active; //the active tool for each page
//	QUndoStack * stack;
	
	QHash<QWidget *, QUndoStack *> stack_association;
	
	QUndoGroup * stacks;
	
	ToolType previous_type;

	QGraphicsPixmapItem * item;
	bool pixmap_available;
	QPoint item_delta;
	
public:	
	Paintbox(QWidget * parent = 0, Qt::WindowFlags flags = 0);
	
	inline void pushCommand(QUndoCommand* c) {stacks->activeStack()->push(c);}
	
	inline QColor getForegroundColor() {return foreground_frame->getColor();}
	inline QColor getBackgroundColor() {return background_frame->getColor();}
	
	inline ToolType getCurrentType() {return active;}
	inline int getSize() {return size_slider->value();}
	inline double getRadius() {return (double)getSize() / 100.0;}
	inline int getSizeUnit() {return pen_unit->currentIndex();}
	inline Brush getBrush() {return (Brush)brush_box->currentIndex();}
	inline int getSearchMode() { return search_mode->currentIndex() + 1; } 
	inline int getOpacity() {return opacity_slider->value();}
	inline int getSmoothPercentual() {return percentual_slider->value();}
	inline int getHardness() {return hardness_slider->value();}
	inline bool getPaintBackFace() {return backface_culling->isChecked();}
	inline bool getPaintInvisible() {return invisible_painting->isChecked(); }
	inline int getGradientType() { return gradient_type->currentIndex(); }
	inline int getGradientForm() { return gradient_form->currentIndex(); }
	inline int getPickMode() { return pick_mode->currentIndex(); }
	inline float getNoiseSize() {return noise_slider->value();}
	inline bool getPressureDisplacement() {return displacement_box->isChecked();}
	inline bool getPressureHardness() {return hardness_box->isChecked();}
	inline bool getPressureSize() {return size_box->isChecked(); }
	inline bool getPressureOpacity() {return opacity_box->isChecked();}
	inline int getDisplacement() {return mesh_displacement_slider->value();}
	inline int getDirection() {return mesh_displacement_direction->currentIndex();}
	inline QUndoStack * getUndoStack() {return stacks->activeStack();}
	
	
	void setUndoStack(QWidget * parent);
	
	void setForegroundColor(QColor & c);
	void setBackgroundColor(QColor & c);
	
	//Brush preview
	void refreshBrushPreview();
	
	//Cloning
	inline QGraphicsScene * getCloneScene() {return clone_source_view->scene();}
	inline QGraphicsPixmapItem * getClonePixmap() {return item;}
	void getPixmapBuffer(GLubyte * & cbuffer, GLfloat* & zbuffer, int & w, int & h);
	inline bool isNewPixmapAvailable(){return pixmap_available;}
	 
	void setClonePixmap(QImage & image);
//	void setPixmapCenter(qreal x, qreal y);
	QPoint getPixmapDelta();
	void setPixmapOffset(qreal x, qreal y);
	void loadClonePixmap();
	void restorePreviousType();

signals: 
	void undo();
	void redo();
	void typeChange(ToolType t);
	void brushSettingsChange(int size, int opacity, int hardness);
	
public slots: 
	void on_pen_button_toggled(bool checked) {if(checked) {active = COLOR_PAINT; emit typeChange(active);}}
	void on_fill_button_toggled(bool checked) {if(checked) {active = COLOR_FILL;emit typeChange(active);}}
	void on_gradient_button_toggled(bool checked) {if(checked) {active = COLOR_GRADIENT; emit typeChange(active);}}
	void on_smooth_button_toggled(bool checked){if(checked) {active = COLOR_SMOOTH;emit typeChange(active);}}
	void on_clone_button_toggled(bool checked){if(checked) {active = COLOR_CLONE; emit typeChange(active);} clone_source_frame->setVisible(checked);}
	void on_pick_button_toggled(bool checked){if(checked) {previous_type = active; active = COLOR_PICK; emit typeChange(active);}}
	void on_mesh_pick_button_toggled(bool checked){if(checked) {active = MESH_SELECT; emit typeChange(active);}}
	void on_mesh_smooth_button_toggled(bool checked){if(checked) {active = MESH_SMOOTH; emit typeChange(active);}}
	void on_mesh_sculpt_button_toggled(bool checked){if(checked) {active = MESH_PUSH; emit typeChange(active);}}
	void on_mesh_add_button_toggled(bool checked){if(checked) {active = MESH_PULL; emit typeChange(active);}}
	void on_perlin_button_toggled(bool checked){if(checked) {active = COLOR_NOISE; emit typeChange(active);}}
	void on_undo_button_clicked(){emit undo();}
	void on_redo_button_clicked(){emit redo();}
	void on_default_colors_clicked();
	void on_switch_colors_clicked();
	void on_brush_box_currentIndexChanged(int){refreshBrushPreview();}
	void on_hardness_slider_valueChanged(int){refreshBrushPreview();}
	void on_size_slider_valueChanged(int){refreshBrushPreview();}
	void on_clone_source_load_button_clicked(){loadClonePixmap();}
	void on_mesh_displacement_slider_valueChanged(int val){displacement_spin_box->setValue((double)val);}
	void on_displacement_spin_box_valueChanged(double val){mesh_displacement_slider->setValue((int)val);}
	void on_noise_slider_valueChanged(int val){noise_box->setValue((double)val);}
	void on_noise_box_valueChanged(double val){noise_slider->setValue((int)val);}
		
	void setPixmapDelta(double x, double y);
	void movePixmapDelta(double x, double y);
};

/******Brush Shapes******/ 

/**
 * Returns the "value" of the brush at the given distance from the center
 */
inline float brush(Brush b, float distance, float dx, float dy, float hardness)
{
	float op = 0.0; /*< 0 means totally colored, 1 totally blank, but right before returning, its meaning will be the opposite*/
	
	switch (b) 
	{
		case CIRCLE : //circle
			if (distance >= 1.0) op = 1.0;
			else if (distance * 100.0 > hardness) //if after the treshold of total color 
			{
				op = (distance * 100.0 - hardness)/ (float) (100 - hardness);
			}
			op = 1 - op;
			break;
			
		case SQUARE : 
			dx = vcg::math::Abs(dx * 141.4213562); dy = vcg::math::Abs(dy * 141.4213562); //multiply by sqrt(2) and by 100
			if (dx >= 100 || dy >= 100 ) op = 1.0;
			else if (dx > hardness || dy > hardness) 
			{
				op = (vcg::math::Max<float>(dx, dy) - hardness)/ (float) (100 - hardness);
			}
			op = 1 - op;
			break;
		
		case PIXMAP :
			
		default:
			break;
	}
	
	return op;
}

/**
 * Rasterizes a given brush at the given hardness as a w x h image
 */
inline QImage raster(Brush b, int w, int h, float hardness)
{
	float cx = w/2.0;
	float cy = h/2.0;
	QImage image(w, h, QImage::Format_RGB32); 
	for (float x = 0; x < w; x++){
		for (float y = 0; y < h; y++){
			float _x = ((x - cx) * 1.4142 * 2)/w;
			float _y = ((y - cy) * 1.4142 * 2)/h;	
			int op = 255 * (1 - brush(b, sqrt(_x * _x + _y * _y), _x, _y, hardness));
			image.setPixel(x, y, qRgb(op, op, op));
		}
	}
	return image;
}

#endif /*PAINTBOX_H_*/
