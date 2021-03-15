#include "decorate_plugin.h"

/** \brief
 * This function is called by the framework, for each plugin that has global parameters (e.g. \ref MeshDecorateInterface) at the start of the application.
 * The rationale is to allow to each plugin to have a list of global persistent parameters that can be changed from the meshlab itself and whose value is persistent between different meshlab invocations.
 * A typical example is the background color.
 *
 * For the global parameters the following rules apply:
 *
 * \li there is a <b>hardwired</b> default value: a safe consistent value that is directly coded into the plugin and to which the user can always revert if needed.
 * \li there is a <b>saved</b> value: a value that is stored into a persistent location into the user space (registry/home/library) and it is presented as default value of the parameter at each MeshLab invocation.
 * \li there is a <b>current</b> value: a value that is currently used, different for each document instance and that is not stored permanently.
 *
 * The plugin use the current value to draw its decoration.
 * at startup the current value is always silently initialized to the saved value.
 * User can revert current value to the saved values and to the hardwired values.
 * In the dialog for each parameter some buttons should be present:

 * \li apply: use the currently edited parameter value without saving it anywhere. After the closure of the document these values will be lost.
 * \li load:  load from the saved values
 * \li save:  save to a permanent location the current value (to the registry),
 * \li reset:  revert to the hardwired values

 * If your plugins/action has no GlobalParameter, do nothing.
 * The RichParameterList comes to the StartDecorate already initialized with the values stored on the permanent storage.
 * At the start up the initGlobalParameterList function is called with an empty RichParameterList (to collect the default values)
 * If a filter wants to save some permanent stuff should set the permanent default values.
 */
void DecoratePlugin::initGlobalParameterList(const QAction* /*format*/, RichParameterList& /*globalparam*/)
{
}

QAction* DecoratePlugin::action(QString name) const
{
	QString n = name;
	for(QAction *tt : actions())
		if (name == this->decorationName(ID(tt))) return tt;
	n.replace("&","");
	for(QAction *tt : actions())
		if (n == this->decorationName(ID(tt))) return tt;

	qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(name));
	return 0;
}

MeshLabPluginFile::ActionIDType DecoratePlugin::ID(const QAction* a) const
{
	QString aa=a->text();
	foreach(ActionIDType tt, types())
		if (a->text() == this->decorationName(tt)) return tt;
	aa.replace("&","");
	foreach(ActionIDType tt, types())
		if (aa == this->decorationName(tt)) return tt;

	qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(a->text()));
	assert(0);
	return -1;
}

MeshLabPluginFile::ActionIDType DecoratePlugin::ID(QString name) const
{
	QString n = name;
	foreach(ActionIDType tt, types())
		if (name == this->decorationName(tt)) return tt;
	n.replace("&","");
	foreach(ActionIDType tt, types())
		if (n == this->decorationName(tt)) return tt;

	qDebug("unable to find the id corresponding to action  '%s'", qUtf8Printable(name));
	assert(0);
	return -1;
}
