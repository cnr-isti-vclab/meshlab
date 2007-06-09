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


