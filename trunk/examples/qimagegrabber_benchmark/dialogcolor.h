#ifndef DIALOGCOLOR_H
#define DIALOGCOLOR_H

#include <QDialog>

namespace Ui {
class DialogColor;
}

class DialogColor : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogColor(QWidget *parent = 0);
    ~DialogColor();
    void showColor(QString color);

private:
    Ui::DialogColor *ui;
};

#endif // DIALOGCOLOR_H
