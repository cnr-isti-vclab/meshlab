#ifndef __MESHLAB_PLUGIN_SUPPORT_H
#define __MESHLAB_PLUGIN_SUPPORT_H


enum TypeIO{IMPORT,EXPORT};
	
void LoadKnownFilters(std::vector<MeshIOInterface*> meshIOPlugins, QStringList &filters, QHash<QString, int> &allKnownFormats, int type);
 
#endif