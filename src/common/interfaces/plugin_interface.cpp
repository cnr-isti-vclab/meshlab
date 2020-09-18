#include "plugin_interface.h"

PluginInterface::PluginInterface() :
    logstream(nullptr)
{
}

/** \brief This function is called by the framework, for each plugin that has global parameters (e.g. \ref MeshDecorateInterface) at the start of the application.
 *The rationale is to allow to each plugin to have a list of global persistent parameters that can be changed from the meshlab itself and whose value is persistent between different meshlab invocations.
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
 * The RichParameterSet comes to the StartDecorate already initialized with the values stored on the permanent storage.
 * At the start up the initGlobalParameterSet function is called with an empty RichParameterSet (to collect the default values)
 * If a filter wants to save some permanent stuff should set the permanent default values.
 */
void PluginInterface::initGlobalParameterSet(QAction* /*format*/, RichParameterList& /*globalparam*/)
{
}

void PluginInterface::setLog(GLLogStream* log)
{
	this->logstream = log;
}

void PluginInterface::log(const char* s)
{
	if(logstream != nullptr) {
		logstream->Log(GLLogStream::FILTER, s);
	}
}

void PluginInterface::log(const std::string& s)
{
	if(logstream != nullptr) {
		logstream->Log(GLLogStream::FILTER, s);
	}
}

void PluginInterface::log(GLLogStream::Levels level, const char* s)
{
	if(logstream != nullptr) {
		logstream->Log(level, s);
	}
}

void PluginInterface::log(GLLogStream::Levels level, const std::string& s)
{
	if(logstream != nullptr) {
		logstream->Log(level, s);
	}
}

void PluginInterface::realTimeLog(QString Id, const QString& meshName, const char* f)
{
	if(logstream != nullptr) {
		logstream->RealTimeLog(Id, meshName, f);
	}
}
