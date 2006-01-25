#ifndef SHADERSDIALOG_H
#define SHADERSDIALOG_H

#include "shaderStructs.h"
#include "ui_shadersDialog.h"
#include <QLabel>
#include <QColor>
#include <QColorDialog>
#include <QInputDialog>
#include <QSignalMapper>
#include <map>

class ShadersDialog : public QDialog
{
    Q_OBJECT

public:
    ShadersDialog(ShaderInfo* sInfo, QWidget *parent = 0);
    ~ShadersDialog();

signals:
		void clicked(const QString &text);

private:
    Ui::ShadersDialogClass ui;
		ShaderInfo* shaderInfo;
		QSignalMapper *signalMapper;
		std::map<QString, QLabel*> labels;
		void setSimpleValue(QString varName);
		void setColorValue(QString varName);
		void setSliderValue(QString varName);

private slots:
		void changeClick(const QString &text);
		
};

#endif // SHADERSDIALOG_H
