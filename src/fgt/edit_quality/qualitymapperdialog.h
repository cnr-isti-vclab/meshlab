#ifndef QUALITYMAPPERDIALOG_H
#define QUALITYMAPPERDIALOG_H

#include <QDialog>
#include "ui_qualitymapperdialog.h"

class QualityMapperDialog : public QDialog
{
	Q_OBJECT

public:
	QualityMapperDialog(QWidget *parent = 0);
	~QualityMapperDialog();

private:
	Ui::QualityMapperDialogClass ui;

// private slots:
// 	void on_radioButton_toggled(bool);
// 	void on_applyButton_clicked();
};

#endif // QUALITYMAPPERDIALOG_H
