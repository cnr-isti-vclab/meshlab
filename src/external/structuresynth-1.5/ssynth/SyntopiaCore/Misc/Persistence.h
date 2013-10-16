#pragma once

#include <QString>
#include <QWidget>
#include <QList>


namespace SyntopiaCore {
	namespace Misc {	


		/// Util classes for making GUI settings persistent.
		/// (i.e. remember the state of drop-downs, line-edits, and so on).
		///
		/// Per default, the widget is stored under its objectName, but it possible to specify
		/// another name (e.g. to share widget data between dialogs).
		///
		/// Notice that widgets must be hardcoded into the cpp-file to be supported 
		class Persistence {
		public:
			static void Store(QWidget* widget, QString storageName = QString());
			static void Restore(QWidget* widget, QString storageName = QString());
			static bool Contains(QString storageName);
			static QVariant Get(QString storageName);
			static void Put(QString storageName, QVariant value);
		private:
			static QMap<QString, QVariant>& GetStore();
		};


	}
}


