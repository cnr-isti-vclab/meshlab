#include "singletons.h"

#include "../plugin_manager.h"
#include "globals.h"

namespace meshlab {

MeshLabSingletons::MeshLabSingletons()
{
}

PluginManager& MeshLabSingletons::pluginManagerInstance()
{
	static PluginManager pm(defaultGlobalParameterList());
	return pm;
}

} //namespace meshlab
