#include<QFileDialog.h>
#include "converterdirectory.h"

ConverterDirectory::ConverterDirectory(QWidget *parent)
	: QFrame(parent)
{
	ui.setupUi(this);
}

ConverterDirectory::~ConverterDirectory()
{

}


void ConverterDirectory::on_browse_button_clicked()
{
	QFileDialog* brows = new QFileDialog();
	brows->show();
	ui.file_loc->setText(QFileDialog::getOpenFileName());
}