#include <QApplication>
#include "QEdgeLength.h"
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QEdgeLength *el = new QEdgeLength(5);
	el->show();
	return app.exec();
} 
