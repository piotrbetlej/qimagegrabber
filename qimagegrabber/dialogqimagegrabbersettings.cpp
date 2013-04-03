#include "dialogqimagegrabbersettings.h"
#include "ui_dialogqimagegrabbersettings.h"

DialogQImageGrabberSettings::DialogQImageGrabberSettings(QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::DialogQImageGrabberSettings),
    grabber(NULL),
    sourceCompleter(NULL)
{
    ui->setupUi(this);
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

    ui->lineEditSource->setText(grabber->currentSource());

    ui->spinBoxFPS->setValue(grabber->getFps());

    if (grabber->getFps() == 0) {
        ui->spinBoxFPS->setEnabled(false);
        ui->checkBoxNoLimit->setChecked(true);
    }

    connect(grabber, SIGNAL(stateChanged(QImageGrabber::GrabbingState)), this ,SLOT(grabberStateChanged(QImageGrabber::GrabbingState)));
    connect(grabber, SIGNAL(errorHappend()), this, SLOT(grabberErrorHappend()));

    if (sourceCompleter != NULL) {
        delete sourceCompleter;
    }

    if (gb->isSourceEnumeratorSupported()) {
        sourceCompleter = new QCompleter(gb->enumerateSources());
        ui->lineEditSource->setCompleter(sourceCompleter);
    }

    if (gb->currentSource() == "")
        ui->lineEditSource->setText(gb->getDefaultSource());
    else
        ui->lineEditSource->setText(gb->currentSource());

    QLayoutItem *child;
    while ((child = ui->gridLayoutProperties->takeAt(0)) != 0) {
        delete child;
    }

    int rowCount = 0;
    foreach (QImageGrabberParameter param, gb->getParameterList()) {
        QLabel *paramLabel = new QLabel(ui->groupBoxProperties);
        paramLabel->setText(param.name);

        QWidget *valueWidget = NULL;
        switch(param.value.type()) {
        case QVariant::String: {
            QLineEdit *valueLineEdit = new QLineEdit(this);
            valueLineEdit->setText(param.value.toString());
            valueWidget = valueLineEdit;
        } break;
        case QVariant::LongLong:
        case QVariant::Int: {
            QSpinBox *valueSpinBox = new QSpinBox(this);
            valueSpinBox->setValue(param.value.toLongLong());
            valueWidget = valueSpinBox;
        } break;
        default:
            valueWidget = new QLabel("Unknown type parameter", this);
            break;
        }

        if (param.flags & QImageGrabberParameter::ReadOnly)
            valueWidget->setEnabled(false);

        ui->gridLayoutProperties->addWidget(paramLabel, rowCount, 0);
        ui->gridLayoutProperties->addWidget(valueWidget, rowCount, 1);
        rowCount++;
    }

    this->adjustSize();
}

void DialogQImageGrabberSettings::closeEvent(QCloseEvent *)
{
    if (grabber != NULL) {
        grabber->setSource(ui->lineEditSource->text());
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
    adjustSize();
}

