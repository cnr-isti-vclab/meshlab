#ifndef MESHCUTDIALOG_H
#define MESHCUTDIALOG_H

#include <QWidget>
#include "ui_meshcutdialog.h"

class MeshCutDialog : public QWidget
{
	Q_OBJECT

public:
	MeshCutDialog(QWidget *parent = 0);
	~MeshCutDialog();

private:
	Ui::MeshCutDialogClass ui;

	public slots:
		void on_meshcutButton_clicked();
	signals:
	void mesh_cut();
};

#endif // MESHCUTDIALOG_H
