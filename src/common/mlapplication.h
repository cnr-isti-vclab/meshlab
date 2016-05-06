#ifndef ML_APPLICATION_H
#define ML_APPLICATION_H

#include <QApplication>
#include <QString>
#include <wrap/gl/gl_mesh_attributes_info.h>

template<typename T>
struct MeshLabScalarTest
{

};

template<>
struct MeshLabScalarTest<float>
{
	static const QString floatingPointPrecision() {return QString("fp");}
    static const char* floatingPointPrecisionIOToken() {return "%f";}
	static bool doublePrecision() {return false;}
};

template<>
struct MeshLabScalarTest<double>
{
	static const QString floatingPointPrecision() {return QString("dp");}
    static const char* floatingPointPrecisionIOToken() {return "%lf";}
	static bool doublePrecision() {return true;}
};


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
    static const QString appVer() {return tr("1.4.0"); }
    static const QString completeName(const HW_ARCHITECTURE hw){return appArchitecturalName(hw) + " v" + appVer(); }
    static const QString organization(){return tr("VCG");}
    static const QString organizationHost() {return tr("http://vcg.isti.cnr.it");}
    static const QString webSite() {return tr("http://meshlab.sourceforge.net/");}
    static const QString downloadSite() {return tr("http://downloads.sourceforge.net/project/meshlab");}
    static const QString downloadUpdatesSite() {return downloadSite() + tr("/updates");}

    static const QString pluginsPathRegisterKeyName() {return tr("pluginsPath");}
    static const QString versionRegisterKeyName() {return tr("version");}
    static const QString wordSizeKeyName() {return tr("wordSize");}
};

#endif
