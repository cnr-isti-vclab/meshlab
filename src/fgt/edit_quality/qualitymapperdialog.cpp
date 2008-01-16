#include "qualitymapperdialog.h"

QualityMapperDialog::QualityMapperDialog(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
}

QualityMapperDialog::~QualityMapperDialog()
{

}

void QualityMapperDialog::setValues(const QualityMapperSettings& es)
{
	_settings=es;
	QString qnum="%1";
	ui.minSpinBox->setValue(_settings.meshMinQ);
	ui.midSpinBox->setValue(_settings.meshMidQ);
	ui.maxSpinBox->setValue(_settings.meshMaxQ);

	/*
	ui.percentileSpinbox->setValue(settings.percentile);
	ui.rangeSpinbox->setValue(settings.range);
	ui.meshMinQlineEdit->setText(qnum.arg(settings.meshMinQ));
	ui.meshMaxQlineEdit->setText(qnum.arg(settings.meshMaxQ));
	ui.histoMinQlineEdit->setText(qnum.arg(settings.histoMinQ));
	ui.histoMaxQlineEdit->setText(qnum.arg(settings.histoMaxQ));
	ui.manualMinQlineEdit->setText(qnum.arg(settings.manualMinQ));
	ui.manualMaxQlineEdit->setText(qnum.arg(settings.manualMaxQ));
	ui.manualCheckBox->setChecked(settings.useManual);  */
}

QualityMapperSettings QualityMapperDialog::getValues()
{
	_settings.manualMinQ=ui.minSpinBox->value();
	_settings.manualMidQ=ui.midSpinBox->value();
	_settings.manualMaxQ=ui.maxSpinBox->value();
	/*
	settings.percentile = ui.percentileSpinbox->value();
	settings.range = ui.rangeSpinbox->value();
	settings.useManual=ui.manualCheckBox->isChecked();
	settings.manualMaxQ=ui.manualMaxQlineEdit->text().toFloat();
	*/
	return _settings;
}








