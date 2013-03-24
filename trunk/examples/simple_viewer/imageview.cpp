#include "imageview.h"

ImageView::ImageView(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    scene = new QGraphicsScene(this);
    this->setScene(scene);
    firstImageReceieved = false;

    imageItem = new ImageItem();
    scene->addItem(imageItem);

    this->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));  // opengl rulz :) lenne ha az ati nem egy buzi cég lenne
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // zoomoláshoz kell
    this->setRenderHint(QPainter::HighQualityAntialiasing, true);
    this->setCacheMode(CacheBackground);
    this->setViewportUpdateMode(BoundingRectViewportUpdate);
    this->setResizeAnchor(AnchorViewCenter);
}

void ImageView::grabbingStarted()
{
    firstImageReceieved = false;
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    float scale = 1;

    if (event->delta() > 0)
    {
        scale=1.1;
        zoomSteps++;
        if (zoomSteps == 1)
        {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        }
    }
    else
    {
        if (zoomSteps != 0)
        {
            zoomSteps--;
            scale=1/1.1f;
            this->scale(scale, scale);
            return;
        }
        else
        {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    this->scale(scale, scale);
}


void ImageView::newImageReceieved(QImage *image)
{
    if (!firstImageReceieved) {
        firstImageReceieved = true;
        imageItem->setPos(-image->width()/2, -image->height()/2);
        this->fitInView(image->rect(), Qt::KeepAspectRatio);
    }

    imageItem->setImage(image);
    scene->update(image->rect());
}
