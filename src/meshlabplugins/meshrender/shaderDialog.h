#ifndef SHADERDIALOG_H
#define SHADERDIALOG_H

#include <map>
#include <GL/glew.h>
#include <meshlab/meshmodel.h>
#include <QtGui>
#include "shaderStructs.h"
#include "ui_shaderDialog.h"

class ShaderDialog : public QDockWidget
{
    Q_OBJECT

public:
    ShaderDialog(ShaderInfo *sInfo, QGLWidget* gla, RenderMode &rm, QWidget *parent = 0);
    ~ShaderDialog();

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

private slots:
	void valuesChanged(const QString &);
	void setColorValue(const QString &);
	void setColorMode(int);
	void changeTexturePath(int);
	void browseTexturePath(int);
	void reloadTexture(int i);
		
};

#endif // SHADERDIALOG_H
