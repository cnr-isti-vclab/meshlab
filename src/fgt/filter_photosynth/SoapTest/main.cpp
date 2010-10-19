#include <QtCore/QCoreApplication>
#include <QtSoapHttpTransport>
#include <stdio.h>
#include <QScriptEngine>
#include <QScriptValue>
#include "handler.h"

//#define PRINT_JSON

QtSoapHttpTransport transport;

Handler::Handler(QObject *parent)
    : QObject(parent)
{

}

Handler::~Handler()
{

}

void Handler::readWSresponse()
{
    const QtSoapMessage &response = transport.getResponse();
    if (response.isFault())
    {
        printf("ERROR: %s\n",response.faultString().toString().toLatin1().constData());
        return;
    }

    printf("-----------RESPONSE------------\n");
    qWarning("%s\n", response.toXmlString().toLatin1().constData());
    printf("-----------------------\n");

    const QtSoapType &returnValue = response.returnValue();
    //controllo se la risposta del webservice è un errore
    if(returnValue["fault"].isValid())
        printf("Warning: %s", returnValue["fault"]["faultstring"].toString().toLatin1().constData());
    else
        //non ci sono stati errori nella comunicazione con il webservice
        //controllo qual e' il risultato dell'operazione
        if (returnValue["Result"].isValid())
        {
            printf("Result field: %s\n",returnValue["Result"].toString().toLatin1().constData());
            if(returnValue["Result"].toString() == "OK")
            {
                QString jsonURL = returnValue["JsonUrl"].toString();
                downloadJsonData(jsonURL);
            }
        }
        else
            printf("Cannot read the response\n");
}

//effettua una richiesta http all'indirizzo jsonURL per ottenere una stringa json
void Handler::downloadJsonData(QString jsonURL)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseJsonString(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(jsonURL)));
}

//gestisce la risposta http e tenta il parsing della stringa ricevuta nel payload
void Handler::parseJsonString(QNetworkReply *httpResponse)
{
    qWarning("parsing json...\n");
    QByteArray payload = httpResponse->readAll();
    QString json(payload);
#ifdef PRINT_JSON
    qWarning("json string:\n%s\n",json.toLatin1().constData());
#endif
    QScriptEngine engine;
    QScriptValue jsonData = engine.evaluate(json);
    if(engine.hasUncaughtException())
    {
      qWarning("Uncaught exception\n");
      if(jsonData.isError())
      {
        qWarning("%s\njson string lenght: %d\n",jsonData.toString().toLatin1().constData(),json.length());
#ifndef PRINT_JSON
        qWarning("to see json uncomment the line \"#define PRINT_JSON\" in main.cpp\n");
#endif
      }
    }
    QScriptValue collections = jsonData.property("l");
    if(!collections.isValid())
    {
      qWarning("Invalid property\n");
    }
    if(collections.isArray())
    {
      qWarning("isArray\n");
      QStringList items;
      qScriptValueToSequence(collections, items);
      if(!items.isEmpty())
        qDebug("value %s",items.first().toStdString().c_str());
      else
        qWarning("La lista è vuota\n");
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Handler *h = new Handler();

    QString cid = "d09b3327-c5e9-4d98-b486-ff720352ce53";
    if(argc > 1)
    {
        QString url(argv[1]);
        printf("Asking for synth: %s\n",argv[1]);
        int i = url.indexOf("cid=",0,Qt::CaseInsensitive);
        if(i < 0 || url.length() < i + 40)
        {
            printf("Invalid URL.\n");
            return -1;
        }
        cid = url.mid(i + 4, 36);
    }
    else
        printf("Asking for synth: http://photosynth.net/view.aspx?cid=d09b3327-c5e9-4d98-b486-ff720352ce53\n");

    QtSoapMessage message;
    message.setMethod("GetCollectionData", "http://labs.live.com/");
    message.addMethodArgument("collectionId", "", cid);
    message.addMethodArgument("incrementEmbedCount", "", false, 0);

    transport.setAction("http://labs.live.com/GetCollectionData");
    transport.setHost("photosynth.net");
    QObject::connect(&transport, SIGNAL(responseReady()), h, SLOT(readWSresponse()));

    printf("-----------REQUEST------------\n");
    qWarning("%s\n", message.toXmlString().toLatin1().constData());

    transport.submitRequest(message, "/photosynthws/PhotosynthService.asmx");

    return a.exec();
}
