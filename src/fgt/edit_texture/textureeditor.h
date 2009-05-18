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

#ifndef TEXTUREEDITOR_H
#define TEXTUREEDITOR_H

#include <stdio.h>
#include <QWidget>
#include <meshlab/glarea.h>
#include "ui_textureeditor.h"
#include "renderarea.h"
#include <vcg/container/simple_temporary_data.h>	// da rimuovere una volta spostato smooth
#include <vcg/complex/trimesh/textcoord_optimization.h>

#define MARGIN 5
#define MAXW 1400
#define MAXH 1000

using namespace std;

class TextureEditor : public QWidget
{
	Q_OBJECT

	public:
		TextureEditor(QWidget *parent = 0, MeshModel *m = 0, GLArea *gla = 0, bool degenere = false);
		~TextureEditor();

		void AddRenderArea(QString texture, MeshModel *m, unsigned ind);
		void AddEmptyRenderArea();
		void Reset();
		void SelectFromModel();

	private:
		Ui::TextureEditorClass ui;
		void ResetLayout();
		void SmoothTextureCoordinates();
		MeshModel *model;	// Ref to the model (for upate)
		GLArea *area;		// Ref to the GLArea (for refresh)
		bool degenere;

	private slots:
		void on_tabWidget_currentChanged(int index);
		void on_connectedButton_clicked();
		void on_moveButton_clicked();
		void on_selectButton_clicked();
		void on_vertexButton_clicked();
		void on_clampButton_clicked();
		void on_modulusButton_clicked();
		void on_smoothButton_clicked();
		void on_cancelButton_clicked();
		void on_invertButton_clicked();	
		void on_flipHButton_clicked();
		void on_flipVButton_clicked();
		void on_unify2Button_clicked();
		void on_unifySetButton_clicked();
		void on_browseButton_clicked();

	public slots:
		void UpdateModel();

	signals:
		void updateTexture();

	protected:
		void keyPressEvent(QKeyEvent *event);

// Da mettere in VCG
template<class MESH_TYPE>
void SmoothTextureWEdgeCoords(MESH_TYPE &m, bool smoothWholeModel)
{
	assert(m.HasPerWedgeTexCoord());
	
	for (int i = 0; i < m.face.size(); i++)
	{
		if (!m.face[i].IsV() && (smoothWholeModel || m.face[i].IsS()))
		{
			vcg::SimpleTempData<typename MESH_TYPE::VertContainer, int> div(m.vert);
			vcg::SimpleTempData<typename MESH_TYPE::VertContainer, Point2f > sum(m.vert);

			for (typename MESH_TYPE::VertexIterator v = m.vert.begin(); v != m.vert.end(); v++) 
			{
				sum[v] = Point2f(0,0);
				div[v] = 0;
			}

			vector<CFaceO*> Q = vector<CFaceO*>();
			Q.push_back(&m.face[i]);
			int index = 0;
			m.face[i].SetV();
			// First iteration: calculate the alpha for each WT
			while (index < Q.size())
			{
				for (int j = 0; j < 3; j++)
				{
					CFaceO* p = Q[index]->FFp(j);
					if (!p->IsV() && (smoothWholeModel || p->IsS()))
					{
						p->SetV();
						Q.push_back(p);
					}
					div[Q[index]->V(j)] += 2;
					sum[Q[index]->V(j)].X() = sum[Q[index]->V(j)].X() + Q[index]->WT((j+1)%3).u() + Q[index]->WT((j+2)%3).u();
					sum[Q[index]->V(j)].Y() = sum[Q[index]->V(j)].Y() + Q[index]->WT((j+1)%3).v() + Q[index]->WT((j+2)%3).v();
				}
				index++;
			}
			index = 0;
			// Second iteration: apply the smooth
			while (index < Q.size())
			{
				for (int y = 0; y < 3; y++)
				{
					if (div[Q[index]->V(y)]>0)
					{
						Q[index]->WT(y).u() = sum[Q[index]->V(y)].X()/div[Q[index]->V(y)];
						Q[index]->WT(y).v() = sum[Q[index]->V(y)].Y()/div[Q[index]->V(y)];
					}
				}	
				index++;
			}
			if (!smoothWholeModel) break;
		}
	}
	for (typename MESH_TYPE::FaceIterator f = m.face.begin(); f != m.face.end(); f++) f->ClearV();
}

};

#endif // TEXTUREEDITOR_H