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
		bool not = QApplication::notify(rec,ev);
		return not;
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
