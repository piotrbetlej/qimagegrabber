#include "imageitem.h"

ImageItem::ImageItem() :
    QGraphicsItem()
{
    borderPen.setColor(Qt::blue);
    borderPen.setWidth(2);
}

QRectF ImageItem::boundingRect() const
{
    return image.rect();
}

void ImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(borderPen);
    painter->drawImage(0,0, image);
    painter->drawRect(image.rect());
}

void ImageItem::setImage(QImage *newImage)
{
    image = newImage->copy();
}
