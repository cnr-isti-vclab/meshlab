#include "mlapplication.h"
#include "mlexception.h"
#include <vcg/complex/complex.h>

#include <QFile>
#include <QTextStream>

bool MeshLabApplication::notify( QObject * rec, QEvent * ev )
{
  try
  {
    return QApplication::notify(rec,ev);
  }
  catch (MLException& e)
  {
    qCritical("%s",e.what());
  }
  catch (vcg::MissingComponentException &e )
  {
    qCritical("%s",e.what());
    abort();
  }
  catch (...)
  {
    qCritical("Something really bad happened!!!!!!!!!!!!!");
    abort();
  }
	return false;
}

const QString MeshLabApplication::appVer()
{
	static QString version = "";
	if (version == ""){
		QFile f("VERSION");
		if (!f.open(QFile::ReadOnly | QFile::Text)) version = "2020.02";
		else {
			QTextStream in(&f);
			version = in.readAll();
			version.chop(1);
		}
	}
	return version;
}

