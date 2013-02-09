#ifndef IMAGEGRABBER_H
#define IMAGEGRABBER_H

#include <QObject>
#include <QImage>
#include <QBuffer>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QSize>
#include <QTimer>
#include <QTime>
#include <QVariant>

#include "qimagegrabber_global.h"
#define FPS_ARRAY_SIZE 50

class QImageGrabberParameter
{
public:
    enum ParameterFlag {
         NoFlags = 0x0,
         ReadOnly = 0x1,
         WriteOnly = 0x2
     };
    Q_DECLARE_FLAGS(ParameterFlags, ParameterFlag)

    QString name;
    QVariant value;
    ParameterFlags flags;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QImageGrabberParameter::ParameterFlags)


class QIMAGEGRABBERSHARED_EXPORT QImageGrabber : public QObject
{
    Q_OBJECT
public:
    enum GrabbingState {
        GrabbingOff = 0,
        GrabbingTurnOn,
        GrabbingOn,
        GrabbingTurnOff,
        GrabbingError
    };

    enum GrabberType {
        SrcTypeStillJpg,
        SrcTypeWebcamXpMjpg,
        SrcTypeJpgFromUrl,
        SrcTypeJpgsFromDir,
        SrcTypeMjpegStreamerMjpg ,
       SrcTypeV4LDevice
    };


    QImageGrabber(QObject *parent = 0);

    virtual QString grabberName() = 0;

    QString errorString() const {return m_errorStr;}

    virtual bool startGrabbing() = 0;
    virtual void stopGrabbing() = 0;
    virtual bool isGrabbing() const {return (currentState == GrabbingOn);}

    virtual void setFps(double framePerSec) = 0;
    double getFps() const {return fps;}

    virtual void setSource(QString str) = 0;
    virtual QString currentSource() = 0;

    QList<QImageGrabberParameter> getParameterList() const {return m_parameters;}
    bool setParameter(QString name, QVariant value);
    QVariant getParameter(QString name);

    bool isSourceEnumeratorSupported() const {return enumeratorSupported;}
    virtual QStringList enumerateSources() = 0;

    virtual QString getDefaultSource() = 0;

protected:
    QString m_errorStr;
    QBuffer *imageBuffer;
    QImageReader *imageReader;
    QImage *currentImage;
    QString source;
    GrabbingState currentState;

    void setError(QString str);

    double fps;
    quint16 delays[FPS_ARRAY_SIZE];
    int currentDelayPointer;
    bool hasFPSBufferFilled;
    quint64 sum;
    QTime requestTime;

    void calcFPS(quint16 newDelay);

    QList<QImageGrabberParameter> m_parameters;

    bool enumeratorSupported;

    QTimer newImageTimer;

signals:
    void newImageGrabbed(QImage *img);
    void errorHappend();
    void stateChanged(QImageGrabber::GrabbingState newState);
    void newFPSCalculated(double current, double avg);

public slots:

protected slots:
    /*  this slot is connected to the FPS limiting timer's timeout singal
        you should request a new image in you implementation */
    virtual void newImageTimeOut() = 0;
};

#endif // IMAGEGRABBER_H
