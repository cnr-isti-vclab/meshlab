#ifndef SHADERDIALOG_H
#define SHADERDIALOG_H

#include <QDialog>
#include <QDir>
#include "shaderStructs.h"
#include "ui_shaderDialog.h"

class ShaderDialog : public QDialog
{
    Q_OBJECT

public:
    ShaderDialog(ShaderInfo *sInfo, QWidget *parent = 0);
    ~ShaderDialog();

private:
		ShaderInfo * shaderInfo;
    Ui::ShaderDialogClass ui;

private slots:
	
	void onVarList_change(QString);
		
};

#endif // SHADERDIALOG_H
