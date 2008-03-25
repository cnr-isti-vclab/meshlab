#ifndef TEXTUREEDITOR_H
#define TEXTUREEDITOR_H

#include <stdio.h>
#include <QWidget>
#include <meshlab/glarea.h>
#include "ui_textureeditor.h"
#include "renderarea.h"

#define MARGIN 5

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
	void SetStatusText(QString text);
	void SetProgressMax(int val);

	RenderArea *first;	// Ref to the first RA
	MeshModel *model;	// Ref to the model (for upate)
	GLArea *area;		// Ref to the GLArea (for refresh)

private:
	Ui::TextureEditorClass ui;
	int initialDim;
	void ResetLayout();

private slots:
	void on_editButton_clicked();
	void on_optimizeButton_clicked();
	void on_moveButton_clicked();
	void on_selectButton_clicked();
	void on_optionButton_clicked();
	void on_scaleButton_clicked();
	void on_rotateButton_clicked();
	void on_clampButton_clicked();
	void on_modulusButton_clicked();
	void on_buttonClear_clicked();
	void on_radioButtonMove_toggled(bool t);
	void on_radioButtonSelect_toggled(bool t);
	void on_radioButtonArea_toggled(bool t);
	void on_radioButtonConnected_toggled(bool t);

public slots:
	void UpStat(float u, float v, int faceN, int vertN, int countFace);

signals:
	void updateUV(float u, float v);
};

#endif // TEXTUREEDITOR_H
