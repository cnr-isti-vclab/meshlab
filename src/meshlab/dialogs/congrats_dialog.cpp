#include "congrats_dialog.h"
#include "ui_congrats_dialog.h"

#include <common/mlapplication.h>

#include <QSettings>
#include <QDesktopServices>

CongratsDialog::CongratsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CongratsDialog)
{
	ui->setupUi(this);
}

CongratsDialog::~CongratsDialog()
{
	delete ui;
}

void CongratsDialog::on_githubButton_clicked()
{
	QDesktopServices::openUrl(QUrl("https://github.com/cnr-isti-vclab/meshlab/discussions/new"));
}

void CongratsDialog::on_emailButton_clicked()
{
	QDesktopServices::openUrl(QUrl("mailto:paolo.cignoni@isti.cnr.it;alessandro.muntoni@isti.cnr.it?subject=[MeshLab] Reporting Info on MeshLab Usage - V"+MeshLabApplication::appVer()));
}

void CongratsDialog::on_closeButton_clicked()
{
	updateDontShowSetting();
	close();
}

void CongratsDialog::updateDontShowSetting()
{
	const QString dontRemindMeToSendEmailVar("dontRemindMeToSendEmail");
	QSettings settings;
	//This preference values store when you did the last request for a mail
	int congratsMeshCounter = settings.value("congratsMeshCounter", 50).toInt();
	settings.setValue("congratsMeshCounter", congratsMeshCounter * 2);
	if (ui->dontShowCheckBox->isChecked())
		settings.setValue(dontRemindMeToSendEmailVar, true);
	else
		settings.setValue(dontRemindMeToSendEmailVar, false);
}
