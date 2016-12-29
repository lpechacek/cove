#ifndef __QIMAGEVIEW_H__
#define __QIMAGEVIEW_H__

#ifndef USING_PCH
#include <QScrollArea>
#include <QWidget>
#endif /* USING_PCH */

class QImage;
class QPaintEvent;

class ImageWidget: public QWidget
{
  protected:
    const QImage* dispImage;
    float dispMagnification;
    bool scalingSmooth;
    bool dispRealPaintEnabled;
    QRect drect;
    virtual void paintEvent(QPaintEvent* pe);
  public:
    ImageWidget(QWidget* parent = 0);
    virtual ~ImageWidget();
    const QImage* image() const;
    void setImage(const QImage* im);
    float magnification() const;
    void setMagnification(float mag);
    bool smoothScaling() const;
    void setSmoothScaling(bool ss);
    bool realPaintEnabled() const;
    void setRealPaintEnabled(bool ss);
    void displayRect(const QRect& r);
};

class QImageView: public QScrollArea
{
  Q_OBJECT

  protected:
    ImageWidget* iw;
    QPoint dragStartPos, dragCurPos;
    QRect zoomRect;
    enum OperatingMode { MOVE, ZOOM_IN, ZOOM_OUT } opMode;
    int lastSliderHPos, lastSliderVPos;
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
  public:
    QImageView(QWidget* parent = 0);
    void reset();
    virtual ~QImageView();
    const QImage* image() const;
    void setImage(const QImage* im);
    float magnification() const;
    void setMagnification(float mag);
    bool smoothScaling() const;
  public slots:
    void setMoveMode();
    void setZoomInMode();
    void setZoomOutMode();
    void setOrigSize();
    void setSmoothScaling(bool ss);
  signals:
    void magnificationChanged(float oldmag, float mag);
};

#endif
