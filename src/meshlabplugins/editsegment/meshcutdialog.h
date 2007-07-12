#ifndef MESHCUTDIALOG_H
#define MESHCUTDIALOG_H

#include <QWidget>
#include "ui_meshcutdialog.h"
#include <colorpicker.h>

class MeshCutDialog : public QWidget
{
	Q_OBJECT

public:
	MeshCutDialog(QWidget *parent = 0);
	~MeshCutDialog();

	inline bool SelectForeground() {return ui.foreRadioButton->isChecked();}
	inline QColor getForegroundColor() {return ui.foreColorPicker->getColor();}
	inline QColor getBackgroundColor() {return ui.backColorPicker->getColor();}

private:
	Ui::MeshCutDialogClass ui;

	public slots:
		void on_meshSegmentButton_clicked();
		void on_foreRadioButton_clicked();
		void on_backRadioButton_clicked();

		void on_gaussianButton_clicked();
		void on_meanButton_clicked();
		
	signals:
		void meshCutSignal();
		void selectForegroundSignal(bool);

		void colorizeGaussianSignal();
		void colorizeMeanSignal();
};

#endif // MESHCUTDIALOG_H
