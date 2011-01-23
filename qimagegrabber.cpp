#include "qimagegrabber.h"

QImageGrabber::QImageGrabber(QObject *parent) :
    QObject(parent), hasFPSBufferFilled(false)
{
    fps = 0;
    imageReader = new QImageReader();
    imageReader->setAutoDetectImageFormat(false);
    imageReader->setFormat("JPG");

    currentImage = new QImage(640, 480, QImage::Format_RGB888);
    imageBuffer = new QBuffer(this);
    imageBuffer->open(QBuffer::ReadWrite);
    imageReader->setDevice(imageBuffer);
    fps = 0;
    currentState = GrabbingOff;
    currentDelayPointer = 0;
    sum = 0;
    for (int i = 0; i<FPS_ARRAY_SIZE; i++) {
        delays[i] = 0;
    }

    connect(&newImageTimer, SIGNAL(timeout()), this, SLOT(newImageTimeOut()));
}

void QImageGrabber::setError(QString str) {
    errorStr = str;
    emit errorHappend();
    currentState = GrabbingError;
    emit stateChanged(GrabbingError);
}

void QImageGrabber::calcFPS(quint16 newDelay)
{
    if (newDelay != 0) {
        sum -= delays[currentDelayPointer];
        delays[currentDelayPointer] = newDelay;
        sum += newDelay;
        currentDelayPointer++;
        if (currentDelayPointer == FPS_ARRAY_SIZE) {
            currentDelayPointer = 0;
            hasFPSBufferFilled = true;
        }
        if (hasFPSBufferFilled)
            emit newFPSCalculated(1000.0f/newDelay, 1000.0f/(sum/(double)FPS_ARRAY_SIZE));
        else
            emit newFPSCalculated(1000.0f/newDelay, 1000.0f/(sum/(double)currentDelayPointer));
    }
}

