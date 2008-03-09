#ifndef TEXTUREEDITOR_H
#define TEXTUREEDITOR_H

#include <stdio.h>
#include <QWidget>
#include <meshlab/meshmodel.h>
#include <meshlab/glarea.h>
#include "ui_textureeditor.h"
#include "renderarea.h"
#include "Container.h"

#define MARGIN 5

using namespace std;

class TextureEditor : public QWidget
{
	Q_OBJECT

public:
	TextureEditor(QWidget *parent = 0);
	~TextureEditor();

	void AddRenderArea(QString texture, QHash<CVertexO*, Container> map, bool outOfRange);
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
	int oldX, oldY;		// Value stored to calcolate difference
	double oldU, oldV;	
	bool lock;			// Used to avoid double call of function..
	void HandleSpinBoxUV();
	void HandleSpinBoxXY();

private slots:
	void on_ApplyScale_clicked();
	void on_ApplyRotate_clicked();
	void on_spinBoxY_valueChanged(int);
	void on_spinBoxX_valueChanged(int);
	void on_tabWidgetEdit_currentChanged(int);
	void on_SpinBoxV_valueChanged(double);
	void on_SpinBoxU_valueChanged(double);
	void on_tabWidget_currentChanged(int);
	void on_applyButton_clicked();
	void on_remapRB_toggled(bool);
	void on_vertRB_toggled(bool);
	void on_faceRB_toggled(bool);

public slots:
	void UpStat(float u, float v, int faceN, int vertN, int countFace);

signals:
	void updateUV(float u, float v);
};

#endif // TEXTUREEDITOR_H
