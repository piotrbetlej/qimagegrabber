#include "dialogfpsgraph.h"
#include "ui_dialogfpsgraph.h"
#include <QDir>
#include <QFile>
#include <QMessageBox>


DialogFPSGraph::DialogFPSGraph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFPSGraph)
{
    ui->setupUi(this);
    ui->lineEditSaveFileName->setText(QDir::homePath()+"/fpslog.csv");
    ui->fpsPlot->setTitle("FPS");
    ui->fpsPlot->setAxisScale(QwtPlot::yLeft, 0, 35);
    ui->fpsPlot->setAutoReplot(true);
    fpsCurve.setTitle("FPS");
    avgCurve.setTitle("FPS avg");
    fpsCurve.setPen(QPen(Qt::red, 1));
    avgCurve.setPen(QPen(Qt::green, 1));
    fpsCurve.attach(ui->fpsPlot);
    avgCurve.attach(ui->fpsPlot);
    cnt = 0;
}

DialogFPSGraph::~DialogFPSGraph()
{
    delete ui;
}

void DialogFPSGraph::on_pushButtonSave_clicked()
{
    QFile file(ui->lineEditSaveFileName->text());
    bool write = true;
    if (file.exists()){
        write = false;
        QMessageBox msgBox;
        msgBox.setText(tr("The file already exists!"));
        msgBox.setInformativeText("Do you want to overwrite it?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if(msgBox.exec() == QMessageBox::Yes)
            write = true;
    }

    if (write) {
        file.open(QFile::ReadWrite);
        for (int i = 0; i<timeBuffer.size(); i++) {
            file.write(QString("%1; %2;\n")
                       .arg(timeBuffer[i])
                       .arg(fpsBuffer[i]).toAscii());
        }
        file.close();
    }
}


void DialogFPSGraph::newFPSget(double fps, double avg)
{
    if (cnt > ui->spinBoxDisplay->value())
        return;

    fpsBuffer.append(fps);
    avgBuffer.append(avg);
    timeBuffer.append(startTime.msecsTo(QTime::currentTime()));
    fpsCurve.setSamples(timeBuffer, fpsBuffer);
    avgCurve.setSamples(timeBuffer, avgBuffer);
    cnt++;
}


void DialogFPSGraph::start()
{
    startTime = QTime::currentTime();
    cnt = 0;
    fpsBuffer.clear();
    avgBuffer.clear();
    fpsCurve.setSamples(timeBuffer, fpsBuffer);
    avgCurve.setSamples(timeBuffer, avgBuffer);
}
