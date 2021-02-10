#include "mlapplication.h"
#include "mlexception.h"
#include <vcg/complex/complex.h>
#include <QStandardPaths>
#include <QDir>
#include "globals.h"

#ifndef MESHLAB_VERSION
#define MESHLAB_VERSION 2020.09
#endif

#if defined(__clang__)
#define ML_COMPILER "Clang"
#define ML_COMPILER_VER MeshLabApplication::versionString(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__) || defined(__GNUG__) 
#define ML_COMPILER "GCC"
#define ML_COMPILER_VER MeshLabApplication::versionString(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#define ML_COMPILER "MSVC"
#define ML_COMPILER_VER std::to_string(_MSC_VER)
#else
#define ML_COMPILER "Unknown Compiler"
#define ML_COMPILER_VER std::string()
#endif

#ifdef NDEBUG
bool MeshLabApplication::notify( QObject * rec, QEvent * ev )
{
	try {
		return QApplication::notify(rec,ev);
	}
	catch (MLException& e) {
		qCritical("%s",e.what());
	}
	catch (vcg::MissingComponentException &e ) {
		qCritical("%s",e.what());
		abort();
	}
	catch (...) {
		qCritical("Something really bad happened!!!!!!!!!!!!!");
		abort();
	}
	return false;
}
#endif

const QString MeshLabApplication::appVer()
{
	return QString::fromStdString(meshlab::meshlabVersion());
}

const QString MeshLabApplication::compilerVersion()
{
	return QString(ML_COMPILER) + QString(" ") + QString::fromStdString(ML_COMPILER_VER);
}

const QString MeshLabApplication::qtVersion()
{
	return QString(QT_VERSION_STR);
}

const QString MeshLabApplication::extraPluginsLocation()
{
	QDir appDir(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).first());
	appDir.mkpath(appDir.absolutePath());
	
	appDir.mkdir("MeshLabExtraPlugins");
	appDir.cd("MeshLabExtraPlugins");
	
	QString major = appVer().left(4);
	appDir.mkdir(major);
	appDir.cd(major);
	
	return appDir.absolutePath();
}

std::string MeshLabApplication::versionString(int a, int b, int c)
{
	std::ostringstream ss;
	ss << a << '.' << b << '.' << c;
	return ss.str();
}

