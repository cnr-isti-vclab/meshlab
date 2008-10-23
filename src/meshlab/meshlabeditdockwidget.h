/*
 * MeshlabDockWidget.h
 *
 *  Created on: Aug 29, 2008
 *      Author: racoon
 */

#ifndef MESHLABEDITDOCKWIDGET_H_
#define MESHLABEDITDOCKWIDGET_H_

#include <QDockWidget>
#include "glarea.h"

class MeshlabEditDockWidget: public QDockWidget{
	Q_OBJECT
public:
	MeshlabEditDockWidget(GLArea *gla){
		if (gla !=0){
			setParent(gla->window());
			connect(this, SIGNAL(MEDW_Closing()),gla,SLOT(endEdit()) );
		}
	}
	
	void closeEvent ( QCloseEvent * event )
	{
		emit MEDW_Closing();
	}
	
	Q_SIGNALS:
		void MEDW_Closing();

};


#endif /* MESHLABEDITDOCKWIDGET_H_ */
