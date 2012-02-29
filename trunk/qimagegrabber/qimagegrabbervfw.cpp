#include "qimagegrabbervfw.h"

QImageGrabberVfw::QImageGrabberVfw(QObject *parent)
    : ImageGrabber(parent)
{
    grabbing = false;
    m_hWndVideo = NULL;
    m_height = 480;
    m_width = 640;
    enumerateSources();
}

void QImageGrabberVfw::sendRequest()
{
    openDevice(0);
}

void QImageGrabberVfw::setSource(QString str)
{
    int index = devices.indexOf(str);
    if (index != -1) {
        openDevice(index);
    } else {
        errorStr = QString(tr("Invalid device: %1")).arg(str);
    }
}

/*******************************************************************************
   Function   : Initialize
   Arguments  : DriverIndex (input) - Index of VFW driver.
   Return     : true Success, false Failure
   Description: Inititlizes the object for using VFW interface to capture
                device.
*******************************************************************************/
void QImageGrabberVfw::enumerateSources()
{
    for (int i = 0; i < MAX_VFW_DEVICES; i++) {
        WCHAR szDeviceName[80];
        WCHAR szDeviceVersion[80];
        if(!capGetDriverDescription(i,
                                    szDeviceName,
                                    sizeof(szDeviceName),
                                    szDeviceVersion,
                                    sizeof(szDeviceVersion))) {
            break;
        }
        devices.append(QString::fromWCharArray(szDeviceName));
        qWarning() << devices.last() << QString::fromWCharArray(szDeviceVersion);
    }
}


/*******************************************************************************
   Function   : Destroy
   Arguments  : none
   Return     : none
   Description: Closes up the interface for VFW of capture device.
*******************************************************************************/
void QImageGrabberVfw::stopGrabbing()
{
    if (isGrabbing()) {       

    }
}


/*******************************************************************************
   Function   : SetDriver
   Arguments  : DriverIndex (input) - Driver to set
   Return     : true Success, false Failed.
   Description: Sets curretn capture driver.
*******************************************************************************/
bool QImageGrabberVfw::openDevice(int index)
{
    try {
        if (!(index < devices.size())) {
            setError(tr("Index out of bounds"));
        }

        if(m_hWndVideo != NULL)
            closeDevice();

        // Connect to the daemon
        WCHAR *wcharStr;
        QString title("title");
        title.toWCharArray(wcharStr);
        m_hWndVideo = capCreateCaptureWindow (wcharStr,        // window name if pop-up
                                              WS_VISIBLE,                          // window style (not visible)
                                              0, 0, m_width, m_height,// window position and dimensions
                                              GetDesktopWindow(), 0);
        if (!m_hWndVideo) {
            setError(tr("Unable to create capture window"));
            return false;
        }

        if (!capDriverConnect(m_hWndVideo, index)) {
            setError("Unable to connect to video driver");
            closeDevice();
            return false;
        }

        CAPTUREPARMS params;
        if (!capCaptureGetSetup(m_hWndVideo, &params, sizeof(CAPTUREPARMS))) {
            setError(tr("Unable to get capture parameters"));
            closeDevice();
            return false;
        }

        params.fYield = TRUE;
        params.fCaptureAudio = FALSE;
        params.wPercentDropForError = 100;
        params.fLimitEnabled = FALSE;
        //params.AVStreamMaster = AVSTREAMMASTER_NONE;
        params.fStepCaptureAt2x = FALSE;
        params.fAbortLeftMouse = FALSE;
        params.fAbortRightMouse = FALSE;
        if (!capCaptureSetSetup(m_hWndVideo, &params, sizeof(CAPTUREPARMS))) {
            setError(tr("Unable to set capture parameters"));
            closeDevice();
            return false;
        }

        if (!capSetCallbackOnVideoStream(m_hWndVideo, videoFrameCallback)) {
            setError(tr("Unable to set frame callback"));
            closeDevice();
            return false;
        }

        if (!capSetUserData(m_hWndVideo, this)) {
            setError(tr("Unable to set user data"));
            closeDevice();
            return false;
        }

        DWORD formSize = capGetVideoFormat(m_hWndVideo, NULL, 0);
        BITMAPINFO *videoFormat = (BITMAPINFO *)(new char[formSize]);
        if (!capGetVideoFormat(m_hWndVideo, videoFormat, formSize)) {
            setError(tr("Unable to get video format"));
            closeDevice();
            return false;
        }

        videoFormat->bmiHeader.biWidth = m_width;
        videoFormat->bmiHeader.biHeight = m_height;
        videoFormat->bmiHeader.biBitCount = 24;
        videoFormat->bmiHeader.biCompression = BI_RGB;
        videoFormat->bmiHeader.biClrUsed = 0;
        videoFormat->bmiHeader.biClrImportant = 0;
        videoFormat->bmiHeader.biSizeImage = 0;
        if (!capSetVideoFormat(m_hWndVideo, videoFormat, formSize)) {
            setError(tr("Unable to set video format"));
            delete videoFormat;
            closeDevice();
            return false;
        }

        if (!capGetVideoFormat(m_hWndVideo, videoFormat, formSize)) {
            setError(tr("Unable to get video format"));
        }

        m_width=static_cast<int>(videoFormat->bmiHeader.biWidth);
        m_height=static_cast<int>(videoFormat->bmiHeader.biHeight);

        qWarning() <<"Connection ok" << m_width << m_height;

        delete videoFormat;
    } catch(...) {
        qWarning() << "unandlede";
    }
    return true;
}

//
// Internal callback functions.
//
void QImageGrabberVfw::ErrorCallbackProc(HWND hWnd, int nErrID, LPSTR lpErrorText)
{
    qWarning() << "err" << nErrID << lpErrorText;
}


void QImageGrabberVfw::StatusCallbackProc(HWND hWnd, int nID, LPCSTR lpsz)
{

    /*switch(nID)
    {
    case IDS_CAP_BEGIN:
        break;

    case IDS_CAP_END:
        break;
    }*/
    qWarning() << "status " << nID;
}

void QImageGrabberVfw::videoFrameCallback(HWND hWnd, LPVIDEOHDR lpVHdr)
{
    qWarning() << "New image";
    /*if (!currentImage->loadFromData(lpVHdr->lpData, lpVHdr->dwBytesUsed, "BMP")) {
        qWarning() << "Unable to load";
    }*/
}

void QImageGrabberVfw::closeDevice()
{
    if (m_hWndVideo != NULL) {
        capDriverDisconnect(m_hWndVideo);
        DestroyWindow(m_hWndVideo);
    }
    m_hWndVideo=NULL;
}

void QImageGrabberVfw::setImageSize(QSize size)
{
    m_height = size.height();
    m_width = size.width();
}
