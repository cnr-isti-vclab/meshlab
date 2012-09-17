#ifndef ML_APPLICATION_H
#define ML_APPLICATION_H

#include <QApplication>
#include <QString>

class MeshLabApplication : public QApplication
{
public:
	enum HW_ARCHITECTURE {HW_32BIT = 32,HW_64BIT = 64};
	MeshLabApplication(int &argc, char *argv[]);
	~MeshLabApplication();
	bool notify(QObject * rec, QEvent * ev);
	static const QString appName(){return tr("MeshLab"); }
	static const QString architecturalSuffix(const HW_ARCHITECTURE hw) {return "_" + QString::number(int(hw)) + "bit";}
	static const QString appArchitecturalName(const HW_ARCHITECTURE hw) {return appName() + architecturalSuffix(hw);}
	static const QString appVer() {return tr("1.3.3"); }
	static const QString completeName(const HW_ARCHITECTURE hw){return appArchitecturalName(hw) + " v" + appVer(); } 
	static const QString organization(){return tr("VCG");}
	static const QString organizationHost() {return tr("vcg.isti.cnr.it");}
	static const QString webSite() {return tr("http://meshlab.sourceforge.net/");}
	static const QString downloadSite() {return tr("http://downloads.sourceforge.net/project/meshlab");}
	static const QString downloadUpdatesSite() {return downloadSite() + tr("/updates");}

	static const QString pluginsPathRegisterKeyName() {return tr("pluginsPath");}
	static const QString versionRegisterKeyName() {return tr("version");}
	static const QString worldSizeKeyName() {return tr("worldSize");}
};

#endif
