#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "interfaces.h"

class PluginManager
{
public:
  PluginManager();
  enum TypeIO{IMPORT,EXPORT};
  void loadPlugins(RichParameterSet& defaultGlobal);

	inline QVector<MeshIOInterface*>& meshIOPlugins()  {return meshIOPlug;}
	inline QVector<MeshFilterInterface*>& meshFilterPlugins()  {return meshFilterPlug;}
	inline QVector<MeshRenderInterface*>& meshRenderPlugins()  {return meshRenderPlug;}
	inline QVector<MeshDecorateInterface*>& meshDecoratePlugins()  {return meshDecoratePlug;}
	inline QVector<MeshEditInterfaceFactory*>& meshEditFactoryPlugins()  {return meshEditInterfacePlug;}
	void LoadFormats(QStringList &filters, QHash<QString, MeshIOInterface*> &allKnownFormats, int type);
	static QString getPluginDirPath();
  static QString getBaseDirPath();

	QDir pluginsDir;
  QMap<QString, QAction*> actionFilterMap;
  QMap<QString, MeshFilterInterface*> stringFilterMap;
	QVector<MeshIOInterface*> meshIOPlug;
	QVector<MeshFilterInterface*> meshFilterPlug;
	QVector<MeshRenderInterface*> meshRenderPlug;
	QVector<MeshDecorateInterface*> meshDecoratePlug;
	QVector<MeshEditInterfaceFactory*> meshEditInterfacePlug;
	QVector<QAction *> editActionList;
	QVector<QAction *> decoratorActionList;

  QStringList pluginsLoaded;
};

#endif // PLUGINMANAGER_H
