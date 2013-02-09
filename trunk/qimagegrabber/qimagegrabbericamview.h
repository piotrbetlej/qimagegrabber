#ifndef ICAMVIEWGRABBER_H
#define ICAMVIEWGRABBER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QFile>
#include <QTime>
#include <QDebug>
#include <QFile>
#include "qimagegrabber.h"

class QImageGrabberIcamView : public QImageGrabber
{
    enum {

    } IcamViewState;

    Q_OBJECT
public:
    QImageGrabberIcamView(QObject *parent = 0);
    ~QImageGrabberIcamView() {saveFile.close();}
    void connectToHost(QHostAddress address, quint16 port);

    QString errorString() const {return error;}
    void setUserName(QString username) {userName = username;}
    void setPassword(QString pwd) {password = pwd;}

    void setCameraNo(int number) {cameraNumber = number;}

    bool startGrabbing();
    void stopGrabbing(){}

    void setFps(double framePerSec) {fps = framePerSec;}
    void setSource(QString ) {}

    void sendImageRequest();
    void save();

    QStringList enumerateSources();

private:
    QUdpSocket socket;

    void setError(QString errorString);
    QString error;
    QString userName;
    QString password;

    int requestId;

    bool sendLogin();

    void debugArray(QByteArray & array, bool limited = false);
    int cameraNumber;
    QFile saveFile;

    QByteArray receieveArray;
    bool firstAfterLogin;

    int currentImageSize;

    QTimer timeOutTimer;

    QTime reqTime;
signals:
    void errorHappend();

public slots:
private slots:
    void socketStateChanged(QAbstractSocket::SocketState);
    void socketReadyRead();
    void timeOutSlot();
    void newImageTimeOut();
};

#endif // ICAMVIEWGRABBER_H
