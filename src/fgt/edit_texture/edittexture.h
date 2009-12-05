/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef EDITTEXTUREPLUGIN_H
#define EDITTEXTUREPLUGIN_H

#include <common/interfaces.h>
#include "textureeditor.h"
#include "ui_textureeditor.h"

class EditTexturePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
	public:
		EditTexturePlugin();
		virtual ~EditTexturePlugin();

		static const QString Info();
		virtual bool StartEdit(MeshModel & , GLArea * );
		virtual void EndEdit(MeshModel & , GLArea * );
	    virtual void Decorate(MeshModel &m , GLArea * );
		virtual void mousePressEvent(QMouseEvent *event, MeshModel & , GLArea * );
	    virtual void mouseMoveEvent(QMouseEvent *event, MeshModel & , GLArea * );
		virtual void mouseReleaseEvent (QMouseEvent *event, MeshModel & , GLArea * );
	    
	private:
		typedef enum {	SMAdd,		// Enumeration for mouse actions
						SMClear,
						SMSub
					 } SelMode;
		SelMode selMode;

		bool isDragging;// Indicates if the user is selecting faces
		QPoint start;	// Point for the selection area
		QPoint cur;
	    QPoint prev;
		TextureEditor* widget;
		QDockWidget* dock;
		QFont qFont;
		QString pluginName;
		vector<CMeshO::FacePointer> FaceSel;	// Set of selected faces


		Ui::TextureEditorClass ui;

		void DrawXORRect(GLArea * gla);
		void InitTexture(MeshModel &m);
		bool HasCollapsedTextCoords(MeshModel &m);

		bool degenerate;
	
	signals:
		void setSelectionRendering(bool);

};

#endif
