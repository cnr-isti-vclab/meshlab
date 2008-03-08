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

#include "ui_paintbox.h"

/**
 * The types of tools MUST be defined in the same order as
 * the buttons inside the tabs. 
 */ 
typedef enum {COLOR_PAINT, COLOR_FILL, COLOR_GRADIENT, COLOR_SMOOTH, COLOR_CLONE, COLOR_PICK, 
		MESH_SELECT, MESH_SMOOTH, MESH_PUSH, MESH_PULL} ToolType;

/**
 * Mnemonics to address the tabs numbers. These MUST be declared
 * in the same order as the actual tabs
 */
enum {COLOR_TAB, MESH_TAB, PREFERENCES_TAB};

//TODO add mnemonics for all settings

/**
 * This class manages the user interface and is concerned
 * with emitting appropriate signals. It should not be 
 * concerned with the application logic in any way.
 */
class Paintbox : public QWidget, private Ui::Paintbox 
{
	Q_OBJECT

private:
	ToolType active[2]; //the active tool for each page
	QUndoStack * stack;
	inline int getCurrentTab() {return tabs_container->currentIndex();}
	
public:	
	Paintbox(QWidget * parent = 0, Qt::WindowFlags flags = 0);
	
	inline void pushCommand(QUndoCommand* c) {stack->push(c);}
	
	inline QColor getForegroundColor() {return foreground_frame->getColor();}
	inline QColor getBackgroundColor() {return background_frame->getColor();}
	
	inline ToolType getCurrentType() {return active[tabs_container->currentIndex()];}
	inline int getSize() {
		if (getCurrentTab()==MESH_TAB) return mesh_size_slider->value(); 
		else return size_slider->value();}
	inline double getRadius() {return (double)getSize() / 100.0;}
	inline int getSizeUnit() { 
		if (getCurrentTab()==MESH_TAB) return mesh_pen_unit->currentIndex();
		else return pen_unit->currentIndex();}
	inline QPixmap getBrush(int w, int h) {
		if (getCurrentTab()==MESH_TAB) return mesh_brush_box->itemIcon(mesh_brush_box->currentIndex()).pixmap(w, h);
		else return brush_box->itemIcon(brush_box->currentIndex()).pixmap(w, h);}
	inline int getSearchMode() { return search_mode->currentIndex() + 1; } 
	inline int getOpacity() {return opacity_slider->value();}
	inline int getSmoothPercentual() {
		if (getCurrentTab()==MESH_TAB) return mesh_percentual_slider->value(); 
		else return percentual_slider->value();}
	inline int getHardness() {
		if (getCurrentTab() == MESH_TAB) return mesh_hardness_slider->value();
		else return hardness_slider->value();}
	inline bool getPaintBackFace() {
		if (getCurrentTab() == MESH_TAB) return backface_culling_2->isChecked();
		else return backface_culling->isChecked();}
	inline bool getPaintInvisible() { 
		if (getCurrentTab() == MESH_TAB) return invisible_painting_2->isChecked();
		else return invisible_painting->isChecked(); }
	inline int getGradientType() { return gradient_type->currentIndex(); }
	inline int getGradientForm() { return gradient_form->currentIndex(); }
	inline int getPickMode() { return pick_mode->currentIndex(); }
	inline bool getPressureDisplacement() {return displacement_box->isChecked();}
	inline bool getPressureHardness() {
		if (getCurrentTab() == MESH_TAB) return mesh_hardness_box->isChecked();
		else return hardness_box->isChecked(); }
	inline bool getPressureSize() {
		if (getCurrentTab() == MESH_TAB) return mesh_size_box->isChecked();
		else return size_box->isChecked(); }
	inline bool getPressureOpacity() {return opacity_box->isChecked();}
	inline int getDisplacement() {return mesh_displacement_slider->value();}
	inline int getDirection() {return mesh_displacement_direction->currentIndex();}
	inline QUndoStack * getUndoStack() {return stack;}
	
	void setUndoStack(QUndoStack * qus);
	
	void setForegroundColor(QColor & c);
	void setBackgroundColor(QColor & c);
	
	
public slots: 
	void on_pen_button_clicked() {active[COLOR_TAB] = COLOR_PAINT;}
	void on_fill_button_clicked() {active[COLOR_TAB] = COLOR_FILL;}
	void on_gradient_button_clicked() {active[COLOR_TAB] = COLOR_GRADIENT;}
	void on_smooth_button_clicked(){active[COLOR_TAB] = COLOR_SMOOTH;}
	void on_clone_button_clicked(){active[COLOR_TAB] = COLOR_CLONE;}
	void on_pick_button_clicked(){active[COLOR_TAB] = COLOR_PICK;}
	void on_mesh_pick_button_clicked(){active[MESH_TAB] = MESH_SELECT;}
	void on_mesh_smooth_button_clicked(){active[MESH_TAB] = MESH_SMOOTH;}
	void on_mesh_sculpt_button_clicked(){active[MESH_TAB] = MESH_PUSH;}
	void on_mesh_add_button_clicked(){active[MESH_TAB] = MESH_PULL;}
	void on_undo_button_clicked(){emit undo();}
	void on_redo_button_clicked(){emit redo();}
	void on_mesh_undo_button_clicked(){emit undo();}
	void on_mesh_redo_button_clicked(){emit redo();}
	void on_default_colors_clicked();
	void on_switch_colors_clicked();
	
signals: 
	void undo();
	void redo();
};

#endif /*PAINTBOX_H_*/
