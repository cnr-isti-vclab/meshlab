#ifndef SHADERDIALOG_H
#define SHADERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDir>
#include <QColorDialog>
#include <QSignalMapper>
#include <QLineEdit>
#include <QSlider>
#include <QGridLayout>
#include <QCheckBox>
#include <map>
#include "shaderStructs.h"
#include "ui_shaderDialog.h"
#include <meshlab/glarea.h>

class ShaderDialog : public QDialog
{
    Q_OBJECT

public:
    ShaderDialog(ShaderInfo *sInfo, GLArea* gla, RenderMode &rm, QWidget *parent = 0);
    ~ShaderDialog();

private:
		GLArea* glarea;
		RenderMode * rendMode;
		ShaderInfo * shaderInfo;
		QSignalMapper *colorSignalMapper;
		QSignalMapper *valueSignalMapper;
		std::map<QString, QLabel*> labels;
		std::map<QString, QSlider*> sliders;
		std::map<QString, QLineEdit*> lineEdits;
    Ui::ShaderDialogClass ui;

private slots:
	void valuesChanged(const QString &);
	void setColorValue(const QString &);
	void setColorMode(int);
		
};

#endif // SHADERDIALOG_H
