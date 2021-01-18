#include "coformimportdialog.h"
#include "ui_coformimportdialog.h"
#include <QHeaderView>

CoformImportDialog::CoformImportDialog(QStandardItemModel* model,QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::CoformImportDialogClass();
	ui->setupUi(this);
	setWindowTitle(tr("3D-COFORM Import Dialog"));
	ui->uuidViewer->setModel(model);
	ui->uuidViewer->horizontalHeader()->setStretchLastSection(true);
	ui->uuidViewer->setShowGrid(true);
}

CoformImportDialog::~CoformImportDialog()
{
	delete ui;
}

void CoformImportDialog::addExtensionFilters( const QStringList& ext )
{
	ui->filterBox->addItems(ext);
}