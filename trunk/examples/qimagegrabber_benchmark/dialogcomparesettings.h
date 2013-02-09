#ifndef DIALOGCOMPARESETTINGS_H
#define DIALOGCOMPARESETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QTime>
#include <qwt/qwt_plot_curve.h>

#include <qimagegrabbermjpeg.h>
#include <dialogqimagegrabbersettings.h>



namespace Ui {
class DialogCompareSettings;
}

class DialogCompareSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogCompareSettings(QWidget *parent = 0);
    ~DialogCompareSettings();
    
private slots:
    void on_pushButtonSettingsA_clicked();
    void on_pushButtonSettingsB_clicked();
    void on_pushButtonStart_clicked();
    void newImageAReceieved(QImage*);
    void newImageBReceieved(QImage*);

private:
    Ui::DialogCompareSettings *ui;
    QImageGrabberMjpeg *grabberA, *grabberB;
    DialogQImageGrabberSettings *grabberSettingsDialog;
    QSettings settings;

    bool aReceieved, bReceieved;
    QTime aTime, bTime, startTime;

    QVector<double> differences, times;
    QwtPlotCurve *diffCurve;
    bool running;
};

#endif // DIALOGCOMPARESETTINGS_H
