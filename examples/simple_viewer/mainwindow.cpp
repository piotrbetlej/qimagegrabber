#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentGrabber(NULL)
{
    ui->setupUi(this);

    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    // create docks, toolbars, etc...
    restoreState(settings.value("mainWindowState").toByteArray());

    mjpgGrabber = new QImageGrabberMjpeg(this);
    imageGrabbers.append(mjpgGrabber);
    settings.beginGroup("mjpgGrabber");
    mjpgGrabber->setSource(settings.value("url", "http://127.0.0.1:8080/?action=stream").toString());
    settings.endGroup();

    settings.beginGroup("httpGrabber");
    httpGrabber = new QImageGrabberHttp(this);
    httpGrabber->setSource(settings.value("url", "http://127.0.0.1:8080/?action=snapshot").toString());
    settings.endGroup();

    imageGrabbers.append(httpGrabber);

    foreach (QImageGrabber *currentGrabber, imageGrabbers) {
        ui->comboBoxGrabberTypes->addItem(currentGrabber->grabberName());
    }

    setImageGrabber(httpGrabber);
}

MainWindow::~MainWindow()
{
    settings.beginGroup("mjpgGrabber");
    settings.setValue("url", mjpgGrabber->currentSource());
    settings.endGroup();

    settings.beginGroup("httpGrabber");
    settings.setValue("url", httpGrabber->currentSource());
    settings.endGroup();

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
}

void MainWindow::on_pushButtonSettings_clicked()
{
    settingsDialog.show();
}


void MainWindow::newImageReceieved(QImage *img)
{
    ui->graphicsViewImage->newImageReceieved(img);
}

void MainWindow::newFPSCalculated(double fps, double avg)
{
    ui->labelFPSValue->setText(QString::number(fps).leftJustified(7, '0'));
    ui->labelFPSAvgValue->setText(QString::number(avg).leftJustified(7, '0'));
}

void MainWindow::on_comboBoxGrabberTypes_activated(int index)
{
    setImageGrabber(imageGrabbers.at(index));
}

void MainWindow::setImageGrabber(QImageGrabber *gb)
{
    disconnect(this, SLOT(newImageReceieved(QImage*)));
    disconnect(this, SLOT(newFPSCalculated(double,double)));

    if (currentGrabber != NULL) {
        currentGrabber->stopGrabbing();
    }
    currentGrabber = gb;
    settingsDialog.setImageGrabber(currentGrabber);
    connect(currentGrabber, SIGNAL(newImageGrabbed(QImage*)),this, SLOT(newImageReceieved(QImage*)));
    connect(currentGrabber, SIGNAL(newFPSCalculated(double,double)), this, SLOT(newFPSCalculated(double,double)));
    connect(currentGrabber, SIGNAL(stateChanged(QImageGrabber::GrabbingState)), this, SLOT(grabbingStateChanged(QImageGrabber::GrabbingState)));

    int i = 0;
    foreach(QImageGrabber *cgb, imageGrabbers) {
        if (cgb == currentGrabber) {
            break;
        }
        i++;
    }

    if (ui->comboBoxGrabberTypes->currentIndex() != i)
        ui->comboBoxGrabberTypes->setCurrentIndex(i);
}

void MainWindow::on_pushButtonStart_clicked()
{
    if (currentGrabber != NULL) {
        if (currentGrabber->isGrabbing()) {
            currentGrabber->stopGrabbing();
        } else {
            currentGrabber->startGrabbing();
            ui->graphicsViewImage->grabbingStarted();
        }
    }
}

void MainWindow::grabbingStateChanged(QImageGrabber::GrabbingState newState)
{
    if (newState == QImageGrabber::GrabbingOn)
        ui->pushButtonStart->setText(tr("Stop"));
    else
        ui->pushButtonStart->setText(tr("Start"));
}
