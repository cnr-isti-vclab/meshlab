#include <QtCore>
#include <QString>
#include <QObject>
#include <QApplication>

#include <QtGui>
//#include <QToolBar>
#include <QProgressBar>


#include "meshmodel.h"
#include "interfaces.h"


enum TypeIO{IMPORT,EXPORT};
void LoadKnownFilters(std::vector<MeshIOInterface*> meshIOPlugins, QStringList &filters, QHash<QString, int> &allKnownFormats, int type)
{
  QString allKnownFormatsFilter = QObject::tr("All known formats ("); 
	std::vector<MeshIOInterface*>::iterator itIOPlugin = meshIOPlugins.begin();
	for (int i = 0; itIOPlugin != meshIOPlugins.end(); ++itIOPlugin, ++i)  // cycle among loaded IO plugins
	{
		MeshIOInterface* pMeshIOPlugin = *itIOPlugin;

		QList<MeshIOInterface::Format> currentFormats;// = pMeshIOPlugin->formats();

		/* new */
		if(type == IMPORT)
			currentFormats = pMeshIOPlugin->importFormats();

		if(type == EXPORT)
			currentFormats = pMeshIOPlugin->exportFormats();
		/* end new part */

		QList<MeshIOInterface::Format>::iterator itFormat = currentFormats.begin();
		while(itFormat != currentFormats.end())
		{
			MeshIOInterface::Format currentFormat = *itFormat;
			
			QString currentFilterEntry = currentFormat.description + " (";
			
			QStringListIterator itExtension(currentFormat.extensions);
			while (itExtension.hasNext())
			{
				QString currentExtension = itExtension.next().toLower();
				if (!allKnownFormats.contains(currentExtension))
				{
					allKnownFormats.insert(currentExtension, i+1);
					allKnownFormatsFilter.append(QObject::tr(" *."));
					allKnownFormatsFilter.append(currentExtension);
				}
				currentFilterEntry.append(QObject::tr(" *."));
				currentFilterEntry.append(currentExtension);
			}
			currentFilterEntry.append(')');
			filters.append(currentFilterEntry);

			++itFormat;
		}
	}
	allKnownFormatsFilter.append(')');
	filters.push_front(allKnownFormatsFilter);
}
