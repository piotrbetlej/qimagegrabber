#include "qimagegrabbermjpeg.h"


QImageGrabberMjpeg::QImageGrabberMjpeg(QObject *parent)
    : QImageGrabber(parent)
{
    m_downloadManager = new QNetworkAccessManager(this);
    connect(m_downloadManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    m_request = new QNetworkRequest();
    m_request->setRawHeader("User-Agent", "Mars2020 Imagegrabber LIB");
    m_reply = NULL;

    QImageGrabberParameter boundaryParam;
    boundaryParam.name = tr("Boundary");
    boundaryParam.value = "boundarydonotcross";
    m_parameters.append(boundaryParam);

    QImageGrabberParameter timeStampParameter;
    timeStampParameter.name = "Frame timestamp";
    timeStampParameter.value = -1;
    timeStampParameter.flags |= QImageGrabberParameter::ReadOnly;
    m_parameters.append(timeStampParameter);

    m_timestampRegexp.setPattern("X-Timestamp: (\\d+).(\\d+)\\r\\n");
}

bool QImageGrabberMjpeg::isGrabbing()
{
    return currentState == GrabbingOn;
}

void QImageGrabberMjpeg::setUrl(QUrl &url)
{
    if (this->isGrabbing()) {
        this->stopGrabbing();
        m_currentUrl = url;
        this->sendRequest();
    } else {
        m_currentUrl = url;
    }
}

bool QImageGrabberMjpeg::startGrabbing()
{
    return sendRequest();
}

void QImageGrabberMjpeg::stopGrabbing()
{
    currentState = GrabbingOff;
    emit stateChanged(GrabbingOff);
    if (m_reply != NULL) {
        m_reply->abort();
    }
}


void QImageGrabberMjpeg::downloadFinished(QNetworkReply *reply)
{
    imageReader->setDevice(reply);
    qWarning() << "start reading image";
    imageReader->read(currentImage);
    qWarning() << "reading image done";
    emit newImageGrabbed(currentImage);
}

void QImageGrabberMjpeg::downloadErrorSlot(QNetworkReply::NetworkError )
{
    if (m_reply != NULL) {
        m_errorStr = m_reply->errorString();
        emit errorHappend();
    }
}

bool QImageGrabberMjpeg::sendRequest()
{
    if (m_currentUrl.isValid()) {
        m_request->setUrl(m_currentUrl);

        m_reply = m_downloadManager->get(*m_request);
        connect(m_reply , SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadErrorSlot(QNetworkReply::NetworkError)));
        connect(m_reply, SIGNAL(readyRead()), this, SLOT(replyDataAvailable()));

        m_mjpgState = MjpgBoundary;
        m_streamType = StreamTypeUnknown;
        m_currentImageSize = 0;
        imageBuffer->seek(0);
        requestTime = QTime::currentTime();
        emit stateChanged(GrabbingTurnOn);
        return true;
    } else {
        return false;
    }
}

void QImageGrabberMjpeg::replyDataAvailable()
{
    if (currentState != GrabbingOn) {
        currentState = GrabbingOn;
        emit stateChanged(currentState);
    }
    QString cLine;
    if (m_mjpgState == MjpgBoundary) {
        if (m_streamType == StreamTypeUnknown) {
            if (m_reply->bytesAvailable() >= 50) {
                cLine = m_reply->readLine(51);
                if (cLine.startsWith("mjpeg")) {
                    m_streamType = StreamTypeWebcamXP;
                    bool ok = false;
                    m_currentImageSize = cLine.mid(5,8).toInt(&ok);
                    if (ok) {
                        imageBuffer->seek(0);
                        m_mjpgState = MjpgJpg;
                        qWarning() << m_currentImageSize << "CI" << cLine;
                    } else {
                        qWarning() << QString("Could not convert %1 to number").arg(cLine.mid(5,7));
                    }
                    // we need to seek a bit
                } else {
                    m_streamType = StreamTypeMjpgStreamer;
                }
            } else { // to few bytes came so return the next readyíread will take us further
                return;
            }
        }

        if (m_mjpgState == MjpgBoundary) {
            if (m_streamType == StreamTypeWebcamXP) {
                if (m_reply->bytesAvailable() >= 50) {
                    QByteArray borderArray = m_reply->read(51);
                    if (!borderArray.startsWith("mjpeg")) {
                        qWarning() << "invalid border" << borderArray;
                        return;
                    } else {
                        bool ok = false;
                        m_currentImageSize = borderArray.mid(5,8).toInt(&ok);
                        if (ok) {
                            imageBuffer->seek(0);
                            m_mjpgState = MjpgJpg;
                            qWarning() << m_currentImageSize << "CI" << borderArray;
                        } else {
                            qWarning() << QString("Could not convert %1 to number").arg(QString(borderArray.mid(5,7)));
                        }
                    }
                } else {  // too few bytes came so return the next readyíread will take us further
                    return;
                }
            } else if(m_streamType == StreamTypeMjpgStreamer) {
                bool quitNext = false;
                while(m_reply->canReadLine()) {
                    QString cLine = m_reply->readLine();
                    if (quitNext)
                        break;
                    if (cLine.startsWith("Content-Length:")) {
                        bool ok = false;
                        m_currentImageSize = cLine.mid(16).toInt(&ok);
                        if (!ok) {
                            qWarning() << QString("Could not convert %1 to number").arg(cLine.mid(16));
                            return;
                        }
                    } else if (cLine.startsWith("X-Timestamp:")) {
                        if (m_timestampRegexp.indexIn(cLine) > -1) {
                            m_timestampInMs =
                                    m_timestampRegexp.cap(1).toLong() * 1000 +
                                    m_timestampRegexp.cap(2).toLong();
                        }
                        m_mjpgState = MjpgJpg;
                        quitNext = true;
                    }
                }
            }
        }
    }

    if (m_mjpgState == MjpgJpg) {
        imageBuffer->write(m_reply->read(m_currentImageSize - imageBuffer->pos()));
        if (imageBuffer->pos() == m_currentImageSize) {
            bool ok = false;
            imageReader->setDevice(imageBuffer);
            imageBuffer->seek(0);
            ok = imageReader->read(currentImage);
            imageBuffer->seek(0);
            if (ok == true) {
                emit newImageGrabbed(currentImage);
                calcFPS(requestTime.msecsTo(QTime::currentTime()));
                requestTime = QTime::currentTime();
            } else {
                qWarning() << "Image read fail" << imageReader->errorString();
            }
            m_mjpgState = MjpgBoundary;
        }
    }
}

void QImageGrabberMjpeg::setSource(QString str)
{
    QUrl checkUrl(str);
    if (checkUrl.isValid()) {
        m_currentUrl = checkUrl;
    } else {
        m_errorStr = tr("Invalid URL");
        emit errorHappend();
    }
}

void QImageGrabberMjpeg::newImageTimeOut()
{
    qWarning() << "Hogy a faszba hívódott ez itt meg";
}
