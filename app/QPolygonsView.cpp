#ifndef USING_PCH
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <iterator>
#endif /* USING_PCH */

#include "QPolygonsView.H"

//@{
//! \ingroup gui

/*! \class PaintablePolygonList
  \brief PolygonList holding the QPainterPaths with the polygons. */

PaintablePolygonList::PaintablePolygonList()
{
}

PaintablePolygonList::PaintablePolygonList(const PolygonList& pl)
{
  *this = pl;
}

//! Assignment operator, resizes paths vector and assigns empty paths to the elements.
PaintablePolygonList& PaintablePolygonList::operator=(const PolygonList& pl)
{
  *static_cast<PolygonList*>(this) = pl;
  paths.resize(pl.size());
  for(std::vector<QPainterPath>::iterator i = paths.begin(); i != paths.end(); ++i)
    *i = QPainterPath();
  return *this;
}

const QPainterPath& PaintablePolygonList::getConstPainterPath(
    const Polygons::PolygonList::iterator& it)
{
  int d = distance(begin(), it);
  return paths[d];
}

void PaintablePolygonList::setConstPainterPath(
    const Polygons::PolygonList::iterator& it, const QPainterPath& pa)
{
  int d = distance(begin(), it);
  paths[d] = pa;
}

/*! \class PolyImageWidget
 * \brief ImageWidget drawing \a Polygons::PolygonList above the image.
 *
 * Helper class for QPolygonsView.
 */

PolyImageWidget::PolyImageWidget(QWidget* parent): ImageWidget(parent), polygonsList(0)
{
}

/*! Return currently set PolygonList.  \sa setPolygons(Polygons::PolygonList* p)
  \bug Returns pointer to private data.
  */
const Polygons::PolygonList* PolyImageWidget::polygons() const
{
  return polygonsList;
}

/*! What PolygonList to draw.
  \bug Behaves differently from setImage().  Creates its own copy of
  PolygonList and polygons() then returns pointer to the private copy.
  */
void PolyImageWidget::setPolygons(Polygons::PolygonList* p)
{
  if(!polygonsList && p)
  {
    polygonsList = new PaintablePolygonList;
  }
  else if(polygonsList && !p)
  {
    delete polygonsList;
    polygonsList = 0;
  }

  if(polygonsList) *polygonsList = *p;

  update();
}

/*! paintEvent called by Qt engine, calls inherited ImageWidget::paintEvent and
 * then draws lines. */
void PolyImageWidget::paintEvent(QPaintEvent* pe)
{
  ImageWidget::paintEvent(pe);
  if(!dispRealPaintEnabled) return;

  if(polygonsList) {
    QPainter p(this);
    QRect r = pe->rect();
    QRectF rf(QPointF(r.topLeft()) / dispMagnification,
	QSizeF(r.size()) / dispMagnification);

    for(Polygons::PolygonList::iterator i = polygonsList->begin();
	i != polygonsList->end(); i++)
    {
      // when polygon does not interfere with current repainted area, skip it
      QRect polyBoundRect = QRect(i->boundingRect().topLeft()*dispMagnification, 
	      i->boundingRect().bottomRight()*dispMagnification); 
      if(!r.intersects(polyBoundRect))
       	continue;

      p.save();
      p.scale(dispMagnification, dispMagnification);
      p.setPen(QPen(Qt::cyan));
      const QPainterPath& pp = polygonsList->getConstPainterPath(i);
      if(pp.isEmpty())
      {
	QPainterPath newPP;

	Polygons::Polygon::const_iterator j = i->begin();
	newPP.moveTo(i->begin()->x + 0.5, i->begin()->y + 0.5);
	for(; j != i->end(); ++j)
	{
	  if(j->curve_control_point)
	  {
	    QPointF c1 = QPointF(j->x + 0.5, j->y + 0.5);
	    if(++j == i->end()) { qWarning("c2 invalid"); continue; } // sanity check
	    QPointF c2 = QPointF(j->x + 0.5, j->y + 0.5);
	    if(++j == i->end()) { qWarning("c3 invalid"); continue; }
	    QPointF c3 = QPointF(j->x + 0.5, j->y + 0.5);
	    newPP.cubicTo(c1, c2, c3);
	  }
	  else
	  {
	    newPP.lineTo(j->x + 0.5, j->y + 0.5);
	  }
	}

	if(i->isClosed())
	{
	  newPP.closeSubpath();
	}

	polygonsList->setConstPainterPath(i, newPP);
	p.drawPath(newPP);
      }
      else
      {
	p.drawPath(pp);
      }

      p.restore();
      // draw squares
      QBrush brushNormal(Qt::red), brushControl(Qt::blue);
      for(Polygons::Polygon::const_iterator j = i->begin();
	  j != i->end(); ++j)
      {
	QPoint pt(int((j->x + 0.5)*dispMagnification),
	    int((j->y + 0.5)*dispMagnification));
	if(r.contains(pt))
	  p.fillRect(QRect(pt, QSize(3, 3)), 
	    j->curve_control_point ? brushControl : brushNormal);
      }
    }
  }
}

/*! \class QPolygonsView
    \brief Provides scrollable view of an QImage with vectors over it.
 */

/*! Default constructor. 
 */
QPolygonsView::QPolygonsView(QWidget* parent): QImageView(parent)
{
  delete iw;
  iw = new PolyImageWidget();
  setWidget(iw);
}

/*! Gets the polygonList that is drawed over the image.
 * \sa setPolygons(Polygons::PolygonList* p)
 */
const Polygons::PolygonList* QPolygonsView::polygons() const
{
  return static_cast<PolyImageWidget*>(iw)->polygons();
}

/*! Sets the polygonList that is drawed over the image.
 */
void QPolygonsView::setPolygons(Polygons::PolygonList* p)
{
  static_cast<PolyImageWidget*>(iw)->setPolygons(p);
}

//@}
