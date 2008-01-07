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

		void on_refineCheckBox_clicked();

		void on_gaussianButton_clicked();
		void on_meanButton_clicked();
		void on_resetButton_clicked();

		void on_normalWeightSlider_valueChanged();
		void on_curvatureWeightSlider_valueChanged();		
		
	signals:
		void meshCutSignal();
		void selectForegroundSignal(bool);
		void doRefineSignal(bool);
		void normalWeightSignal(int);
		void curvatureWeightSignal(int);

		void colorizeGaussianSignal();
		void colorizeMeanSignal();
		void resetSignal();
};

#endif // MESHCUTDIALOG_H
