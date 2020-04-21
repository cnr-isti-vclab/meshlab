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
#include <QtGui/QMessageBox>
#include <QtGui/QCursor>
#include <QtCore/QFile>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTextBrowser>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include "google.h"

Google::Google(QWidget *parent)
    : QWidget(parent), http(this)
{
    // Generate the GUI.
    quitButton = new QPushButton("&Quit", this);
    searchString = new QLineEdit(this);
    resultView = new QTextBrowser(this);
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *gLabel = new QLabel("Google Web APIs license key: ", this);
    googleKey = new QLineEdit(this);
    QHBoxLayout *topBar = new QHBoxLayout();
    topBar->addWidget(gLabel);
    topBar->addWidget(googleKey);
    layout->addLayout(topBar);

    QLabel *sLabel = new QLabel("Search string: ", this);
    QPushButton *searchButton = new QPushButton("Search!", this);
    QHBoxLayout *searchBar = new QHBoxLayout();
    searchBar->addWidget(sLabel);
    searchBar->addWidget(searchString);
    searchBar->addWidget(searchButton);
    layout->addLayout(searchBar);

    layout->addWidget(resultView);
    layout->addWidget(quitButton);

    // Connect signals to slots. Note the submitRequest() slots.
    connect(&http, SIGNAL(responseReady()), SLOT(getResponse()));
    connect(quitButton, SIGNAL(clicked()), SLOT(close()));
    connect(searchString, SIGNAL(returnPressed()), SLOT(submitRequest()));
    connect(searchButton, SIGNAL(clicked()), SLOT(submitRequest()));
    connect(googleKey, SIGNAL(returnPressed()), SLOT(submitRequest()));

    // Pressing enter should trigger a search, unless focus has been
    // explicitly moved.
    searchButton->setDefault(true);

    // Prepare to submit request.
    http.setAction("urn:GoogleSearchAction");
    http.setHost("api.google.com");
}

void Google::submitRequest()
{
    // Check that the license key is set.
    if (googleKey->text() == "") {
	QMessageBox::warning(this, tr("Missing license key"),
			     tr("Please enter your Google Web APIs license key."
				" If you do not have one, you can visit<br>"
				" http://api.google.com/ and create a Google"
				" account to obtain a license key."));
	return;
    }

    // Check that we have a search string.
    if (searchString->text() == "") {
	QMessageBox::warning(this, tr("Missing search string"),
			     tr("Please enter a search string."));
	return;

    }

    // Generate request. Details about how to generate a proper
    // request are available from http://api.google.com/. The results
    // are always fetched as latin1 in this example, but this can be
    // easily changed (see the "ie" and "oe" arguments below).
    QtSoapMessage request;
    request.setMethod(QtSoapQName("doGoogleSearch", "urn:GoogleSearch"));
    request.addMethodArgument("key", "", googleKey->text());
    request.addMethodArgument("q", "", searchString->text());
    request.addMethodArgument("start", "", 0);
    request.addMethodArgument("maxResults", "", 10);
    request.addMethodArgument("filter", "", false, 0);
    request.addMethodArgument("restrict", "", "");
    request.addMethodArgument("safeSearch", "", false, 0);
    request.addMethodArgument("lr", "", "");
    request.addMethodArgument("ie", "", "latin1");
    request.addMethodArgument("oe", "", "latin1");

    // Submit the method request to the web service.
    http.submitRequest(request, "/search/beta2");

    // Set the cursor to wait mode.
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

void Google::getResponse()
{
    // Set cursor back to normal shape.
    QApplication::restoreOverrideCursor();

    // Reset resultView.
    resultView->clear();

    // Get the response, check for error.
    const QtSoapMessage &resp = http.getResponse();
    if (resp.isFault()) {
	resultView->setHtml(tr("<b>Query failed</b>: ")
			    + resp.faultString().value().toString());
	return;
    }

    // Extract the return value from this method response, check for
    // errors.
    const QtSoapType &res = resp.returnValue();
    if (!res.isValid()) {
	resultView->append(tr("Invalid return value"));
	return;
    }

    // Generate resultView output. Make it resemble the actual web
    // output from http://www.google.com/.
    QString header(tr("Searched the web for <b>%1</b>, results %2 - %3 "
		      "of about %4. Search took %5 seconds.<br><hr>")
		   .arg(res["searchQuery"].toString())
		   .arg(res["startIndex"].toInt())
		   .arg(res["endIndex"].toInt())
		   .arg(res["estimatedTotalResultsCount"].toInt())
		   .arg(res["searchTime"].value().toDouble(), 0, 'f', 2));

    const QtSoapType &resultElements = res["resultElements"];
    QString allElements;

    for (int i = 0; i < resultElements.count(); ++i) {
	const QtSoapType &resultElement = res["resultElements"][i];

	QString cat = resultElement["directoryCategory"]["fullViewableName"].toString();
	QString summary = resultElement["summary"].toString();
	QString title = resultElement["title"].toString();
	QString snippet = resultElement["snippet"].toString();
	QString url = resultElement["URL"].toString();
	QString cachedSize = resultElement["cachedSize"].toString();

	QString thisElement = "<br>";

	if (!title.isEmpty()) {
	    thisElement += "<font color=\"#0000FF\"><b><u>"
			   + title + "</u></b></font><br>";
	} else {
	    thisElement += "<font color=\"#0000FF\"><b><u>"
			   + url + "</u></b></font><br>";
	}

	if (!snippet.isEmpty())
	    thisElement += snippet + "<br>";

	if (!summary.isEmpty()) {
	    thisElement += "<font color=\"#808080\">Description:</font> "
			   + summary + "<br>";
	}

	if (!cat.isEmpty()) {
	    thisElement += "<font color=\"#808080\">Category: <u>"
			   + cat + "</u></font><br>";
	}

	if (!title.isEmpty()) {
	    thisElement += "<font color=\"#008000\"><u>" + url
			   + "</u> - " + cachedSize + "</font><br>";
	}

	allElements += thisElement;
    }

    // Update the resultView.
    resultView->setHtml(header + allElements);
}

