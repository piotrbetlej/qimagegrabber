#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsScene>
#include <QtOpenGL/QGLWidget>
#include <QDebug>

#include "imageitem.h"

class ImageView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ImageView(QWidget *parent = 0);
    void grabbingStarted();

private:
    unsigned int zoomSteps;
    QGraphicsScene *scene;
    ImageItem *imageItem;
    bool firstImageReceieved;

signals:
    
public slots:
    void newImageReceieved(QImage *image);
    virtual void wheelEvent(QWheelEvent *);
};

#endif // IMAGEVIEW_H
