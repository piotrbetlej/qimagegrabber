#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>

#include <qimagegrabber.h>
#include <qimagegrabberhttp.h>
#include <qimagegrabbermjpeg.h>
#include <dialogqimagegrabbersettings.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    QImageGrabberHttp *httpGrabber;
    QImageGrabberMjpeg *mjpgGrabber;
    QList<QImageGrabber *> imageGrabbers;
    QImageGrabber *currentGrabber;
    DialogQImageGrabberSettings settingsDialog;
    QSettings settings;
    bool firstImageReceieved;


    void setImageGrabber(QImageGrabber *gb);

private slots:
    void on_comboBoxGrabberTypes_activated(int index);
    void on_pushButtonSettings_clicked();
    void newImageReceieved(QImage *);
    void newFPSCalculated(double, double);
    void on_pushButtonStart_clicked();
    void grabbingStateChanged(QImageGrabber::GrabbingState newState);
};

#endif // MAINWINDOW_H
