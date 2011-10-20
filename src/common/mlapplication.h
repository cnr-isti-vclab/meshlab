#ifndef ML_APPLICATION_H
#define ML_APPLICATION_H

#include <QApplication>

class MeshLabApplication : public QApplication
{
public:
	MeshLabApplication(int &argc, char *argv[]);
	~MeshLabApplication();
	bool notify(QObject * rec, QEvent * ev);
};

#endif
