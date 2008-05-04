#ifndef TEXTUREEDITOR_H
#define TEXTUREEDITOR_H

#include <stdio.h>
#include <QWidget>
#include <meshlab/glarea.h>
#include "ui_textureeditor.h"
#include "renderarea.h"
#include <vcg/container/simple_temporary_data.h>

#define MARGIN 5
#define MAXW 1400
#define MAXH 1000

using namespace std;

class TextureEditor : public QWidget
{
	Q_OBJECT

public:
	TextureEditor(QWidget *parent = 0);
	~TextureEditor();

	void AddRenderArea(QString texture, MeshModel *m, unsigned ind);
	void AddEmptyRenderArea();
	void Reset();
	void SelectFromModel();

	RenderArea *first;	// Ref to the first RA
	MeshModel *model;	// Ref to the model (for upate)
	GLArea *area;		// Ref to the GLArea (for refresh)

private:
	Ui::TextureEditorClass ui;
	void ResetLayout();
	void SmoothTextureCoordinates();

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
	void on_checkBox_stateChanged();

public slots:
	void UpdateModel();

protected:
	void keyPressEvent(QKeyEvent *event);


template<class MESH_TYPE>
void SmoothTextureWEdgeCoords(MESH_TYPE &m, float alpha)
{
	assert(m.HasPerWedgeTexCoord());

	vcg::SimpleTempData<typename MESH_TYPE::VertContainer, int> div(m.vert);
	vcg::SimpleTempData<typename MESH_TYPE::VertContainer, Point2f > sum(m.vert);
  
	div.Start();
	sum.Start();
  
	for (typename MESH_TYPE::VertexIterator v = m.vert.begin(); v != m.vert.end(); v++) 
	{
		sum[v] = Point2f(0,0);
		div[v] = 0;
	}

	for (typename MESH_TYPE::FaceIterator f = m.face.begin(); f != m.face.end(); f++)
	{
		for (int i = 0; i < 3; i++)
		{
			div[f->V(i)] += 2; 
			sum[f->V(i)].X() = sum[f->V(i)].X() + f->WT((i+1)%3).u() + f->WT((i+2)%3).u();
			sum[f->V(i)].Y() = sum[f->V(i)].Y() + f->WT((i+1)%3).v() + f->WT((i+2)%3).v();
		}
	}

	for (typename MESH_TYPE::FaceIterator v = m.face.begin(); v != m.face.end(); v++)
	{
		for (int i = 0; i < 3; i++)
		{
			if (div[v->V(i)]>0)
			{
				if (alpha == -1)
				{
					v->WT(i).u() = sum[v->V(i)].X()/div[v->V(i)];
					v->WT(i).v() = sum[v->V(i)].Y()/div[v->V(i)];
				}
				else
				{
					v->WT(i).u() = sum[v->V(i)].X()*alpha;
					v->WT(i).v() = sum[v->V(i)].Y()*alpha;
				}
			}
		}
	}
	
	div.Stop();
	sum.Stop();
}

};

#endif // TEXTUREEDITOR_H