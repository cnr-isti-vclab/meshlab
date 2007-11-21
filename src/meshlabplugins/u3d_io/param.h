#ifndef PARAM_H
#define PARAM_H

#include <QtGui/QDialog>
#include "ui_param.h"

class Param : public QDialog
{
	Q_OBJECT

public:
	Param(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Param();
	
private:
	Ui::ParamClass ui;

private slots:
	void on_ok_button_clicked();
	void on_tree_selector_itemClicked(QTreeWidgetItem* item,int col);
};

#endif // PARAM_H
