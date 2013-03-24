#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPen>

class ImageItem : public QGraphicsItem
{
public:
    explicit ImageItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void setImage(QImage *newImage);

private:
    QImage image;
    QPen borderPen;
};

#endif // IMAGEITEM_H
