#include "meshcutdialog.h"

MeshCutDialog::MeshCutDialog(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.backColorPicker->setColor(Qt::blue);
	ui.foreColorPicker->setColor(Qt::red);
	ui.foreRadioButton->setChecked(true);
}

MeshCutDialog::~MeshCutDialog()
{

}

void MeshCutDialog::on_meshSegmentButton_clicked() {
	emit meshCutSignal();
}

void MeshCutDialog::on_foreRadioButton_clicked() {
	emit selectForegroundSignal(true);
}


void MeshCutDialog::on_backRadioButton_clicked() {
	emit selectForegroundSignal(false);
}

void MeshCutDialog::on_refineCheckBox_clicked() {
	emit doRefineSignal(ui.refineCheckBox->isChecked());
}


//debug function
void MeshCutDialog::on_gaussianButton_clicked() {
	emit colorizeGaussianSignal();
}

void MeshCutDialog::on_meanButton_clicked() {
	emit colorizeMeanSignal();
}

void MeshCutDialog::on_resetButton_clicked() {
	emit resetSignal();
}