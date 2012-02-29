#ifndef IMAGEGRABBERV4L2_H
#define IMAGEGRABBERV4L2_H
#include "qimagegrabber.h"

class QIMAGEGRABBERSHARED_EXPORT QImageGrabberV4L2 : public QImageGrabber
{
public:
    QImageGrabberV4L2(QString devname = "/dev/video0") : deviceName(devname) {}

private:
    QString deviceName;
};

#endif // IMAGEGRABBERV4L2_H
