/********************************************************************************
** Form generated from reading ui file 'ocme.ui'
**
** Created: Mon Feb 15 13:07:18 2010
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_OCME_H
#define UI_OCME_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_OcmeQFrame
{
public:

    void setupUi(QFrame *OcmeQFrame)
    {
        if (OcmeQFrame->objectName().isEmpty())
            OcmeQFrame->setObjectName(QString::fromUtf8("OcmeQFrame"));
        OcmeQFrame->resize(400, 300);
        OcmeQFrame->setFrameShape(QFrame::StyledPanel);
        OcmeQFrame->setFrameShadow(QFrame::Raised);

        retranslateUi(OcmeQFrame);

        QMetaObject::connectSlotsByName(OcmeQFrame);
    } // setupUi

    void retranslateUi(QFrame *OcmeQFrame)
    {
        OcmeQFrame->setWindowTitle(QApplication::translate("OcmeQFrame", "Frame", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(OcmeQFrame);
    } // retranslateUi

};

namespace Ui {
    class OcmeQFrame: public Ui_OcmeQFrame {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OCME_H
