#ifndef ML_APPLICATION_H
#define ML_APPLICATION_H

#include <QApplication>
#include <QDate>
#include <QString>
#include <wrap/gl/gl_mesh_attributes_info.h>
#include "ml_mesh_type.h"

class MeshLabApplication : public QApplication
{
public:
    enum HW_ARCHITECTURE {HW_32BIT = 32,HW_64BIT = 64};
    MeshLabApplication(int &argc, char *argv[]):QApplication(argc,argv){}
    ~MeshLabApplication(){}
    bool notify(QObject * rec, QEvent * ev);
    static const QString appName(){return tr("MeshLab"); }
    static const QString architecturalSuffix(const HW_ARCHITECTURE hw) {return "_" + QString::number(int(hw)) + "bit";}
    static const QString appArchitecturalName(const HW_ARCHITECTURE hw) {return appName() + architecturalSuffix(hw) + "_" + MeshLabScalarTest<MESHLAB_SCALAR>::floatingPointPrecision();}
    static const QString appVer() 
	{
		const QDate dt = QDate::currentDate();
		return QString::number(dt.year()) + "." + (dt.month() < 10 ? "0" + QString::number(dt.month()) : QString::number(dt.month()));
	}

	static const QString shortName() { return appName() + " " + appVer(); }
    static const QString completeName(const HW_ARCHITECTURE hw){return appArchitecturalName(hw) + " v" + appVer(); }
    static const QString organization(){return tr("VCG");}
    static const QString organizationHost() {return tr("http://vcg.isti.cnr.it");}
    static const QString webSite() {return tr("http://www.meshlab.net/");}
    static const QString downloadSite() {return tr("http://www.meshlab.net/#download");}

    static const QString pluginsPathRegisterKeyName() {return tr("pluginsPath");}
    static const QString versionRegisterKeyName() {return tr("version");}
    static const QString wordSizeKeyName() {return tr("wordSize");}
};

#endif
