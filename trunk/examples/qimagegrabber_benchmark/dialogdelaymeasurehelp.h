#ifndef DIALOGLAGGHELP_H
#define DIALOGLAGGHELP_H

#include <QDialog>

namespace Ui {
class DialogDelayMeasureHelp;
}

class DialogDelayMeasureHelp : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogDelayMeasureHelp(QWidget *parent = 0);
    ~DialogDelayMeasureHelp();
    
private:
    Ui::DialogDelayMeasureHelp *ui;
};

#endif // DIALOGLAGGHELP_H
