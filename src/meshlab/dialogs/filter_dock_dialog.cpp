#include "filter_dock_dialog.h"
#include "ui_filter_dock_dialog.h"

FilterDockDialog::FilterDockDialog(
	const RichParameterList& rpl,
	FilterPlugin*            plugin,
	const QAction*           filter,
	QWidget*                 parent,
	GLArea*                  glArea) :
	QDockWidget(parent),
	ui(new Ui::FilterDockDialog),
	plugin(plugin),
	filter(filter),
	parameters(rpl),
	mask(plugin->postCondition(filter)),
	currentGLArea(glArea)
{
	ui->setupUi(this);

	this->setWindowTitle(plugin->filterName(filter));
	ui->filterInfoLabel->setText(plugin->filterInfo(filter));

	ui->parameterFrame->initParams(rpl, rpl, (QWidget*) glArea);
	ui->parameterFrame->setMinimumSize(ui->parameterFrame->sizeHint());

	if (!isPreviewable())
		ui->previewCheckBox->setVisible(false);
	setMinimumWidth(sizeHint().width());
}

FilterDockDialog::~FilterDockDialog()
{
	delete ui;
}

void FilterDockDialog::on_applyPushButton_clicked()
{
	ui->parameterFrame->writeValuesOnParameterList(parameters);
	emit applyButtonClicked(filter, parameters);
}

void FilterDockDialog::on_helpPushButton_clicked()
{
	ui->parameterFrame->toggleHelp();
	ui->parameterFrame->setMinimumWidth(ui->parameterFrame->sizeHint().width());
	setMinimumWidth(sizeHint().width());
}

void FilterDockDialog::on_closePushButton_clicked()
{
	close();
}

void FilterDockDialog::on_defaultPushButton_clicked()
{
	ui->parameterFrame->resetValues();
}

bool FilterDockDialog::isPreviewable() const
{
	if ((filter == nullptr) || (plugin == nullptr) ||
		(plugin->filterArity(filter) != FilterPlugin::SINGLE_MESH))
		return false;

	if ((mask == MeshModel::MM_UNKNOWN) || (mask == MeshModel::MM_NONE))
		return false;

	if ((mask & MeshModel::MM_VERTNUMBER) || (mask & MeshModel::MM_FACENUMBER))
		return false;

	return true;
}
