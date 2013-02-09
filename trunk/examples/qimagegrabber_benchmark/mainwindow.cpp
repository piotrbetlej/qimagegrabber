#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentGrabber(NULL)
{
    ui->setupUi(this);

    mjpgGrabber = new QImageGrabberMjpeg(this);
    imageGrabbers.append(mjpgGrabber);

    httpGrabber = new QImageGrabberHttp(this);
    imageGrabbers.append(httpGrabber);

    foreach (QImageGrabber *currentGrabber, imageGrabbers) {
        ui->comboBoxGrabberTypes->addItem(currentGrabber->grabberName());
    }

    connect(&fpsDialog, SIGNAL(finished(int)), this, SLOT(fpsDialogFinished(int)));

    setImageGrabber(httpGrabber);

    settings = new QSettings("settings.ini", QSettings::IniFormat, this);
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSettings_clicked()
{
    settingsDialog.show();
}


void MainWindow::newImageReceieved(QImage *img)
{
    ui->graphicsViewImage->newImageReceieved(img);
}

void MainWindow::on_actionFPSDiagram_toggled(bool on)
{
    fpsDialog.setVisible(on);
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

void MainWindow::on_comboBoxGrabberTypes_activated(int index)
{
    setImageGrabber(imageGrabbers.at(index));
}

void MainWindow::serverDataAvailable()
{
    while (serverSocket->canReadLine()) {
        QString cLine = serverSocket->readLine();
        if (cLine.startsWith("SOURCE=")) {
            currentGrabber->setSource(cLine.trimmed().mid(7));
            qWarning() << cLine.trimmed().mid(7);
        } else if (cLine.startsWith("START")) {
            fpsDialog.start();
            currentGrabber->startGrabbing();
        } else if (cLine.startsWith("SELECT=")) {
            int index = cLine.trimmed().mid(7).toInt();
            if (index < imageGrabbers.size()) {
                setImageGrabber(imageGrabbers.at(index));
            }
        } else if (cLine.startsWith("STOP")) {
            currentGrabber->stopGrabbing();
        } else if (cLine.startsWith("POLL")) {
            serverSocket->write("ACK\n");
            ui->labelBlink->setStyleSheet("background-color: red;");
            QTimer::singleShot(250, this, SLOT(blinkOut()));
        } else if (cLine.startsWith("DUMP")) {
            serverSocket->write("DUMP\n");
            for (qlonglong i = 0; i<fpsDialog.getCnt(); i++) {
                serverSocket->write(QString("%1;%2;\n")
                                    .arg(fpsDialog.getTimeBuffer()->at(i))
                                    .arg(fpsDialog.getFpsBuffer()->at(i)).toAscii());
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
        ui->labelSocketStatus->setText("Connected");
    } else {
        ui->labelSocketStatus->setText("Disconnected");
    }
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

void MainWindow::on_checkBoxAutoConnect_toggled(bool checked)
{
    settings->beginGroup("network");
    settings->setValue("autoconnect", checked);
    settings->endGroup();
}

void MainWindow::blinkOut()
{
    ui->labelBlink->setStyleSheet("");
}

void MainWindow::on_actionHelp_triggered()
{
    delayHelpDialog.show();
}

void MainWindow::on_pushButtonStart_clicked()
{
    currentGrabber->startGrabbing();
    ui->graphicsViewImage->grabbingStarted();
}

void MainWindow::on_actionSelect_measurement_point_triggered()
{
    this->setCursor(Qt::CrossCursor);
}

void MainWindow::on_actionTest_red_triggered()
{

}

void MainWindow::on_actionTest_blue_triggered()
{

}

void MainWindow::on_actionMeasure_delay_triggered()
{

}

void MainWindow::on_actionCompareSettings_triggered()
{
    compareSettingsDialog->show();
}
