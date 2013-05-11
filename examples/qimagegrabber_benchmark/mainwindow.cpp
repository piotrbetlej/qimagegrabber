#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentImageGrabber(NULL)
{
    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    mjpgGrabber = new QImageGrabberMjpeg(this);
    imageGrabbers.append(mjpgGrabber);


    httpGrabber = new QImageGrabberHttp(this);
    imageGrabbers.append(httpGrabber);

    loadSettings();

    foreach (QImageGrabber *imageGrabber, imageGrabbers) {
        ui->comboBoxGrabberTypes->addItem(imageGrabber->grabberName());
    }

    connect(&fpsDialog, SIGNAL(finished(int)), this, SLOT(fpsDialogFinished(int)));

    setImageGrabber(mjpgGrabber);

    settings->beginGroup("network");
    serverSocket = new QTcpSocket(this);
    connect(serverSocket, SIGNAL(readyRead()), this ,SLOT(serverDataAvailable()));
    connect(serverSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));

    ui->lineEditServerIp->setText(settings->value("serveraddr").toString());
    ui->spinBoxServerPort->setValue(settings->value("serverport").toInt());
    if (settings->value("autoconnect").toBool()) {
        serverSocket->connectToHost(settings->value("serveraddr").toString(),
                                    settings->value("serverport").toInt());
        ui->checkBoxAutoConnect->setChecked(true);
    }
    settings->endGroup();

    compareSettingsDialog = new DialogCompareSettings(this);

    colorChangeTimer = new QTimer(this);
    connect(colorChangeTimer, SIGNAL(timeout()), this, SLOT(delayTestPhaseChangedSlot()));
}

MainWindow::~MainWindow()
{
    on_actionSave_settings_triggered();
    delete ui;
}

void MainWindow::newImageReceieved(QImage *img)
{
    ui->graphicsViewImage->newImageReceieved(img);
}

void MainWindow::newFPSCalculated(double fps, double avg)
{
    ui->labelFPSValue->setText(QString::number(fps).leftJustified(7, '0'));
    ui->labelFPSAvgValue->setText(QString::number(avg).leftJustified(7, '0'));
    fpsDialog.newFPSget(fps, avg);
}


void MainWindow::fpsDialogFinished(int)
{
    ui->actionFPSDiagram->setChecked(false);
}

void MainWindow::serverDataAvailable()
{
    while (serverSocket->canReadLine()) {
        QString cLine = serverSocket->readLine();
        if (cLine.startsWith("SOURCE=")) {
            currentImageGrabber->setSource(cLine.trimmed().mid(7));
            qWarning() << cLine.trimmed().mid(7);
        } else if (cLine.startsWith("START")) {
            fpsDialog.start();
            currentImageGrabber->startGrabbing();
        } else if (cLine.startsWith("SELECT=")) {
            int index = cLine.trimmed().mid(7).toInt();
            if (index < imageGrabbers.size()) {
                setImageGrabber(imageGrabbers.at(index));
            }
        } else if (cLine.startsWith("STOP")) {
            currentImageGrabber->stopGrabbing();
        } else if (cLine.startsWith("POLL")) {
            serverSocket->write("ACK\n");
            ui->labelBlink->setStyleSheet("background-color: red;");
            QTimer::singleShot(250, this, SLOT(blinkOut()));
        } else if (cLine.startsWith("DUMP")) {
            serverSocket->write("DUMP\n");
            for (qlonglong i = 0; i<fpsDialog.getCnt(); i++) {
                serverSocket->write(QString("%1;%2;\n")
                                    .arg(fpsDialog.getTimeBuffer()->at(i))
                                    .arg(fpsDialog.getFpsBuffer()->at(i)).toLocal8Bit());
            }
            serverSocket->write("ENDDUMP\n");
            qWarning() << "ENDDUMP";
        } else {
            qWarning() << "unknown data" << cLine;
        }

    }
}

void MainWindow::socketStateChanged(QAbstractSocket::SocketState newState)
{
    if (newState == QAbstractSocket::ConnectedState) {
        ui->labelSocketStatus->setText(tr("Connected"));
    } else {
        ui->labelSocketStatus->setText(tr("Disconnected"));
    }
}

void MainWindow::setImageGrabber(QImageGrabber *newGrabberb)
{
    disconnect(this, SLOT(newImageReceieved(QImage*)));
    disconnect(this, SLOT(newFPSCalculated(double,double)));

    if (currentImageGrabber != NULL) {
        currentImageGrabber->stopGrabbing();
    }

    currentImageGrabber = newGrabberb;

    settingsDialog.setImageGrabber(currentImageGrabber);
    connect(currentImageGrabber, SIGNAL(newImageGrabbed(QImage*)),this, SLOT(newImageReceieved(QImage*)));
    connect(currentImageGrabber, SIGNAL(newFPSCalculated(double,double)), this, SLOT(newFPSCalculated(double,double)));

    int i = 0;
    foreach(QImageGrabber *imageGrabber, imageGrabbers) {
        if (imageGrabber == currentImageGrabber) {
            break;
        }
        i++;
    }

    if (ui->comboBoxGrabberTypes->currentIndex() != i)
        ui->comboBoxGrabberTypes->setCurrentIndex(i);
}

void MainWindow::loadSettings()
{
    settings->beginGroup("httpGrabber");
    httpGrabber->loadSettings(settings);
    settings->endGroup();

    settings->beginGroup("mjpgGrabber");
    mjpgGrabber->loadSettings(settings);
    settings->endGroup();
}

void MainWindow::blinkOut()
{
    ui->labelBlink->setStyleSheet("");
}

void MainWindow::delayTestPhaseChangedSlot()
{
    switch (delayTestPhase) {
    case ScreenRed:
        break;
    case ScreenBlue:
        break;
    case Idle:
        break;
    }
    colorDialog.close();
}

void MainWindow::on_pushButtonSettings_clicked()
{
    settingsDialog.show();
}

void MainWindow::on_comboBoxGrabberTypes_activated(int index)
{
    setImageGrabber(imageGrabbers.at(index));
}

void MainWindow::on_actionFPSDiagram_toggled(bool on)
{
    fpsDialog.setVisible(on);
}

void MainWindow::on_checkBoxAutoConnect_toggled(bool checked)
{
    settings->beginGroup("network");
    settings->setValue("autoconnect", checked);
    settings->endGroup();
}

void MainWindow::on_pushButton_clicked()
{
    serverSocket->abort();
    settings->beginGroup("network");
    settings->setValue("serveraddr", ui->lineEditServerIp->text());
    settings->setValue("serverport", ui->spinBoxServerPort->value());
    serverSocket->connectToHost(settings->value("serveraddr").toString(),
                                settings->value("serverport").toInt());
    ui->labelSocketStatus->setText("Connecting");
    settings->endGroup();
}


void MainWindow::on_actionHelp_triggered()
{
    delayHelpDialog.show();
}

void MainWindow::on_pushButtonStart_clicked()
{
    currentImageGrabber->startGrabbing();
    ui->graphicsViewImage->grabbingStarted();
}

void MainWindow::on_actionSelect_measurement_point_triggered()
{
    this->setCursor(Qt::CrossCursor);
}

void MainWindow::on_actionTest_red_triggered()
{
    colorChangeTimer->start(1000);
    colorDialog.showColor("red");
}

void MainWindow::on_actionTest_blue_triggered()
{
    colorChangeTimer->start(1000);
    colorDialog.showColor("blue");
}

void MainWindow::on_actionMeasure_delay_triggered()
{

}

void MainWindow::on_actionCompareSettings_triggered()
{
    compareSettingsDialog->show();
}

void MainWindow::on_actionSave_settings_triggered()
{
    settings->beginGroup("httpGrabber");
    httpGrabber->saveSettings(settings);
    settings->endGroup();

    settings->beginGroup("mjpgGrabber");
    mjpgGrabber->saveSettings(settings);
    settings->endGroup();
}

