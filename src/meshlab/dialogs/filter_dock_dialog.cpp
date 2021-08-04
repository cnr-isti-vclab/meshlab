#include "filter_dock_dialog.h"
#include "ui_filter_dock_dialog.h"

FilterDockDialog::FilterDockDialog(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::FilterDockDialog)
{
	ui->setupUi(this);
}

FilterDockDialog::~FilterDockDialog()
{
	delete ui;
}
