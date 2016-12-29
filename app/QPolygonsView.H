#ifndef __QPOLYGONSVIEW_H__
#define __QPOLYGONSVIEW_H__

#ifndef USING_PCH
#include <QPainterPath>
#endif /* USING_PCH */

#include "QImageView.H"
#include "Polygons.H"

class PaintablePolygonList: public Polygons::PolygonList
{
  std::vector<QPainterPath> paths;

  public:
    PaintablePolygonList();
    PaintablePolygonList(const PolygonList& pl);
    PaintablePolygonList& operator=(const PolygonList& pl);
    const QPainterPath& getConstPainterPath(
	const Polygons::PolygonList::iterator& it);
    void setConstPainterPath(const Polygons::PolygonList::iterator& it,
	const QPainterPath& pa);
};

class PolyImageWidget: public ImageWidget
{
  private:
    PaintablePolygonList* polygonsList;

  public:
    PolyImageWidget(QWidget* parent = 0);
    const Polygons::PolygonList* polygons() const;
    void setPolygons(Polygons::PolygonList* p);
    virtual void paintEvent(QPaintEvent* pe);
};

class QPolygonsView: public QImageView
{
  public:
    QPolygonsView(QWidget* parent = 0);
    const Polygons::PolygonList* polygons() const;
    void setPolygons(Polygons::PolygonList* p);
};

#endif
