#include "dialogdelaymeasurehelp.h"
#include "ui_dialogdelaymeasurehelp.h"

DialogDelayMeasureHelp::DialogDelayMeasureHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDelayMeasureHelp)
{
    ui->setupUi(this);
}

DialogDelayMeasureHelp::~DialogDelayMeasureHelp()
{
    delete ui;
}
