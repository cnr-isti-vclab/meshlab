#include "edit_cut_factory.h"
#include "edit_cut.h"

EditCutFactory::EditCutFactory()
{
	editCut = new QAction(QIcon(":/images/icon_cut.png"), "Lasso Cut Tool", this);
	actionList.push_back(editCut);

	foreach(QAction *editAction, actionList)
		editAction->setCheckable(true);
}

QString EditCutFactory::pluginName() const
{
	return "EditCut";
}

EditTool* EditCutFactory::getEditTool(const QAction *action)
{
	if (action == editCut)
		return new EditCutPlugin();
	assert(0);
	return nullptr;
}

QString EditCutFactory::getEditToolDescription(const QAction *)
{
	return EditCutPlugin::info();
}

MESHLAB_PLUGIN_NAME_EXPORTER(EditCutFactory)
