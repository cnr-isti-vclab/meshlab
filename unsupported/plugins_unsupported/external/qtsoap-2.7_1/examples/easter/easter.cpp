/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include "easter.h"

Easter::Easter(short year, QObject *parent)
    : QObject(parent), http(this)
{
    connect(&http, SIGNAL(responseReady(const QtSoapMessage &)),
            this, SLOT(getResponse(const QtSoapMessage &)));

    QtSoapMessage request;
    request.setMethod("GetEaster",
		      "http://www.27seconds.com/Holidays/US/Dates/");
    request.addMethodArgument("year", "", year);

    http.setHost("www.27seconds.com");
    http.setAction("http://www.27seconds.com/Holidays/US/Dates/GetEaster");
    http.submitRequest(request, "/Holidays/US/Dates/USHolidayDates.asmx");

    qDebug("Looking up the date of easter in %i...", year);
    this->year = year;
}

void Easter::getResponse(const QtSoapMessage &message)
{
    if (message.isFault()) {
	qDebug("Error: %s", qPrintable(message.faultString().toString()));
    }
    else {
        QString res = message.returnValue().toString();
        QDateTime dt = QDateTime::fromString(res, Qt::ISODate);
        if (dt.isValid())
            res = QLocale::c().toString(dt.date());

        qDebug("Easter is: %s", res.toLatin1().constData());
    }
    QCoreApplication::quit();
}
