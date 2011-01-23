#ifndef IMAGEGRABBERVFW_H
#define IMAGEGRABBERVFW_H

#include "qimagegrabber.h"
#include <windows.h>
#include <vfw.h>
#include <string.h>
#include <QWidget>

#define MAX_VFW_DEVICES		10
#define ERROR_SIZE		100


class QIMAGEGRABBERSHARED_EXPORT QImageGrabberVfw : public QImageGrabber
{
    Q_OBJECT
public:
    QImageGrabberVfw(QObject *patent = 0);
    QStringList getDevices() const {return devices;}

    void sendRequest();
    void stopGrabbing();
    bool isGrabbing() {return grabbing;}
    void setFps(int framePerSec) {qDebug() << "FPS is not supported on vfw backend";}
    void setSource(QString str);
    void setImageSize(QSize);
    void enumerateSources();

    static void ErrorCallbackProc(HWND hWnd, int nErrID, LPSTR lpErrorText);
    static void StatusCallbackProc(HWND hWnd, int nID, LPCSTR lpsz);
    static void videoFrameCallback(HWND hWnd, LPVIDEOHDR lpVHdr);

private:
    bool openDevice(int DriverIndex);
    void closeDevice();

    HWND m_hWndVideo;
    QStringList devices;
    int m_width, m_height;

    bool grabbing;
};

#endif // IMAGEGRABBERVFW_H
