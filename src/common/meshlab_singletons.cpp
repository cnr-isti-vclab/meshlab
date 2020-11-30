#include "meshlab_singletons.h"

#include "plugin_manager.h"

namespace meshlab {

MeshLabSingletons::MeshLabSingletons()
{
}

RichParameterList& MeshLabSingletons::globalRPLInstance()
{
	static RichParameterList globalRPS;
	return globalRPS;
}

PluginManager& MeshLabSingletons::pluginManagerInstance()
{
	static bool initialized = false;
	static PluginManager pm;
	if (!initialized){
		initialized = true;
		RichParameterList& globalRPS = globalRPLInstance();
		pm.loadPlugins(globalRPS);
	}
	return pm;
}

} //namespace meshlab
