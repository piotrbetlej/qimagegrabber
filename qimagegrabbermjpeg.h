#ifndef IMAGEGRABBERMJPEG_H
#define IMAGEGRABBERMJPEG_H
#include "qimagegrabber.h"
#include <QUrl>
#include <QFile>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


class QImageGrabber;

class QIMAGEGRABBERSHARED_EXPORT QImageGrabberMjpeg : public QImageGrabber
{
    Q_OBJECT

    typedef  enum {
        MjpgBoundary,
        MjpgJpg,
    } MjpgState;

    typedef enum {
        StreamTypeUnknown,
        StreamTypeWebcamXP,
        StreamTypeMjpgStreamer,
    } StreamType;
public:
    QImageGrabberMjpeg(QObject *parent = 0);
    void setUrl(QUrl & url);
    void startGrabbing();
    void stopGrabbing();
    bool isGrabbing();

    QString getDefaultSource() {return "http://127.0.0.1:8080/?action=stream";}

    QString grabberName() {return "HTTP mjpg image grabber";}

    void setFps(double ) {qWarning() << "Not supported option";}
    void setSource(QString str);
    QStringList enumerateSources() {return QStringList();}

private:
    QNetworkAccessManager *downloadManager;
    QNetworkRequest *request;
    QNetworkReply *reply;
    int lastState;
    QUrl currentUrl;

    void sendRequest();

    MjpgState mjpgState;
    StreamType streamType;

    int currentImageSize;

public slots:

private slots:
    void downloadErrorSlot(QNetworkReply::NetworkError);
    void downloadFinished(QNetworkReply *reply);
    void replyDataAvailable();
    void newImageTimeOut();
};

#endif // IMAGEGRABBERMJPEG_H
