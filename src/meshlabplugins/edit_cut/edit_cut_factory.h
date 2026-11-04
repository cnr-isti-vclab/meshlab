#ifndef EDITCUTFACTORYPLUGIN_H
#define EDITCUTFACTORYPLUGIN_H

#include <common/plugins/interfaces/edit_plugin.h>

class EditCutFactory : public QObject, public EditPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(EDIT_PLUGIN_IID)
	Q_INTERFACES(EditPlugin)

public:
	EditCutFactory();
	virtual ~EditCutFactory() { delete editCut; }

	virtual void initGlobalParameterList(RichParameterList& /*defaultGlobalParamSet*/) {}

	virtual QString pluginName() const;
	virtual EditTool* getEditTool(const QAction*);
	virtual QString getEditToolDescription(const QAction*);

private:
	QAction *editCut;
};

#endif
