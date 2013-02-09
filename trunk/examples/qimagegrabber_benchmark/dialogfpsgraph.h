#ifndef DIALOGFPSGRAPH_H
#define DIALOGFPSGRAPH_H

#include <QDialog>
#include <QTime>
#include <qwt/qwt_plot_curve.h>

namespace Ui {
    class DialogFPSGraph;
}

class DialogFPSGraph : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFPSGraph(QWidget *parent = 0);
    ~DialogFPSGraph();
    QVector<double>* getFpsBuffer()  {return &fpsBuffer;}
    QVector<double>* getTimeBuffer() {return &timeBuffer;}
    qlonglong getCnt() const {return cnt;}

public slots:
    void newFPSget(double fps, double avg);
    void start();

private:
    Ui::DialogFPSGraph *ui;
    QwtPlotCurve avgCurve, fpsCurve;
    QVector<double> fpsBuffer, avgBuffer, timeBuffer;
    QTime startTime;
    qlonglong cnt;

private slots:
    void on_pushButtonSave_clicked();
};

#endif // DIALOGFPSGRAPH_H
