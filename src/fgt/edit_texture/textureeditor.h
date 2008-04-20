#ifndef TEXTUREEDITOR_H
#define TEXTUREEDITOR_H

#include <stdio.h>
#include <QWidget>
#include <meshlab/glarea.h>
#include "ui_textureeditor.h"
#include "renderarea.h"

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
	void SetProgress(int val);
	void SetProgressMax(int val);

	RenderArea *first;	// Ref to the first RA
	MeshModel *model;	// Ref to the model (for upate)
	GLArea *area;		// Ref to the GLArea (for refresh)

private:
	Ui::TextureEditorClass ui;
	void ResetLayout();
	void SmoothTextureCoordinates();

private slots:
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

public slots:
	void UpdateModel();

};

#endif // TEXTUREEDITOR_H
