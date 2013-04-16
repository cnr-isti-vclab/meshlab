#include "mlapplication.h"
#include "mlexception.h"

MeshLabApplication::MeshLabApplication( int &argc, char *argv[] )
:QApplication(argc,argv)
{

}

MeshLabApplication::~MeshLabApplication()
{

}

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
	catch (...)
	{
		qCritical("Something really bad happened!!!!!!!!!!!!!");
		abort();
	}
  return false;
}
