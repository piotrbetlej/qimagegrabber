#ifndef IMAGEGRABBERMJPEG_H
#define IMAGEGRABBERMJPEG_H
#include "qimagegrabber.h"
#include <QUrl>
#include <QFile>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRegExp>


class QImageGrabber;

class QIMAGEGRABBERSHARED_EXPORT QImageGrabberMjpeg : public QImageGrabber
{
    Q_OBJECT

    typedef  enum {
        MjpgBoundary,
        MjpgJpg
    } MjpgState;

    typedef enum {
        StreamTypeUnknown,
        StreamTypeWebcamXP,
        StreamTypeMjpgStreamer
    } StreamType;
public:
    QImageGrabberMjpeg(QObject *parent = 0);
    void setUrl(QUrl & url);
    bool startGrabbing();
    void stopGrabbing();
    bool isGrabbing();

    QString getDefaultSource() {return "http://127.0.0.1:8080/?action=stream";}

    QString grabberName() {return "HTTP mjpg image grabber";}

    void setFps(double ) {}
    void setSource(QString str);
    QString currentSource() {return m_currentUrl.toString();}
    QStringList enumerateSources() {return QStringList();}

private:
    QNetworkAccessManager *m_downloadManager;
    QNetworkRequest *m_request;
    QNetworkReply *m_reply;
    int m_lastState;
    QUrl m_currentUrl;

    bool sendRequest();

    MjpgState m_mjpgState;
    StreamType m_streamType;

    int m_currentImageSize;

    quint64 m_timestampInMs;
    QRegExp m_timestampRegexp;

public slots:

private slots:
    void downloadErrorSlot(QNetworkReply::NetworkError);
    void downloadFinished(QNetworkReply *m_reply);
    void replyDataAvailable();
    void newImageTimeOut();
};

#endif // IMAGEGRABBERMJPEG_H
