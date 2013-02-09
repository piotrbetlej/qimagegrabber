#ifndef IMAGEGRABBERHTTP_H
#define IMAGEGRABBERHTTP_H
#include "qimagegrabber.h"
#include <QUrl>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class QImageGrabber;

class QIMAGEGRABBERSHARED_EXPORT QImageGrabberHttp : public QImageGrabber
{
    Q_OBJECT
public:
    QImageGrabberHttp(QObject *parent = 0);
    QImageGrabberHttp(QUrl url, QObject *patent = 0);
    QString grabberName() {return "HTTP single image grabber";}
    void setUrl(QUrl & url);
    bool startGrabbing();
    void stopGrabbing();
    bool isGrabbing();
    void setFps(double framePerSec);
    void setSource(QString str);
    QString currentSource() {return currentUrl.toString();}

    QStringList enumerateSources() {return QStringList();}
    QString getDefaultSource() {return "http://127.0.0.1:8080/?action=snapshot";}

private:
    QNetworkAccessManager *downloadManager;
    QNetworkRequest *request;
    QNetworkReply *reply;
    int lastState;
    QUrl currentUrl;

    void init();

    int imageDelay;

public slots:

private slots:
    void requestNewImage();
    void downloadErrorSlot(QNetworkReply::NetworkError);
    void downloadFinished(QNetworkReply *reply);
    void newImageTimeOut();
};

#endif // IMAGEGRABBERHTTP_H
