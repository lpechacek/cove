#ifndef USING_PCH
#include <math.h>
#include <QAction>
#include <QBitmap>
#include <QBrush>
#include <QCursor>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QWidget>
#endif /* USING_PCH */

#include "QImageView.H"

//@{
//! \ingroup gui

/*! \class ImageWidget
  \brief QWidget that draws a QImage on its face. 
 */

/*! \var const QImage* ImageWidget::dispImage 
  QImage to be displayed.
  \sa setImage
*/

/*! \var float ImageWidget::dispMagnification 
  Magnification factor of image display.
  \sa setMagnification
*/

/*! \var bool ImageWidget::scalingSmooth 
  Whether the Qt::SmoothTransformation options should be used when scaling
  image.
  \sa setSmoothScaling
*/

/*! \var bool ImageWidget::dispRealPaintEnabled 
  Do or don't do real paint when moving the image.  Real paint is disabled as
  long as the user moves the image.
  \sa setRealPaintEnabled
*/

/*! \var QRect ImageWidget::drect 
  Rectangle that is displayed when doing zoom-in operation.
*/


//! Default constructor.
ImageWidget::ImageWidget(QWidget* parent): QWidget(parent), dispImage(0),
					   dispMagnification(1),
					   scalingSmooth(false),
					   dispRealPaintEnabled(true),
					   drect(0, 0, 0, 0)
{
}

//! Destructor.
ImageWidget::~ImageWidget()
{
}

//! Painting event handler.
void ImageWidget::paintEvent(QPaintEvent* pe)
{
  if(!dispRealPaintEnabled) return;
  QPainter p(this);
  QRect r = pe->rect();

  // if there is an dispImage and is not a null dispImage draw it
  if(dispImage && !dispImage->isNull())
  {
    // 1-bit deep dispImages are drawn in different way by painter.  Their native
    // colors are ignored.
    if(dispImage->depth() == 1)
    {
      p.setBackground(QBrush(dispImage->color(0)));
      p.setPen(dispImage->color(1));
    }

    // adjust coordinates
    int w = int(ceil(r.width()/dispMagnification))+1,
	h = int(ceil(r.height()/dispMagnification))+1,
	x = int(floor(r.x()/dispMagnification)), 
	y = int(floor(r.y()/dispMagnification));

    // create cut of the original dispImage
    // Workaround Qt4 bug in copy(QRect(...))
    if(y+h > dispImage->height()) h = dispImage->height()-y;
    QImage copy = dispImage->copy(QRect(x, y, w, h));

    if(!copy.isNull())
    {
      // Gray out the image when widget is disabled.
      if(!isEnabled())
      {
	// copy = copy.convertToFormat(copy.format(), Qt::MonoOnly|Qt::AvoidDither);
	// Qt 4 workaround - MonoOnly doesn't work
	if(copy.depth() == 32)
	{
	  int p = copy.width()*copy.height();
	  QRgb* b = (QRgb*)copy.bits();
	  while(p--)
	  {
	    int I = qGray(*b);
	    *b++ = qRgb(I, I, I);
	  }
	}
	else
	{
	  QVector<QRgb> ct = copy.colorTable();
	  for(QVector<QRgb>::iterator i = ct.begin(); i != ct.end(); ++i)
	  {
	    int I = qGray(*i);
	    *i = qRgb(I, I, I);
	  }
	  copy.setColorTable(ct);
	}
      }

      // scale it
      if(dispMagnification != 1) 
      { 
	copy = copy.scaled(int(w*dispMagnification), int(h*dispMagnification),
	    Qt::IgnoreAspectRatio,
	    scalingSmooth ? Qt::SmoothTransformation : Qt::FastTransformation);
      }

      // and draw the copy onto widget
      p.drawImage(QPoint(int(x*dispMagnification), int(y*dispMagnification)), copy);
    }
  }

  if(!drect.isNull())
  {
    p.setPen(QColor(Qt::magenta));
    p.setBrush(QColor(255, 128, 255, 64));
    p.drawRect(drect);
  }
}

//! What dispImage should be viewed.
void ImageWidget::setImage(const QImage* im)
{
  dispImage = im;
  setMagnification(magnification());
  update();
}

//! Returns pointer to currently displayed bitmap.
const QImage* ImageWidget::image() const
{
  return dispImage;
}


//! What dispMagnification should be used. 1 == no dispMagnification, < 1 size reduction.
void ImageWidget::setMagnification(float mag)
{
  if(dispImage)
  {
    int w = static_cast<int>(mag*dispImage->width()), 
	h = static_cast<int>(mag*dispImage->height());
    setMinimumSize(w, h);
    resize(w, h);
  }
  dispMagnification = mag;
}

/*! Returns current magnification used.
  \sa setMagnification */
float ImageWidget::magnification() const
{
  return dispMagnification;
}

/*! Use smooth scaling (QImage::smoothScale). 
 \sa scalingSmooth */
void ImageWidget::setSmoothScaling(bool ss)
{
  bool doupdate = scalingSmooth^ss;
  scalingSmooth = ss;
  if(doupdate) update();
}

/*! Returns value of the scalingSmooth flag.
  \sa setSmoothScaling */
bool ImageWidget::smoothScaling() const
{
  return scalingSmooth;
}

/*! Do or do not real do painting.  When set to false, paints only background. 
 \sa dispRealPaintEnabled */
void ImageWidget::setRealPaintEnabled(bool ss)
{
  dispRealPaintEnabled = ss;
  if(dispRealPaintEnabled) update();
}

/*! Returns value of the scalingSmooth flag.
  \sa setRealPaintEnabled */
bool ImageWidget::realPaintEnabled() const
{
  return dispRealPaintEnabled;
}

/*! Sets displayed zoom-in rectangle.
  \sa drect */
void ImageWidget::displayRect(const QRect& r)
{
  QRect updreg = r.adjusted(-1, -1, 2, 2);
  
  if(drect.isValid())
    updreg |= drect.adjusted(-1, -1, 2, 2);

  drect = r;
  update(updreg);
}

/*! \class QImageView
    \brief Provides scrollable view of an QImage. 
 */

/*! \var ImageWidget* QImageView::iw 
 Pointer to currently viewed image. \sa setImage. */
/*! \var QPoint QImageView::dragStartPos 
 Point where dragging started when zooming in. */
/*! \var QPoint QImageView::dragCurPos 
 Current point where dragging is when zooming in. */
/*! \var QRect QImageView::zoomRect 
 THe light magenta semitransparent rectangle used when zoming in. */
/*! \var OperatingMode QImageView::opMode 
 Current operation mode as selected in context menu. */
/*! \var int QImageView::lastSliderHPos 
 Value of horizontal slider used when moving image. */
/*! \var int QImageView::lastSliderVPos 
 Value of vertical slider used when moving image. */
/*! \enum QImageView::OperatingMode 
  Operation mode as selected in context menu.
  \sa opMode */
/*! \var QImageView::OperatingMode QImageView::MOVE 
 Move mode. */
/*! \var QImageView::OperatingMode QImageView::ZOOM_IN 
 Zoom in mode. */
/*! \var QImageView::OperatingMode QImageView::ZOOM_OUT 
 Zoom out mode. */
/*! \fn void QImageView::magnificationChanged(float oldmag, float mag)
  Signal emitted when zoom ratio changes. */

//! Default constructor.
QImageView::QImageView(QWidget* parent):
 QScrollArea(parent), iw(new ImageWidget(this))
{
  setWidget(iw);
  reset();
  QAction* a;
  addAction(a = new QAction(tr("Move"), this));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setMoveMode()));
  addAction(a = new QAction(tr("Zoom in"), this));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setZoomInMode()));
  addAction(a = new QAction(tr("Zoom out"), this));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setZoomOutMode()));
  addAction(a = new QAction(this));
  a->setSeparator(true);
  addAction(a = new QAction(tr("Original size"), this));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setOrigSize()));
  addAction(a = new QAction(tr("Smooth scaling"), this));
  a->setCheckable(true);
  connect(a, SIGNAL(toggled(bool)), this, SLOT(setSmoothScaling(bool)));
  setContextMenuPolicy(Qt::ActionsContextMenu);
}

//! Resets QImaeView into its initial state.  I.e. move mode, magnification 1, scrollbars 0.
void QImageView::reset()
{
  setMoveMode();
  setMagnification(1);
  horizontalScrollBar()->setValue(0);
  verticalScrollBar()->setValue(0);
}

QImageView::~QImageView()
{
  if(iw) delete iw;
}

//! Mouse wheel based zooming.
void QImageView::wheelEvent(QWheelEvent* event)
{
  static QPoint accumulator;
  static int stepsize = 8*15*2;

  accumulator += event->angleDelta();
  if (abs(accumulator.y()) >= stepsize)
  {
    int sgn = accumulator.y()/abs(accumulator.y());
    accumulator.ry() -= stepsize * sgn;
    double nm = magnification() * pow(2, sgn);
    if (nm < 32 && nm > 1/32)
      setMagnification(nm);
  }
}

//! Handles mouse click.
void QImageView::mousePressEvent(QMouseEvent* event)
{
  switch(opMode)
  {
    case MOVE:
      if(event->button() == Qt::LeftButton)
      {
	dragStartPos = event->pos();
	iw->setRealPaintEnabled(false);
	lastSliderHPos = horizontalScrollBar()->value();
	lastSliderVPos = verticalScrollBar()->value();
      }
      break;

    case ZOOM_IN:
      if(event->button() == Qt::LeftButton)
      {
	dragStartPos = event->pos();
      }
      break;

    case ZOOM_OUT:
      if(event->button() == Qt::LeftButton)
      {
	double mwm = viewport()->width()/iw->width();
	double mhm = viewport()->height()/iw->height();
	double nm = magnification()/2;
    if(nm > 1/32 && (nm <= 1 || nm >= mwm || nm >= mhm))
	  setMagnification(nm);
      }
  }
}

//! Handles mouse drag when moving image or zooming in.
void QImageView::mouseMoveEvent(QMouseEvent* event)
{
  switch(opMode)
  {
    case MOVE:
      if(event->buttons() & Qt::LeftButton)
      {
	QPoint pixmapOffset = event->pos() - dragStartPos;
	verticalScrollBar()->setValue(lastSliderVPos-pixmapOffset.y());
	horizontalScrollBar()->setValue(lastSliderHPos-pixmapOffset.x());
      }
      break;

    case ZOOM_IN:
      if(event->buttons() & Qt::LeftButton
	  && (event->pos() - dragStartPos).manhattanLength() > 5)
      {
	dragCurPos = event->pos();

	QRect vportrect = viewport()->rect();
	if(!vportrect.contains(dragCurPos))
	{
	  int dx = 0, dy = 0, t;
	  if((t = dragCurPos.x() - vportrect.right()) > 0)
	    dx -= t;
	  else 
	    if((t = dragCurPos.x() - vportrect.left()) < 0)
	      dx -= t;

	  if((t = dragCurPos.y() - vportrect.bottom()) > 0)
	    dy -= t;
	  else 
	    if((t = dragCurPos.y() - vportrect.top()) < 0)
	      dy -= t;

	  QPoint d = iw->pos() + QPoint(dx, dy);
	  if(d.x() < (t = viewport()->width() - iw->width()))
	    d.setX(t);
	  else
	    if(d.x() > 0) 
	      d.setX(0);
	  if(d.y() < (t = viewport()->height() - iw->height()))
	    d.setY(t);
	  else
	    if(d.y() > 0) 
	      d.setY(0);
	  dragStartPos += d - iw->pos();
	  iw->move(d);
	}

	QPoint dist = dragStartPos - dragCurPos;
	int x, y, w, h;

	if(dist.x() > 0) 
	{ x = dragCurPos.x(); w = dist.x(); }
	else
	{ x = dragStartPos.x(); w = -dist.x(); }

	if(dist.y() > 0) 
	{ y = dragCurPos.y(); h = dist.y(); }
	else
	{ y = dragStartPos.y(); h = -dist.y(); }

	zoomRect = QRect(QPoint(x, y) - iw->pos(), QSize(w, h))
      .intersected(iw->rect().adjusted(0, 0, -1, -1));
	iw->displayRect(zoomRect);
      }

    case ZOOM_OUT: ;
  }
}

//! Handles end-of-drag.
void QImageView::mouseReleaseEvent(QMouseEvent* event)
{
  switch(opMode)
  {
    case MOVE:
      if(event->button() == Qt::LeftButton)
      {
	QPoint pixmapOffset = event->pos() - dragStartPos;
	iw->setRealPaintEnabled(true);
	verticalScrollBar()->setValue(lastSliderVPos-pixmapOffset.y());
	horizontalScrollBar()->setValue(lastSliderHPos-pixmapOffset.x());
      }
      break;

    case ZOOM_IN:
      if(event->button() == Qt::LeftButton)
      {
	int dx, dy;
	if((event->pos() - dragStartPos).manhattanLength() > 5
	    && zoomRect.width() && zoomRect.height())
	{
	  double m = viewport()->width()/zoomRect.width(),
		 n = viewport()->height()/zoomRect.height(),
		 rmag = m>n?n:m; // minimum of those two
	  // max possible 2^n magnif. with n integer
	  double magmul = pow(2, trunc(log(rmag)/log(2)));
	  if(floor(magmul) == 0.0) magmul = 1;
	  if(magnification()*magmul > 16) magmul = 16.0/magnification();
	  rmag = magnification()*magmul;
	  QPoint zrc = zoomRect.center() + iw->pos();
	  dx = int(magmul*(zrc.x() - viewport()->size().width()/2));
	  dy = int(magmul*(zrc.y() - viewport()->size().height()/2));
	  iw->displayRect(QRect());
	  setMagnification(rmag);
	} 
	else
	{
	  int magmul = magnification()<16 ? 2 : 1;
	  setMagnification(magnification()*magmul);
	  dx = magmul*(event->pos().x() - viewport()->size().width()/2);
	  dy = magmul*(event->pos().y() - viewport()->size().height()/2);
	}
	verticalScrollBar()->setValue(verticalScrollBar()->value() + dy);
	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + dx);
      }
      break;

    case ZOOM_OUT: ;
  }
}

//! What dispImage should be viewed.
void QImageView::setImage(const QImage* im) 
{
  iw->setImage(im);
}

//! return pointer to currently displayed bitmap.
const QImage* QImageView::image() const
{
  return iw->image();
}

//! What dispMagnification should be used. 1 == no dispMagnification, < 1 size reduction.
void QImageView::setMagnification(float mag)
{
  double oldmag = iw->magnification(),
	 magratio = mag/oldmag,
	 oneMmagratio = 1 - magratio;
  int w = viewport()->size().width(),
      h = viewport()->size().height();
  int dx = (int)((iw->pos().x()-w/2.0)*oneMmagratio),
      dy = (int)((iw->pos().y()-h/2.0)*oneMmagratio);
  iw->setMagnification(mag);
  verticalScrollBar()->setValue(verticalScrollBar()->value() + dy);
  horizontalScrollBar()->setValue(horizontalScrollBar()->value() + dx);
  if(oldmag != mag)
    emit magnificationChanged(oldmag, mag);
}

//! Current magnification used.
float QImageView::magnification() const
{
  return iw->magnification();
}

//! Use smooth scaling (QImage::smoothScale).
void QImageView::setSmoothScaling(bool ss)
{
  iw->setSmoothScaling(ss);
}

//! Returns true when smooth scaling is enabled.
bool QImageView::smoothScaling() const
{
  return iw->smoothScaling();
}

//! Slot for context menu.
void QImageView::setMoveMode()
{
  iw->setCursor(QCursor(Qt::SizeAllCursor));
  opMode = MOVE;
}


//! Slot for context menu.
void QImageView::setZoomInMode()
{
#define zoomin_width 32
#define zoomin_height 32
  static unsigned char zoomin_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x38, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
    0x80, 0x00, 0x02, 0x00, 0x40, 0x10, 0x04, 0x00, 0x40, 0x10, 0x04, 0x00,
    0x20, 0x10, 0x08, 0x00, 0x20, 0xfe, 0x08, 0x00, 0x20, 0x10, 0x08, 0x00,
    0x40, 0x10, 0x04, 0x00, 0x40, 0x10, 0x04, 0x00, 0x80, 0x00, 0x02, 0x00,
    0x00, 0x01, 0x07, 0x00, 0x00, 0xc6, 0x0e, 0x00, 0x00, 0x38, 0x1c, 0x00,
    0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xe0, 0x00,
    0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define zoomin_mask_width 32
#define zoomin_mask_height 32
  static unsigned char zoomin_mask_bits[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0xff,
    0x01,0x00,0x80,0xc7,0x03,0x00,0xc0,0x11,0x07,0x00,0xe0,0x38,0x0e,0x00,0xe0,
    0x38,0x0e,0x00,0x70,0xfe,0x1c,0x00,0x70,0xff,0x1d,0x00,0x70,0xfe,0x1c,0x00,
    0xe0,0x38,0x0e,0x00,0xe0,0x38,0x0e,0x00,0xc0,0x11,0x07,0x00,0x80,0xc7,0x0f,
    0x00,0x00,0xff,0x1f,0x00,0x00,0xfe,0x3e,0x00,0x00,0x38,0x7c,0x00,0x00,0x00,
    0xf8,0x00,0x00,0x00,0xf0,0x01,0x00,0x00,0xe0,0x03,0x00,0x00,0xc0,0x03,0x00,
    0x00,0x80,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  iw->setCursor(QCursor(
	QBitmap::fromData(QSize(zoomin_width, zoomin_height),
	  zoomin_bits, QImage::Format_MonoLSB),
	QBitmap::fromData(QSize(zoomin_mask_width, zoomin_mask_height),
	  zoomin_mask_bits, QImage::Format_MonoLSB), 13, 13));
  opMode = ZOOM_IN;
}


//! Slot for context menu.
void QImageView::setZoomOutMode()
{
#define zoomout_width 32
#define zoomout_height 32
  static unsigned char zoomout_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x38, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
    0x80, 0x00, 0x02, 0x00, 0x40, 0x00, 0x04, 0x00, 0x40, 0x00, 0x04, 0x00,
    0x20, 0x00, 0x08, 0x00, 0x20, 0xfe, 0x08, 0x00, 0x20, 0x00, 0x08, 0x00,
    0x40, 0x00, 0x04, 0x00, 0x40, 0x00, 0x04, 0x00, 0x80, 0x00, 0x02, 0x00,
    0x00, 0x01, 0x07, 0x00, 0x00, 0xc6, 0x0e, 0x00, 0x00, 0x38, 0x1c, 0x00,
    0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xe0, 0x00,
    0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define zoomout_mask_width 32
#define zoomout_mask_height 32
  static unsigned char zoomout_mask_bits[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0xff,
    0x01,0x00,0x80,0xc7,0x03,0x00,0xc0,0x01,0x07,0x00,0xe0,0x00,0x0e,0x00,0xe0,
    0x00,0x0e,0x00,0x70,0xfe,0x1c,0x00,0x70,0xff,0x1d,0x00,0x70,0xfe,0x1c,0x00,
    0xe0,0x00,0x0e,0x00,0xe0,0x00,0x0e,0x00,0xc0,0x01,0x07,0x00,0x80,0xc7,0x0f,
    0x00,0x00,0xff,0x1f,0x00,0x00,0xfe,0x3e,0x00,0x00,0x38,0x7c,0x00,0x00,0x00,
    0xf8,0x00,0x00,0x00,0xf0,0x01,0x00,0x00,0xe0,0x03,0x00,0x00,0xc0,0x03,0x00,
    0x00,0x80,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  iw->setCursor(QCursor(
	QBitmap::fromData(QSize(zoomout_width, zoomout_height),
	  zoomout_bits, QImage::Format_MonoLSB),
	QBitmap::fromData(QSize(zoomout_mask_width, zoomout_mask_height),
	  zoomout_mask_bits, QImage::Format_MonoLSB), 13, 13));
  opMode = ZOOM_OUT;
}

//! Slot for context menu. Sets magnification to 1.
void QImageView::setOrigSize()
{
  setMagnification(1);
}

//@}
