#include "dialogqimagegrabbersettings.h"
#include "ui_dialogqimagegrabbersettings.h"

DialogQImageGrabberSettings::DialogQImageGrabberSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogQImageGrabberSettings),
    grabber(NULL),
    sourceCompleter(NULL)
{
    ui->setupUi(this);
    propertiesLayout = new QGridLayout(ui->groupBoxProperties);
}

DialogQImageGrabberSettings::~DialogQImageGrabberSettings()
{
    delete ui;
}

void DialogQImageGrabberSettings::setImageGrabber(QImageGrabber *gb)
{
    disconnect(this, SLOT(grabberErrorHappend()));
    grabber = gb;
    connect(gb, SIGNAL(errorHappend()), this, SLOT(grabberErrorHappend()));
    this->setWindowTitle(gb->grabberName());
    ui->lineEditSource->setText(grabber->getSource());
    ui->spinBoxFPS->setValue(grabber->getFps());
    if (grabber->getFps() == 0) {
        ui->spinBoxFPS->setEnabled(false);
        ui->checkBoxNoLimit->setChecked(true);
    }
    connect(grabber, SIGNAL(stateChanged(ImageGrabber::GrabbingState)), this ,SLOT(grabberStateChanged(ImageGrabber::GrabbingState)));
    connect(grabber, SIGNAL(errorHappend()), this, SLOT(grabberErrorHappend()));

    if (sourceCompleter != NULL) {
        delete sourceCompleter;
    }

    if (gb->isSourceEnumeratorSupported()) {
        sourceCompleter = new QCompleter(gb->enumerateSources());
        ui->lineEditSource->setCompleter(sourceCompleter);
    }

    ui->lineEditSource->setText(gb->getDefaultSource());

    QLayoutItem *child;
    while ((child = propertiesLayout->takeAt(0)) != 0) {
        delete child;
    }

    foreach (QImageGrabberParameter param, gb->getParameterList()) {
        QLabel *paramLabel = new QLabel(ui->groupBoxProperties);
        paramLabel->setText(param.name);

        QWidget *valueWidget = NULL;
        switch(param.value.type()) {
        case QVariant::String:
            valueWidget = new QLineEdit(ui->groupBoxProperties);
            break;
        default:
            valueWidget = new QLabel("Unknown type parameter");
            break;
        }
        propertiesLayout->addWidget(paramLabel, propertiesLayout->rowCount(), 0);
        propertiesLayout->addWidget(valueWidget, propertiesLayout->rowCount(), 1);
    }
}

void DialogQImageGrabberSettings::on_checkBoxNoLimit_toggled(bool checked)
{
    ui->spinBoxFPS->setEnabled(!checked);
    if (grabber != NULL) {
        if (checked)  // no limit -> FPS 0
            grabber->setFps(0);
        else
            grabber->setFps(ui->spinBoxFPS->value());
    }
}

void DialogQImageGrabberSettings::on_pushButtonStart_clicked()
{
    if (grabber != NULL) {
        if (grabber->isGrabbing()) {
            grabber->stopGrabbing();
        } else {
            grabber->setSource(ui->lineEditSource->text());
            grabber->startGrabbing();
        }
        ui->pushButtonStart->setText(grabber->isGrabbing()?"Stop":"Start");
    }
}

void DialogQImageGrabberSettings::grabberStateChanged(QImageGrabber::GrabbingState state)
{
    switch (state) {
    case QImageGrabber::GrabbingOn:
        ui->pushButtonStart->setText("Stop"); break;
    default:
        ui->pushButtonStart->setText("Start");
    }
    ui->pushButtonStart->setEnabled(true);
}

void DialogQImageGrabberSettings::grabberErrorHappend()
{
    ui->labelError->setText(grabber->errorString());
    ui->pushButtonStart->setText("Start");
}

void DialogQImageGrabberSettings::on_pushButtonMore_clicked()
{
    ui->pushButtonMore->setText(
            ui->groupBoxProperties->isVisible()?"More":"Hide details");
    ui->groupBoxProperties->setVisible(!ui->groupBoxProperties->isVisible());
}
