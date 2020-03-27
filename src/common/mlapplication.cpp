#include "mlapplication.h"
#include "mlexception.h"
#include <vcg/complex/complex.h>

#define xstr(a) str(a)
#define str(a) #a

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
	return QString(xstr(MESHLAB_VERSION));
}

