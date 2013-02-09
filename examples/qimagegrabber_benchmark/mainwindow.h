#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTcpSocket>

#include <qimagegrabber.h>
#include <qimagegrabberhttp.h>
#include <qimagegrabbermjpeg.h>
#include <dialogqimagegrabbersettings.h>
#include <dialogcomparesettings.h>

#include "dialogfpsgraph.h"
#include "dialogdelaymeasurehelp.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QImageGrabberHttp *httpGrabber;
    QImageGrabberMjpeg *mjpgGrabber;
    QList<QImageGrabber *> imageGrabbers;
    QImageGrabber *currentGrabber;
    DialogQImageGrabberSettings settingsDialog;
    DialogDelayMeasureHelp delayHelpDialog;
    DialogFPSGraph fpsDialog;
    QSettings *settings;
    QTcpSocket *serverSocket;
    bool firstImageReceieved;

    DialogCompareSettings *compareSettingsDialog;

    void setImageGrabber(QImageGrabber *gb);

private slots:
    void on_checkBoxAutoConnect_toggled(bool checked);
    void on_pushButton_clicked();
    void on_comboBoxGrabberTypes_activated(int index);
    void on_actionFPSDiagram_toggled(bool );
    void on_pushButtonSettings_clicked();
    void newImageReceieved(QImage *);
    void newFPSCalculated(double, double);
    void fpsDialogFinished(int);
    void serverDataAvailable();
    void socketStateChanged(QAbstractSocket::SocketState);
    void blinkOut();
    void on_pushButtonStart_clicked();
    void on_actionHelp_triggered();
    void on_actionSelect_measurement_point_triggered();
    void on_actionTest_red_triggered();
    void on_actionTest_blue_triggered();
    void on_actionMeasure_delay_triggered();
    void on_actionCompareSettings_triggered();
};

#endif // MAINWINDOW_H
