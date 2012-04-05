#ifndef DIALOGIMAGEGRABBERSETTINGS_H
#define DIALOGIMAGEGRABBERSETTINGS_H

#include <QDialog>
#include <QCompleter>
#include <QGridLayout>
#include "qimagegrabber.h"

namespace Ui {
    class DialogQImageGrabberSettings;
}

class QIMAGEGRABBERSHARED_EXPORT DialogQImageGrabberSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogQImageGrabberSettings(QWidget *parent = 0);
    ~DialogQImageGrabberSettings();
    void setImageGrabber(QImageGrabber *gb);

protected:
    void closeEvent(QCloseEvent *);
private:
    Ui::DialogQImageGrabberSettings *ui;
    QImageGrabber *grabber;
    QCompleter *sourceCompleter;
public slots:
    void on_pushButtonStart_clicked();

private slots:
    void on_pushButtonMore_clicked();
    void grabberStateChanged(QImageGrabber::GrabbingState state);
    void grabberErrorHappend();
    void on_checkBoxNoLimit_toggled(bool checked);
};

#endif // DIALOGIMAGEGRABBERSETTINGS_H
