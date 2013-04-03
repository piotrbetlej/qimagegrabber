#include "qimagegrabberhttp.h"

QImageGrabberHttp::QImageGrabberHttp(QUrl url, QObject *parent)
    : QImageGrabber(parent), currentUrl(url)
{
    init();
}

QImageGrabberHttp::QImageGrabberHttp(QObject *parent)
    : QImageGrabber(parent)
{
    init();
}

void QImageGrabberHttp::init()
{
    enumeratorSupported = false;
    downloadManager = new QNetworkAccessManager(this);
    connect(downloadManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    request = new QNetworkRequest();
    request->setRawHeader("User-Agent", "QImageGrabber");
    reply = NULL;
    setUrl(currentUrl);
}

bool QImageGrabberHttp::isGrabbing()
{
    return currentState == GrabbingOn;
    if (reply != NULL) {
        //return reply->isRunning();
    }
    return false;
}

void QImageGrabberHttp::setFps(double framePerSec)
{
    fps = framePerSec;
    if (framePerSec > 0) {
        imageDelay = (1000/framePerSec);
    } else {
        imageDelay = 0;
    }
}

void QImageGrabberHttp::setUrl(QUrl &url)
{
    currentUrl = url;
    source = url.toString();

    if (!url.isValid()) {
        m_errorStr = QString("Invalid URL: %1").arg(url.toString());
        emit errorHappend();
        this->stopGrabbing();
        return;
    }

    if (this->isGrabbing()) {
        this->stopGrabbing();
        currentUrl = url;
        this->startGrabbing();
    } else {
        currentUrl = url;
    }
}

bool QImageGrabberHttp::startGrabbing()
{
    if (!this->isGrabbing()) {
        if (currentUrl.isValid()) {
            currentState = GrabbingTurnOn;
            requestNewImage();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void QImageGrabberHttp::stopGrabbing()
{
    if (reply != NULL) {
        currentState = GrabbingOff;
        reply->close();
        newImageTimer.stop();
        emit stateChanged(GrabbingOff);
    }
}


void QImageGrabberHttp::downloadFinished(QNetworkReply *reply)
{
    if (currentState == GrabbingTurnOn) {
        currentState = GrabbingOn;
        emit stateChanged(GrabbingOn);
    }

    if (reply->bytesAvailable()) {
        imageReader->setDevice(reply);
        imageReader->read(currentImage);
        emit newImageGrabbed(currentImage);
        calcFPS(requestTime.msecsTo(QTime::currentTime()));
        if (currentState == GrabbingOn) {
            if (fps == 0) {
                requestNewImage();
            } else {
                int elaspedTime = requestTime.msecsTo(QTime::currentTime());
                if (elaspedTime < imageDelay) {
                    QTimer::singleShot(imageDelay-elaspedTime, this, SLOT(requestNewImage()));
                } else {
                    requestNewImage();
                }
            }
        }
    }
}

void QImageGrabberHttp::downloadErrorSlot(QNetworkReply::NetworkError )
{
    if (currentState != GrabbingOff) { // to ignore the Operation cancelled warning
        m_errorStr = reply->errorString();
        emit errorHappend();
        currentState = GrabbingError;
        emit stateChanged(GrabbingError);
    }
}

void QImageGrabberHttp::requestNewImage()
{
    if ((currentState == GrabbingOn) || (currentState == GrabbingTurnOn)) {
        request->setUrl(currentUrl);
        requestTime = QTime::currentTime();
        if (reply) reply->deleteLater();
        reply = downloadManager->get(*request);
        connect(reply , SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadErrorSlot(QNetworkReply::NetworkError)));
    }
}

void QImageGrabberHttp::setSource(QString str)
{
    source = str;
    QUrl url(str);
    this->setUrl(url);
}

void QImageGrabberHttp::newImageTimeOut()
{
    requestNewImage();
}
