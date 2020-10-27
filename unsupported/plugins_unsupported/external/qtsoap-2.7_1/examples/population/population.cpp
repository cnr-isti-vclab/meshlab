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

#include <QtGui/QApplication>
#include "population.h"

Population::Population(const QString &country, QObject *parent)
    : QObject(parent), http(this)
{
    // Connect the HTTP transport's responseReady() signal.
    connect(&http, SIGNAL(responseReady()), this, SLOT(getResponse()));

    // Construct a method request message.
    QtSoapMessage request;

    // Set the method and add one argument.
    request.setMethod("getPopulation", "http://www.abundanttech.com/WebServices/Population");
    request.addMethodArgument("strCountry", "", country);

    // Submit the request the the web service.
    http.setHost("www.abundanttech.com");
    http.setAction("http://www.abundanttech.com/WebServices/Population/getPopulation");
    http.submitRequest(request, "/WebServices/Population/population.asmx");

    qDebug("Looking up population of %s...", country.toLatin1().constData());
}

void Population::getResponse()
{
    // Get a reference to the response message.
    const QtSoapMessage &message = http.getResponse();

    // Check if the response is a SOAP Fault message
    if (message.isFault()) {
        qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
    }
    else {
        // Get the return value, and print the result.
        const QtSoapType &response = message.returnValue();
        qDebug("%s has a population of %s (last updated at %s)",
               response["Country"].value().toString().toLatin1().constData(),
               response["Pop"].value().toString().toLatin1().constData(),
               response["Date"].value().toString().toLatin1().constData());
    }
    qApp->quit();
}
