#include "singletons.h"

#include "../plugin_manager.h"
#include "globals.h"

namespace meshlab {

MeshLabSingletons::MeshLabSingletons()
{
}

PluginManager& MeshLabSingletons::pluginManagerInstance()
{
	static bool initialized = false;
	static PluginManager pm;
	if (!initialized){
		initialized = true;
		RichParameterList& globalRPS = defaultGlobalParameterList();
		pm.loadPlugins(globalRPS);
	}
	return pm;
}

} //namespace meshlab
