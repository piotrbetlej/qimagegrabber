#include "dialogcolor.h"
#include "ui_dialogcolor.h"

DialogColor::DialogColor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogColor)
{
    ui->setupUi(this);
}

DialogColor::~DialogColor()
{
    delete ui;
}

void DialogColor::showColor(QString color)
{
    ui->labelColor->setStyleSheet("background-color:"+color);
    this->showFullScreen();
}
