#ifndef DIALOG_H
#define DIALOG_H

#include <map>
#include <GL/glew.h>
#include <common/interfaces.h>
#include <QtGui>
#include <QGLWidget>
#include "shaderStructs.h"
#include "ui_dialog.h"

#include "point_based_renderer.h"

class Dialog : public QDockWidget
{
    Q_OBJECT

public:
  Dialog(PointBasedRenderer* r, QGLWidget* gla, QWidget *parent = 0);
    ~Dialog();

private:
		QGLWidget* glarea;
		RenderMode * rendMode;
		ShaderInfo * shaderInfo;
		QSignalMapper *colorSignalMapper;
		QSignalMapper *valueSignalMapper;
		QSignalMapper *textLineSignalMapper;
		QSignalMapper *textButtonSignalMapper;
		std::map<QString, QLabel*> labels;
		std::map<QString, QSlider*> sliders;
		std::vector<QLineEdit*> textLineEdits;
		std::map<QString, QLineEdit*> lineEdits;
		Ui::ShaderDialogClass ui;

		PointBasedRenderer* renderer;

private slots:

	void setColorMode(int);
	void setRadiusSize(double);
	void setPrefilterSize(double);
	void setMinimumSize(double);

	void changeTexturePath(int);
	void browseTexturePath(int);
	void reloadTexture(int i);
		
};

#endif // DIALOG_H
