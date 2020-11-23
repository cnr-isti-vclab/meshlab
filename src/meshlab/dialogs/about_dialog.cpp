#include "about_dialog.h"
#include "ui_about_dialog.h"

#include "common/mlapplication.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	ui->labelMLName->setText(
				MeshLabApplication::completeName(MeshLabApplication::HW_ARCHITECTURE(QSysInfo::WordSize)) + "\n" +
				"built on "+__DATE__+" with " + MeshLabApplication::compilerVersion() +
				" and Qt " + MeshLabApplication::qtVersion() + ".");
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
