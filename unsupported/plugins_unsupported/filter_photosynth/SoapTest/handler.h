#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QNetworkReply>

class Handler : public QObject
{
    Q_OBJECT

public:
    Handler(QObject *parent = 0);
    ~Handler();
    void downloadJsonData(QString jsonURL);
public slots:
    void readWSresponse();
    void parseJsonString(QNetworkReply *httpResponse);
};

#endif // HANDLER_H
