#include "coformimportdialog.h"
#include "ui_coformimportdialog.h"
#include <QHeaderView>

CoformImportDialog::CoformImportDialog(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::CoformImportDialogClass();
	ui->setupUi(this);
	setWindowTitle(tr("3D-COFORM Import Dialog"));
	setupModel();
	ui->uuidViewer->setModel(model);
	ui->uuidViewer->horizontalHeader()->setStretchLastSection(true);
	ui->uuidViewer->setShowGrid(true);
}

CoformImportDialog::~CoformImportDialog()
{
	delete ui;
	delete model;
}

void CoformImportDialog::addExtensionFilters( const QStringList& ext )
{
	ui->filterBox->addItems(ext);
}

void CoformImportDialog::setupModel()
{
	model = new QStandardItemModel(0,2,this);
	model->setHeaderData(0, Qt::Horizontal, tr("UUID"));
	model->setHeaderData(1, Qt::Horizontal, tr("File Name"));
}