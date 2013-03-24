#ifndef COLORDETECTOR_H
#define COLORDETECTOR_H

#include <QObject>
#include <QImage>
#include <QRect>

/*
 * Color detector: a simple class to detect that a region of the nput image is red, green or blue
 */

class ColorDetector : public QObject
{
    Q_OBJECT
public:
    explicit ColorDetector(QObject *parent = 0);

    void setDetectPoint(QPoint pt);
    void setRegionRect(QRect rect);
    void setRegionSize(unsigned int regionSize);

    void processImage(QImage* image);

    bool isBlue() const {return m_isBlue;}
    bool isGreen() const {return m_isGreen;}
    bool isRed() const {return m_isRed;}

    int redTreshold() const {return m_redTreshold;}
    int greenTreshold() const {return m_greenTreshold;}
    int blueTreshold() const {return m_blueTreshold;}

    void setRedTreshold(int tres);
    void setGreenTreshold(int tres);
    void setBlueTreshold(int tres);
    
private:
    int m_redTreshold,  m_greenTreshold, m_blueTreshold;
    bool m_isBlue, m_isGreen, m_isRed;

signals:
    
public slots:
    
};

#endif // COLORDETECTOR_H
