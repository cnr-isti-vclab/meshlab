#include "mlapplication.h"
#include "mlexception.h"
#include <vcg/complex/complex.h>
bool MeshLabApplication::notify( QObject * rec, QEvent * ev )
{
  try
  {
    return QApplication::notify(rec,ev);
  }
  catch (MeshLabException& e)
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
