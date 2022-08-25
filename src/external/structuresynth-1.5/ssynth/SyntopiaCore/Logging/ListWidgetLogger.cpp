#include "ListWidgetLogger.h"



namespace SyntopiaCore {
	namespace Logging {

		ListWidgetLogger::ListWidgetLogger(QWidget* parent) : parent(parent) { 	
			listWidget = new QListWidget(parent);
		}

		ListWidgetLogger::~ListWidgetLogger() { 
		}

		void ListWidgetLogger::log(QString message, LogLevel priority) {
			QListWidgetItem* i = new QListWidgetItem(message, listWidget);

			// Levels: NoneLevel, DebugLevel, TimingLevel, InfoLevel, WarningLevel, CriticalLevel, AllLevel

			if ( priority == InfoLevel ) {
				i->setBackgroundColor(QColor(255,255,255));
			} else if ( priority == WarningLevel ) {
				parent->show();
				i->setBackgroundColor(QColor(255,243,73));
			} else if ( priority == CriticalLevel ) {
				parent->show();
				i->setBackgroundColor(QColor(255,2,0));
			} else if ( priority == TimingLevel ) {
				parent->show();
				i->setBackgroundColor(QColor(25,255,0));
			} else {
				i->setBackgroundColor(QColor(220,220,220));
			}
			listWidget->scrollToItem(i); 

		}

	}
}
