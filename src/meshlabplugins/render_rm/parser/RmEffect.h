#ifndef __RMEFFECT_H__
#define __RMEFFECT_H__

#include <QString>
#include <QList>

#include "RmPass.h"

class RmEffect
{
	QString name;
	QList<RmPass> passlist;
	

	public:

		RmEffect( QString _name ) { name = _name; }

		int size() { return passlist.size(); }
		RmPass & at( int idx ) { return passlist[idx]; }
		RmPass & operator[] (int idx) {return passlist[idx]; }

		QList<RmPass> & getPassList() { return passlist; }
		
		void addPass( RmPass pass ) { passlist.append(pass); }
		QString & getName() { return name; }

		void sortPasses() {  qSort( passlist.begin(), passlist.end() ); }
};

#endif

