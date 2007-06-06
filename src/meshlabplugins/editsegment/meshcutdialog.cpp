#include "meshcutdialog.h"

MeshCutDialog::MeshCutDialog(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

MeshCutDialog::~MeshCutDialog()
{

}

void MeshCutDialog::on_meshcutButton_clicked() {
	emit mesh_cut();
}