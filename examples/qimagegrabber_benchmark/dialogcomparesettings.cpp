#include "dialogcomparesettings.h"
#include "ui_dialogcomparesettings.h"

#include <QDebug>

DialogCompareSettings::DialogCompareSettings(QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::DialogCompareSettings),
    running(false)
{
    ui->setupUi(this);
    grabberA = new QImageGrabberMjpeg(this);
    grabberB = new QImageGrabberMjpeg(this);

    settings.beginGroup("compare");
    grabberA->setSource(settings.value("grabberASrc").toString());
    grabberB->setSource(settings.value("grabberBSrc").toString());
    settings.endGroup();

    connect(grabberA, SIGNAL(newImageGrabbed(QImage*)), this, SLOT(newImageAReceieved(QImage*)));
    connect(grabberB, SIGNAL(newImageGrabbed(QImage*)), this, SLOT(newImageBReceieved(QImage*)));

    grabberSettingsDialog = new DialogQImageGrabberSettings(this);

    diffCurve = new QwtPlotCurve(QString("Difference %1 vs %2")
                                 .arg(grabberA->currentSource())
                                 .arg(grabberB->currentSource()));
    diffCurve->setPen(QPen(Qt::yellow, 1));
    diffCurve->attach(ui->qwtPlot);

    ui->qwtPlot->setTitle(QString("%1 vs %2 in ms")
                          .arg(grabberA->currentSource())
                          .arg(grabberB->currentSource()));
    ui->qwtPlot->setAxisAutoScale(QwtPlot::yLeft);
    ui->qwtPlot->setAutoReplot(true);
}

DialogCompareSettings::~DialogCompareSettings()
{
    settings.beginGroup("compare");
    if (grabberA->currentSource() == "") {
        settings.setValue("grabberASrc", grabberA->getDefaultSource());
    } else {
        settings.setValue("grabberASrc", grabberA->currentSource());
    }

    if (grabberB->currentSource() == "") {
        settings.setValue("grabberBSrc", grabberB->getDefaultSource());
    } else {
        settings.setValue("grabberBSrc", grabberB->currentSource());
    }

    settings.endGroup();
    settings.sync();
    delete ui;
}

void DialogCompareSettings::on_pushButtonSettingsA_clicked()
{
    grabberSettingsDialog->setImageGrabber(grabberA);
    grabberSettingsDialog->show();
}

void DialogCompareSettings::on_pushButtonSettingsB_clicked()
{
    grabberSettingsDialog->setImageGrabber(grabberB);
    grabberSettingsDialog->show();
}

void DialogCompareSettings::on_pushButtonStart_clicked()
{
    running = !running;

    if (running) {
        startTime = QTime::currentTime();
        differences.clear();
        times.clear();

        grabberA->startGrabbing();
        grabberB->startGrabbing();

        aReceieved = false;
        bReceieved = false;
        ui->pushButtonStart->setText("Stop");
    } else {
        grabberA->stopGrabbing();
        grabberB->stopGrabbing();
        ui->pushButtonStart->setText("Start");
    }
}

void DialogCompareSettings::newImageAReceieved(QImage *)
{
    aReceieved = true;
    aTime = QTime::currentTime();
    if (bReceieved) {
        differences.append(aTime.msecsTo(bTime));
        times.append(startTime.msecsTo(QTime::currentTime()));

        qWarning() << differences.last() << "A" << times.last();

        diffCurve->setSamples(times, differences);

        aReceieved = false;
        bReceieved = false;
    }
}

void DialogCompareSettings::newImageBReceieved(QImage *)
{
    bReceieved = true;
    bTime = QTime::currentTime();
    if (aReceieved) {
        differences.append(aTime.msecsTo(bTime));
        times.append(startTime.msecsTo(QTime::currentTime()));

        qWarning() << differences.last() << "B" << times.last();

        diffCurve->setSamples(times, differences);

        aReceieved = false;
        bReceieved = false;
    }
}
