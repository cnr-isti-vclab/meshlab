#include "Persistence.h"

#include "../Logging/Logging.h"

#include <QMap>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QVariant>

using namespace SyntopiaCore::Logging;

namespace SyntopiaCore {
	namespace Misc {	




		QMap<QString, QVariant>& Persistence::GetStore() { 
			static QMap<QString, QVariant> p;
			return p;
		}

		void Persistence::Store(QWidget* widget, QString storageName) {
			if (storageName.isEmpty()) storageName = widget->objectName();
			if (qobject_cast<QLineEdit*>(widget)) {
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget); 
				GetStore()[storageName] = QVariant(lineEdit->text());
			} else if (qobject_cast<QCheckBox*>(widget)) {
				QCheckBox* cb = qobject_cast<QCheckBox*>(widget); 
				GetStore()[storageName] = QVariant(cb->isChecked());
			} else if (qobject_cast<QSpinBox*>(widget)) {
				QSpinBox* sb = qobject_cast<QSpinBox*>(widget); 
				GetStore()[storageName] = QVariant(sb->value());
			} else if (qobject_cast<QRadioButton*>(widget)) {
				QRadioButton* rb = qobject_cast<QRadioButton*>(widget); 
				GetStore()[storageName] = QVariant(rb->isChecked());
			} else if (qobject_cast<QComboBox*>(widget)) {
				QComboBox* cb = qobject_cast<QComboBox*>(widget); 
				GetStore()[storageName] = QVariant(cb->currentText());
			} else {
				WARNING("Unsupported widget: " + widget->objectName());
			}
		}

		void Persistence::Restore(QWidget* widget, QString storageName) {
			if (storageName.isEmpty()) storageName = widget->objectName();
			if (qobject_cast<QLineEdit*>(widget)) {
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget); 
				if (GetStore().contains(storageName)) {
					lineEdit->setText(GetStore()[storageName].toString());
				}

			} else if (qobject_cast<QCheckBox*>(widget)) {
				QCheckBox* cb = qobject_cast<QCheckBox*>(widget); 
				if (GetStore().contains(storageName)) {
					cb->setChecked(GetStore()[storageName].toBool());
				}

			} else if (qobject_cast<QSpinBox*>(widget)) {
				QSpinBox* sb = qobject_cast<QSpinBox*>(widget); 
				if (GetStore().contains(storageName)) {
					sb->setValue(GetStore()[storageName].toInt());
				}

			} else if (qobject_cast<QRadioButton*>(widget)) {
				QRadioButton* rb = qobject_cast<QRadioButton*>(widget); 
				if (GetStore().contains(storageName)) {
					rb->setChecked(GetStore()[storageName].toInt());
				}

			} else if (qobject_cast<QComboBox*>(widget)) {
				QComboBox* rb = qobject_cast<QComboBox*>(widget); 
				if (GetStore().contains(storageName)) {
					int i = rb->findText(GetStore()[storageName].toString());
					if (i!=-1) {
						rb->setCurrentIndex(i);
					} else {
					}
				}

			} else {
				WARNING("Unsupported widget: " + widget->objectName());
			}
		}

		bool Persistence::Contains(QString storageName) {
			return GetStore().contains(storageName);
		}

		QVariant Persistence::Get(QString storageName) {
			return GetStore()[storageName];
		}

		void Persistence::Put(QString storageName, QVariant value) {
			GetStore()[storageName] = value;
		}


	}
}

