#include "singletons.h"

#include "../plugin_manager.h"
#include "globals.h"

namespace meshlab {

MeshLabSingletons::MeshLabSingletons()
{
}

PluginManager& MeshLabSingletons::pluginManagerInstance(bool verbose)
{
	static PluginManager pm(defaultGlobalParameterList(), verbose);
	return pm;
}

} //namespace meshlab
